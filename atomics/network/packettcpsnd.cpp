#include "packettcpsnd.h"
////IMPORTANTE: NO MODIFICAR POR EL EDITOR DE PD PUES RECORTA CODIGO (EN EL APARTADO DE &ext)!
void packettcpsnd::init(double t,...) {
	BaseSimulator::init(t);


	va_list parameters;
	va_start(parameters,t);

	////////////////////////////////////////////////////////////////
	// Prefixes
	// BUF_ : Pointer into the SEND BUFFER
	// WND_ : Pointer into SEND Sliding WINDOW
	// SEQ_ : Packet Sequence NUMBER
	// APP_ : Relative to the APPlication (upper) Layer
	// T_	: Time

	char* fvar;
	fvar = va_arg(parameters,char*);
	// MAX Segment Size
	MSS = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param MSS: %u \n", MSS);

	fvar = va_arg(parameters,char*);
	this->ipSender = fvar;
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: ipSender: %s  \n", this->getFullName().data(), this->ipSender.data());

	fvar = va_arg(parameters,char*);
	this->ipReceiver = fvar;
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: ipReceiver: %s  \n", this->getFullName().data(), this->ipReceiver.data());

	fvar = va_arg(parameters,char*);
	this->portSender = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: portSender: %d  \n", this->getFullName().data(), this->portSender);

	fvar = va_arg(parameters,char*);
	this->portReceiver = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: portReceiver: %d  \n", this->getFullName().data(), this->portReceiver);

	fvar = va_arg(parameters,char*);
	// MAX Congestion Window (in packets) inf==NONE, Typical: 64
	WND_CWND_MAX = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param WND_CWND_MAX: %i \n",WND_CWND_MAX);

	fvar = va_arg(parameters,char*);
	// Experimental Slow Start, 0==OFF
	WND_SSTHRESH = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param WND_SSTHRESH: %i \n",WND_SSTHRESH);

	fvar = va_arg(parameters,char*);
	// Typical value 0.125. RTT_alfa==0 => No moving average for RTT, only instantaneous measurements
	RTT_alfa = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param RTT_alfa: %g \n",RTT_alfa);

	fvar = va_arg(parameters,char*);
	// Initial Calculated Round Trip Time
	T_RTT = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param Initial T_RTT: %g \n",T_RTT);

	fvar = va_arg(parameters,char*);
	// Typical value 3
	DUP_ACK_LIMIT = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param DUP_ACK_LIMIT: %u \n",DUP_ACK_LIMIT);

	fvar = va_arg(parameters,char*);
	// T_FORCED_RTO>=0 => Forces RTO instead of calculating it
	T_FORCED_RTO = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param T_FORCED_RTO: %g \n",T_FORCED_RTO);

	fvar = va_arg(parameters,char*);
	// Arbitrary Practical Criteria. If 1 mseg. (i.e. Limits (P)acket (T)hroughput to T=1 KPps, which at 1KByte per Segment renders T=1MBps of user data. or T=8Mbps of bandwidth
	// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
	INTERPACKET_SND_TIME = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param INTERPACKET_SND_TIME: %g \n",INTERPACKET_SND_TIME);

	fvar = va_arg(parameters,char*);
	// Arbitrary Practical Criteria. 0.1 mseg. Time to wait
	// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
	INTER_REQ_TIME = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] tcpSND: Param INTER_REQ_TIME: %g \n",INTER_REQ_TIME);

	// EXPERIMENTAL:
	NchangeFactor=1;  // 1=No Effect (i.e. this is disabled)
	NchangeFactorInit=-1;//NchangeFactorInit=10; // Matias: dissable this thing
	NchangeFactorEnd=-1;//NchangeFactorEnd=20;
	//

	this->logger->initSignals(std::map<std::string, int>{
		{"cwnd", LOG_LEVEL_PRIORITY},
		{"timeout", LOG_LEVEL_IMPORTANT},
		{"tripleDupACK", LOG_LEVEL_PRIORITY},
		{"dupACK", LOG_LEVEL_PRIORITY},
		{"measuredRTT", LOG_LEVEL_PRIORITY},
		{"RTO", LOG_LEVEL_FULL_LOGGING},
		{"RTO_dev", LOG_LEVEL_FULL_LOGGING},
		{"packetdrop", LOG_LEVEL_DEBUG},
		{"calcRTT", LOG_LEVEL_DEBUG},
		{"receiveAck", LOG_LEVEL_DEBUG},
		{"packetsend", LOG_LEVEL_DEBUG},
		{"ECNMark", LOG_LEVEL_DEBUG}
	});

	//Para logueo en PCAP
	//debugMsg(LOG_LEVEL_DEBUG, "[INIT] tcpSND[init] LoggerToPcap: PCAPTCPSND.pcap\n",t);
	//char pcapfilelog[100]; strcpy (pcapfilelog,"dev_");strcat (pcapfilelog, this->getFullName().data());strcat (pcapfilelog,"_log.pcap");
	//myPcap = new LoggerToPcap(65535, (char *)pcapfilelog);
	//myPcap->openPcap();

	//Para control de logueo
	CtrolCongestion_Record_log = false;
	SndPktAcks_Record_log = false;
	pout = NULL;
	////////////////////////////////////////
	WND_firsttime_SSTHRESH = true;
	BUF_Fully_SNT = false ; //
	BUF_SIZE = 0 ;			//
	WND_Fully_SNT = false ; //
	WND_NXT_SND = 0 ;		// Next position to be sent (when allowed)
	// -> It can overflow and be 1 ahead of the current SEND BUFFER size
	//SEQ_NEXT_Expected_ACK=1 ;//
	SEQ_NEXT_Expected_ACK=MSS + 1;//501 ;//KIKE-Prueba: SEQ_ini + Tama?o del paquete
	SEQ_LAST_Valid_ACK = 1 ;	// Last valid ACKnowledged packet sequence
	WND_DUP_ACK = 0 ;		// Amount of Duplicate ACKs received so far
	WND_CWND = 1 ;		    // 0 ;	// Congestion Window (in packets)
	WND_CWND_precise = 1 ;	// Congestion Window (with fractions)
	SEQ_CYCLE_ACK = 0;		// Indicates which sent SEQ number will be considered to determine a RTT CYCLE
	// -> This will be the expected ACK capable of growing W_CWND 1 unit
	SEQ_NXT = 1;			// Next Sequence Number to use for sending

	ReceivedACKedFromPeerLayer = 0;			// This is the most recent ACK number that "the other end is confirming as valid"
	SendACKtoPeerLayer = 0;			// This is the most actual ACK number "that can be confirmed to the other end"
	T_MEASURED_RTT =  0;		// [[PENDING: init to something standard
	T_RTT_DEVIATION = 0; 		// [[PENDING: init to something standard

	if (T_FORCED_RTO>=0) {
		T_RTO = T_FORCED_RTO ;                  // FORCED
	} else {
		T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
	}
	T_RTO = std::max(MIN_RTO, T_RTO); // enforces min RTO

	this->logger->logSignal(t,T_RTO,"RTO");
	debugMsg(LOG_LEVEL_DEBUG, "[INIT] tcpSND[init] T_RTT(%g), T_RTO(%g)\n",T_RTT,T_RTO);
	////////////////////////////////////////////////////////////////


	// State Transition
	mystate = tr_REQxAPP;	// Inmediately asks for the first new packet
	sigma = 0;  			//

	return;
}

void packettcpsnd::dint(double t) {
	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[int]: state: mystate=%i ; sigma=%g;  \n", t, this->getFullName().data(), mystate, sigma);

	// According to the State do:
	switch (mystate)
	{
	case tr_REQxAPP: // Asks for more data from the upper layer
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[InT]@tr_REQxAPP:  p->lastQueueSizeAfterLeaving=%g ; BUF_SIZE=%i  ; WND_CWND=%g ;\n", t, this->getFullName().data(), p?p->lastQueueSizeAfterLeaving:-1, BUF_SIZE, WND_CWND);

		// State Transition
		mystate=WAITxAPP; // wait for new data from upper layer
		sigma=INTER_REQ_TIME; // for a short time

		break;
	case WAITxAPP: // Waits for data from the upper layer
		if (BUF_SIZE>0) { // Buffer NOT Empty
			// State Transition
			mystate=SENDING;
			sigma=INTERPACKET_SND_TIME;
			//KIKE//Prueba: intento que si tengo data enviarla inmediatamente //KIKE-MEJORA
			//sigma=0;//(==INTERPACKET_SND_TIME=0)(K2)
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: to SENDING during %g \n",t,sigma);
		} else {		// Buffer Empty, nothing to do, stays waiting forever
			// State Transition
			mystate=WAITxAPP;
			sigma = std::numeric_limits<double>::infinity();
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: to WAITxAPP during %g \n",t,sigma);
		}
		break;
	case SENDING: // Flushing the sender window
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@SENDING SEQ_CYCLE_ACK(%u)\n",t,SEQ_CYCLE_ACK);

		if (SEQ_CYCLE_ACK==0) {
			// PENDING: SEQ_CYCLE_ACK=pout->myprotocol.headerFields[SEQ];
			//SEQ_CYCLE_ACK = pout->auxiliar_SEQ ;
			//KIKE-Prueba: considero la long en bytes de los paquetes
			SEQ_CYCLE_ACK = pout->auxiliar_SEQ + MSS;//500; //+ packet length
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint]: SENDING. SEQ_CYCLE_ACK=0 set to: %u \n",t, this->getFullName().data(), SEQ_CYCLE_ACK);
		};

		if (BUF_SIZE>0 && WND_NXT_SND<=BUF_SIZE && WND_NXT_SND<=WND_CWND) { // Send Buffer ! empty AND ! completely sent

			WND_NXT_SND++ ; // para saber donde apunta el buffer de salida (incrementa WND_NXT_SND:2)
			mystate=SENDING;	// State Transition
			sigma=INTERPACKET_SND_TIME;


			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint-SENDNIG]: Send buffer NOT empty (BUF_SIZE(%u)> 0 && BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g)), forwarding after sigma=%g.  \n", t, this->getFullName().data(),
					BUF_SIZE,BUF_SIZE, WND_NXT_SND,WND_CWND, sigma);
		} else {		// Send Buffer empty
			mystate=WAITxACK;	// State Transition
			sigma=T_RTO;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint-SENDING]: Send buffer empty. WAITxACK during RTO=%g \n", t, this->getFullName().data(), sigma);
		}

		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@SENDING: to mystate=%u y sigma=%g\n",t,mystate, sigma);
		break;

	case WAITxACK: // Waiting For ACK after having flushed the whole CWND
		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[InT]@WAITxACK: TCP TIMEOUT -> Try to retransmit buffer. Current values: sigma=%g, WND_CWND=%g, WND_SSTHRESH=%d, WND_DUP_ACK=%d, T_RTO=%g\n", t, this->getFullName().data(),
				sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK, T_RTO);

		WND_DUP_ACK=0 ; // [[ to Reconsider
		// CLASSICAL OPTION:
		// WND_CWND = 1;
		// MODIFIED OPTION: (Treats TO as a DUP ACK) //KIKE-MEJORA: cuando hay timeout debe ir a SS comenzando con cwnd = 1 MSS
		//WND_CWND = abs(WND_CWND / 2);

		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[InT] TCP TIMEOUT Init \"Slow Start\"(SS) WND_CWND tiene valor (%g) \n",t, this->getFullName().data(),WND_CWND);
		//set SSTHRESH a la mitad de los paquetes en vuelo y a CWND en 1 MSS
		WND_SSTHRESH = WND_CWND / 2;
		WND_SSTHRESH = std::max(1, WND_SSTHRESH); // Matias: fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)

		WND_CWND = 1;
		WND_firsttime_SSTHRESH=false;
		debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[InT]@WAITxACK: TCP TIMEOUT -> se reduce a la mitad WND_SSTHRESH(%d),set WND_CWND(%g)=1, set WND_DUP_ACK(%d)=0 \n",t, this->getFullName().data(), WND_SSTHRESH, WND_CWND, WND_DUP_ACK);
		// if (WND_CWND<1) //Matias fix: it was set 'WND_CWND=1', so the if does not make sense.
		//		{
		//			WND_CWND=1;			//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@WAITxACK: WND_CWND<1 entonces seteo WND_CWND=1\n",t);
		//		};
		WND_CWND_precise = WND_CWND ;

		this->logger->logSignal(t,(double)WND_CWND,"cwnd");
		this->logger->logSignal(t, 1,"timeout");

		debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT] TCP TIMEOUT Init \"Slow Start\"(SS) WND_CWND(%g), WND_NXT_SND(%d) \n",t,WND_CWND, WND_NXT_SND);
		//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@WAITxACK: LoggerToScilab cwnd=%_IS_DUP_ACK_LIMIT_PACKETu\n",t,WND_CWND);
		this->logger->logSignal(t,(double)WND_CWND,"cwnd");
		WND_NXT_SND = 1 ;	   // Update de pointer , NORMAL case
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: WAITxACK (WND_NXT_SND set a valor %u) \n",t,WND_NXT_SND);
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: BUF_SIZE %u WND_CWND %u WND_NXT_SND %u \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND);
		BUF_Fully_SNT = false ; 	//
		WND_Fully_SNT = false ; 	//

		SEQ_LAST_Valid_ACK = -1; 	// prevents identifying the next dup acks that might be on-fly as a repetition of this condition (dup ack is detected only once inside the same cycle)
		SEQ_CYCLE_ACK = 0;  		// A new measurement cycle starts all over again
		// This Signals the SENDING state that
		// the next SEQ number will represent a CYCLE_ACK
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: SEQ_CYCLE_ACK set to: %u (open to next) \n",t,SEQ_CYCLE_ACK);

		T_RTO = 2 * T_RTO; // exponential back-off

		if (BUF_SIZE<WND_CWND) {
			// State Transition
			mystate=tr_REQxAPP; 	// Check if: there is DATA to send ASAP ?
			sigma=0;
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: to tr_REQxAPP during %g \n",t,sigma);
		} else {
			// State Transition
			mystate=SENDING;		// Start re-sending from the updated pointer
			sigma=0;
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: to SENDING during %g \n",t,sigma);
		};

		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[InT]@WAITxACK: TCP TIMEOUT occurred. State set to retransmit buffer. New values: sigma=%g, WND_CWND=%g, WND_SSTHRESH=%d, WND_DUP_ACK=%d, T_RTO=%g\n",t, this->getFullName().data(),
				    sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK, T_RTO);
		break;

	default:
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: Error: Do not know what to do... \n",t);
		break;
	}
	/* //KIKE-original
		int first; int last;
		if (BUF_SIZE>0) {
				first=(int)(SendBuffer.at(0)->getId()) ;
				last=(int)(SendBuffer.at(BUF_SIZE-1)->getId()) ;
			} else {
				first=0;
				last=0;
			};

		if(SndPktAcks_Record_log)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: BUF_SIZE %u WND_CWND %g WND_NXT_SND %u (%i-%i) \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND,first,last);
	 */
	int first; int last; int id;
	int snd_una; int snd_nxt; double maxWindow; //double effectiveWin;//KIKE//Prueba
	if (BUF_SIZE>0) {
		first=(int)(SendBuffer.at(0)->getId()) ;//KIKE//Prueba: lo veo equivalente a "snd_una"
		last=(int)(SendBuffer.at(BUF_SIZE-1)->getId()) ;//KIKE//Prueba: es el ultimo paquete encolado y necesito el ultimo enviado para "snd_nxt"
		maxWindow = std::min(132.0, WND_CWND);//rwnd,cwnd
		snd_una = first;//ultimo no reconocido.
		if(pout==NULL){
			snd_nxt = last;
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]:a) set snd_nxt=last\n",t);
		}
		else{
			snd_nxt = (pout->getId()) + 1; ////El ultimo Tx + 1 pero compenso con el first(el proximo a enviar)//last;//SEQ_NXT;//(pout->getId()) + 1;
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]:b) set snd_nxt=pout->getId()\n",t);
		}
		effectiveWin = maxWindow - (snd_nxt - snd_una);
	} else {
		first=0;
		last=0;
		effectiveWin = std::min(132.0, WND_CWND);//131.07
	};
	char *sEf = (char *)((effectiveWin < 0) ? "NEGATIVO" : "Positivo");
	/*if(SndPktAcks_Record_log){
			debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: BUF_SIZE %u WND_CWND %g WND_NXT_SND %u (%i-%i) \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND,first,last);
			debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: (snd_nxt(%u) - snd_una(%u)) maxWindow %g effectiveWin %g(%s) \n",t, snd_nxt, snd_una, maxWindow, effectiveWin,sEf);
		}*/

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[int]: New state: mystate=%i ; sigma=%g;  \n", t, this->getFullName().data(), mystate, sigma);

	return;
}
void packettcpsnd::dext(Event x, double t) {
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: [EXT] \n",t);

	// For ANY current State do:

	bool _IS_DATA_PACKET ;
	bool _IS_ACK_PACKET ;

	bool _IS_VALID_ACK_PACKET ;
	bool _IS_DUP_ACK_LIMIT_PACKET ;

	if (x.port==0)	{ // Data from upper layer (application)
		_IS_DATA_PACKET = true;
		_IS_ACK_PACKET = false;

		p = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event

		// Add TCP protocol (with payloadSize=0, as we are not adding any payload here)
		auto tcpProt = std::make_shared<TcpIpProtocol>(0, this->ipSender, this->ipReceiver, this->portSender, this->portReceiver);
		p->addProtocol(tcpProt);


		p->auxiliar_SEQ = SEQ_NXT ; // PENDING: Remove
		////KIKE-Prueba
		//SEQ_NXT++ ;
		SEQ_NXT = SEQ_NXT + MSS;//500; //+ packet length
		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[ExT]: Received data from upper layer (packet #%u).  set p->auxiliar_SEQ=%u; Next SEQ=%u \n",t, this->getFullName().data(), p->getId(), p->auxiliar_SEQ, SEQ_NXT);

		// NEW packet enters the buffer
		SendBuffer.push_back(p);   			// New packets are stored at the tail of the double ended queue
		BUF_Fully_SNT = false; 				// The Buffer gets a new packet, so it is not fully sent for sure
		BUF_SIZE = (int)SendBuffer.size();
		if (BUF_SIZE==1) {
			WND_NXT_SND=1;
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: BUF_SIZE==1 (WND_NXT_SND set to %u) \n",t, this->getFullName().data(),WND_NXT_SND);
		}
	} else {       // an ACK from RCV module
		_IS_DATA_PACKET = false;
		_IS_ACK_PACKET = true;

		//		double *xv;
		//		xv=(double*)(x.value);
		//KIKE-MEJORA: recibo packetes por el canal de retorno
		//		pack = (Packet*)x.value; // Matias: how can it be that x.value is both double and Packet*?
		pack = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event

		// validate the packet is for me
		if(!validateAck(pack)){
			debugMsg(LOG_LEVEL_ERROR, "[%g] WARNING - %s_dext: ACK packet #%u was not intended for this TCP session (different SRC/DST or PORTS). Ignoring. \n ", t, this->getFullName().data(), pack->getId());
			this->sigma = this->sigma - e; // continue as before
			return;
		}

		auto tcpProtocol = std::static_pointer_cast<TcpIpProtocol>(pack->getProtocol(PROTOCOL_ID_TCP_IP));

		// Update the proper ACK sequences of interest (local, and remote)
		ReceivedACKedFromPeerLayer=tcpProtocol->getACK(); // This is the most recent ACK number that "the other end is confirming as valid"
		OriginalACKedSEQ_TimeStamp=pack->lastModifTime; // TimeStamp of the DATA Packet that Originated this ACK
		SendACKtoPeerLayer=0; // This is the most actual ACK number "that can be confirmed to the other end"
		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: received ACK packet (expecting ACK=%d). pack->ACK=%d ; pack->getId()=#%d ;  ReceivedACKedFromPeerLayer=%d. TCP: WND_CWND(%g), BUF_SIZE(%d), WND_SSTHRESH(%d)  \n",t, this->getFullName().data(),
				SEQ_NEXT_Expected_ACK, tcpProtocol->getACK(), pack->getId(), ReceivedACKedFromPeerLayer, OriginalACKedSEQ_TimeStamp, SendACKtoPeerLayer, WND_CWND, BUF_SIZE, WND_SSTHRESH);

		this->logger->logSignal(t,ReceivedACKedFromPeerLayer,"receiveAck");
		//KIKE-MEJORA: logueo al PCAP el ACK
		//myPcap->writeFrame(t, "10.0.0.2", "10.0.0.1", 10004, 1001, ReceivedACKedFromPeerLayer, ReceivedACKedFromPeerLayer,0);
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: recibi ACK from packet SEQ(%u) y lo grabo como receiveAck\n",t,ReceivedACKedFromPeerLayer);
		// FOR TESTING: (Always the ACK is the expected one)
		//		            ReceivedACKedFromPeerLayer = SEQ_NEXT_Expected_ACK;
		//
		//		            // Also Eventually forces a DUP ACK
		//		            if (mySTDEVS->bernoulli(0)==1) {
		//		                ReceivedACKedFromPeerLayer = SEQ_NEXT_Expected_ACK-1;
		//					};

		if (ReceivedACKedFromPeerLayer==SEQ_NEXT_Expected_ACK)	{ // This is a VALID ACK, i.e., the expected sequence number
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: This is a VALID ACK... \n",t, this->getFullName().data());
			_IS_VALID_ACK_PACKET = true;
			_IS_DUP_ACK_LIMIT_PACKET = false;

			// OLD packet leaves the buffer
			SendBuffer.pop_front();	// DROP the just ACKNOWLEDGED PACKET FROM THE SEND BUFFER
			//

			T_MEASURED_RTT = t - OriginalACKedSEQ_TimeStamp ;
			this->logger->logSignal(t,T_MEASURED_RTT,"measuredRTT");

			SEQ_LAST_Valid_ACK = ReceivedACKedFromPeerLayer ;
			////KIKE-Prueba:
			//SEQ_NEXT_Expected_ACK++ ;
			SEQ_NEXT_Expected_ACK = SEQ_NEXT_Expected_ACK + MSS;//500; //+ packet length
			if(SndPktAcks_Record_log)
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: NEXT Expected ACK is %u \n",t, this->getFullName().data(),SEQ_NEXT_Expected_ACK);
			//KIKE-MEJORA: comento la siguiente linea para agregarla mas abajo con "Fast Recovery"
			//WND_DUP_ACK=0 ;		// Clears the Duplicate ACK counter
			WND_NXT_SND-- ;		// Updates the pointer to the next packet to be sent, because it is relative to the back of the queue
			if(SndPktAcks_Record_log)
				debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: ACK valido. Updates the pointer to the next packet to be sent(decremento WND_NXT_SND:%u) \n",t,WND_NXT_SND);
			if (WND_NXT_SND<1) {WND_NXT_SND=1;}; // Saturates at 1 at low values
			BUF_SIZE = (int)SendBuffer.size();

			//KIKE-MEJORA: agrego "Fast Recovery"
			if(WND_DUP_ACK >= DUP_ACK_LIMIT )
			{
				//Perform "Fast Recovery":
				WND_CWND = WND_SSTHRESH; // exit FR
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: (PRUEBO GUARDAR CADA VEZ que CAMBIA) LoggerToScilab cwnd=%g\n",t, this->getFullName().data(),WND_CWND);
				this->logger->logSignal(t,(double)WND_CWND,"cwnd");

				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Valid Ack, Perform \"Fast Recovery\": WND_CWND = WND_SSTHRESH(%u) and go to CA\n",t, this->getFullName().data(),WND_SSTHRESH);
				//PRUEBA pues es nuevo ACK valido, entonces retomo el env?o normal
				SEQ_CYCLE_ACK = 0;
			} else {
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: ACK arrived out of order WND_DUP_ACK(%d)<DUP_ACK_LIMIT(%d)\n",t, this->getFullName().data(), WND_DUP_ACK,DUP_ACK_LIMIT);

				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: a.1.2 verifico si es un CYCLE ACK: ReceivedACKedFromPeerLayer(%u)<=?SEQ_CYCLE_ACK(%u) \n",t, this->getFullName().data(),ReceivedACKedFromPeerLayer,SEQ_CYCLE_ACK);
				if (ReceivedACKedFromPeerLayer<=SEQ_CYCLE_ACK) { // THIS IS A CYCLE ACK //KIKE-MEJORA: cambia condicion "==" to "<=" llega pkt antes tx.
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: This is a CYCLE ACK. \n",t, this->getFullName().data());

					//					if (ReceivedACKedFromPeerLayer==SEQ_CYCLE_ACK) { // Matias fix: why is RTT calculated only on SEQ_CYCLE_ACK? SEQ_CYCLE_ACK is reset for every valid ACK, so RTT is effectively calculated only for the first packets.
					T_RTT = RTT_alfa * T_RTT + (1-RTT_alfa) * T_MEASURED_RTT ;
					this->logger->logSignal(t,(double)T_RTT,"calcRTT");

					T_RTT_DEVIATION = RTT_alfa * T_RTT_DEVIATION + (1-RTT_alfa) * fabs(T_RTT - T_MEASURED_RTT);

					if (T_FORCED_RTO>=0) {
						T_RTO = T_FORCED_RTO ;                  // FORCED
					} else {
						T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
					}
					T_RTO = std::max(MIN_RTO, T_RTO); // forces min RTO

					debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: New calculation of the RTO=%g. T_RTT=%g ; T_MEASURED_RTT=%g; T_RTT_DEVIATION=%g \n",t, this->getFullName().data(), T_RTO, T_RTT, T_MEASURED_RTT, T_RTT_DEVIATION);
					this->logger->logSignal(t,T_RTO,"RTO");
					this->logger->logSignal(t,T_RTT_DEVIATION,"RTO_dev");
					//					} // Matias fix

					//KIKE//if (WND_CWND<WND_SSTHRESH && WND_firsttime_SSTHRESH==true) {
					if (WND_CWND<WND_SSTHRESH) { // Slow Start
						debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: we are in Slow Start (WND_CWND=%g >= WND_SSTHRESH=%d) \n", t, this->getFullName().data(), WND_CWND, WND_SSTHRESH);
						//KIKE: Each time an ACK is received, the congestion window is increased by 1 segment (= MSS bytes). cwnd = cwnd + segsize with segsize = MSS
						//WND_CWND=WND_CWND*2;    // Simil Slow Start, but only the first time //KIKE: Congestion Window (in packets) NO en bytes...
						WND_CWND = WND_CWND + 1;//KIKE//Prueba

						if(CtrolCongestion_Record_log)
							debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: \"Slow Start\"(SS) WND_CWND(%g)\n",t, this->getFullName().data(),WND_CWND);
						//KIKE-MEJORA: comento la condicion de Max cwnd.
						/*
							if (WND_CWND>WND_SSTHRESH) {
										debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: Maximum Window WND_CWND>WND_SSTHRESH=%u\n",t,WND_SSTHRESH);
										WND_CWND=WND_SSTHRESH;  // Maximum Window
									};
						 */
					} else { // Congestion Avoidance
						debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: we are in Congestion Avoidance (WND_CWND=%g >= WND_SSTHRESH=%d) \n", t, this->getFullName().data(), WND_CWND, WND_SSTHRESH);
						//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: Simil Congestion Avoidance...\n",t);
						//WND_CWND++;
						WND_CWND = WND_CWND + (1/WND_CWND);//KIKE//Prueba
						//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: WND_CWND++=%u\n",t,WND_CWND);
						//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: WND_CWND(%g) + (1/WND_CWND)(%g)\n",t,WND_CWND,(1/WND_CWND));
						// EXPERIMENTAL: User Load Factor (N) Modifier
						if (t>NchangeFactorInit && t<NchangeFactorEnd) {
							WND_CWND = WND_CWND - abs(WND_CWND * (1-NchangeFactor));
						}
						// EXPERIMENTAL
						WND_firsttime_SSTHRESH=false;
					}

					//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: verifico si CWND=%u supera la Maxima=WND_CWND_MAX(%u)\n",t,WND_CWND,WND_CWND_MAX );
					//KIKE-MEJORA: saco el limite (en realidad deberia ser el max wind?...
					/*
					if (WND_CWND>WND_CWND_MAX) {
							WND_CWND=WND_CWND_MAX;  // Maximum Window
						};
					 */

					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Updated window: WND_CWND=%g ; WND_SSTHRESH=%d   \n",t, this->getFullName().data(),WND_CWND, WND_SSTHRESH);
					this->logger->logSignal(t,(double)WND_CWND,"cwnd");

					// REVIEW: setting SEQ_CYCLE_ACK=0 probably requires some condition, now it is done for every valid ACK so it is not tracking cycles and it is causing RTT not to be properly calculated
					SEQ_CYCLE_ACK = 0;  // A new measurement cycle starts all over again
					// This Signals the SENDING state that
					// the next SEQ number will represent a CYCLE_ACK
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: Estoy en SEQ_CYCLE_ACK set to: %u (open to next) \n",t,SEQ_CYCLE_ACK);
				};
			}
			WND_DUP_ACK=0 ;
		} else {					// This is an INVALID ACK, an unexpected acked sequence number
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: ACK %d is INVALID. last valid: %d ; expected: %d  \n", t, this->getFullName().data(), ReceivedACKedFromPeerLayer, SEQ_LAST_Valid_ACK, SEQ_NEXT_Expected_ACK);

			_IS_VALID_ACK_PACKET = false;
			_IS_DUP_ACK_LIMIT_PACKET = false;

			if (ReceivedACKedFromPeerLayer==SEQ_LAST_Valid_ACK) {  // It is a DUPLICATED ACK
				WND_DUP_ACK++;
				this->logger->logSignal(t,(double)WND_DUP_ACK,"dupACK");
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: This is DupACK number %d  \n", t, this->getFullName().data(), WND_DUP_ACK);

				if (WND_DUP_ACK==DUP_ACK_LIMIT) {		// and Reached LIMIT duplicate ACKs
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Reached the DupACK Limit (DUP_ACK_LIMIT=%d), so start Fast Recovery. Current WND_CWND=%g ; WND_SSTHRESH=%g \n", t, this->getFullName().data(), DUP_ACK_LIMIT, WND_CWND, WND_SSTHRESH);

					_IS_DUP_ACK_LIMIT_PACKET = true;
					this->logger->logSignal(t,(double)WND_DUP_ACK,"tripleDupACK");

					//SEQ_NextAcceptedECNMark = SendBuffer.at(WND_NXT_SND-1)->auxiliar_SEQ; // ignore ECN marks for already sent packets. It is not exactly the same as in the RFC (because in-flight != CWND). See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"
					SEQ_NextAcceptedECNMark = ReceivedACKedFromPeerLayer + (WND_CWND * MSS);; // ignore ECN marks for the next window of data. See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"

					//WND_DUP_ACK=0 ;//KIKE-MEJORA: comento para poder manejar los siguientes dupacks...
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: WND_CWND / 2\n",t);
					//KIKE HOY el tema es esto seria FR pero debe hacerlo al comienzo y luego cwnd=cwnd+1 x c/DupACK....
					WND_SSTHRESH = WND_CWND / 2;//KIKE-MEJORA: Efecto: en CA la WND_CWND aumenta menos?: xq en SS compara con Ssthresh y define el Max cwnd....
					WND_SSTHRESH = std::max(1, WND_SSTHRESH); // Matias fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)
					WND_CWND = abs(WND_CWND / 2) + (3);//KIKE asi estaba hasta ahora://WND_CWND = abs(WND_CWND / 2);

					WND_firsttime_SSTHRESH=false;
					if (WND_CWND<1)
					{
						WND_CWND=1;	//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: WND_CWND=1\n",t);
					}
					WND_CWND_precise = WND_CWND ;
					this->logger->logSignal(t,(double)WND_CWND,"cwnd");


					//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: LoggerToScilab cwnd=%u\n",t,WND_CWND);
					//this->logger->logSignal(t,(double)WND_CWND,"cwnd");//KIKE-MEJORA: lo ponga abjo comun para todo el IF
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: **ACK_INVALIDO(a)** LoggerToScilab cwnd=%u\n",t,WND_CWND);
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Is DUP_ACK_LIMIT(%u) \"Fast Retransmit\"(FR) WND_SSTHRESH(%u) WND_CWND(%g)\n",t, this->getFullName().data(), DUP_ACK_LIMIT,WND_SSTHRESH,WND_CWND);

					WND_NXT_SND = 1 ;	   		// Update the pointer
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: DupACK Limit, Updates the pointer (set WND_NXT_SND:%u) \n",t,WND_NXT_SND);
					BUF_Fully_SNT = false ; 	//
					WND_Fully_SNT = false ; 	//

					//KIKE-MEJORA: comento la siguiente linea (SEQ_LAST_Valid_ACK = -1) para Detectar DupACKs>DUP_ACK_LIMIT
					//SEQ_LAST_Valid_ACK = -1; 	// prevents identifying the next dup acks that might be on-fly as a repetition of this condition (dup ack is detected only once inside the same cycle)

					// This Signals the SENDING state that the next SEQ number will represent a CYCLE_ACK
					SEQ_CYCLE_ACK = 0;  // A new measurement cycle starts all over again


					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: DupACK Limit (DUP_ACK_LIMIT=%u) updated window: WND_CWND=%g ; WND_SSTHRESH=%g \n", t, this->getFullName().data(), DUP_ACK_LIMIT, WND_CWND, WND_SSTHRESH);
				} else if(WND_DUP_ACK > DUP_ACK_LIMIT && ReceivedACKedFromPeerLayer > 0) { //KIKE-MEJORA:Por c/DupACK repetido doblo CWND "Fast Recovery"
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: We are already in Fast Recovery. Current WND_CWND=%g ; WND_SSTHRESH=%g \n", t, this->getFullName().data(), WND_CWND, WND_SSTHRESH);

					//WND_CWND=WND_CWND*2;
					WND_CWND = WND_CWND + 1;//equivalente a (SS)
					if (WND_CWND>WND_CWND_MAX) {
						WND_CWND=WND_CWND_MAX;  // Maximum Window
						if(CtrolCongestion_Record_log)
							debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: WND_CWND=WND_CWND_MAX=%g\n",t, this->getFullName().data(),WND_CWND);
					};

					this->logger->logSignal(t,(double)WND_CWND,"cwnd");
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: (b\")This is Invalid ACKs (ACK recv=%u), debo incrementar WND_CWND=MASMSS(%u) \"**Fast Recovery**\" \n",t,ReceivedACKedFromPeerLayer,WND_CWND);
					//this->logger->logSignal(t,(double)WND_CWND,"cwnd");//REVISAR!!!El valor de CWND se va al carajo, ver cual es el limite (ej: flight_size = std::min(state->snd_cwnd, state->snd_wnd);)
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: **ACK_INVALIDO(b)** LoggerToScilab cwnd=%u\n",t,WND_CWND);

					if(CtrolCongestion_Record_log)
						debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Is DupACK repetido \"Fast Recovery\"(FR) CWND(%g)\n",t, this->getFullName().data(),WND_CWND);

					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: DupACK during Fast Recovery updated window: WND_CWND=%g ; WND_SSTHRESH=%g \n", t,  this->getFullName().data(), WND_CWND, WND_SSTHRESH);
					//Falta que Retransmita el paquete!!!!!!!:
					_IS_DUP_ACK_LIMIT_PACKET = true;
				}
			} else {

				// Just ignores the Invalid ACK (Es con ACK=0 no esta explicado porque llegan...
				if(CtrolCongestion_Record_log)
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: (b\")This is Invalid ACKs (ACK recv=%u)(DupACK=%u), Just ignores, do Nothing... \n",t, this->getFullName().data(),ReceivedACKedFromPeerLayer,WND_DUP_ACK);
			}

			//KIKE-MEJORA: El valor de CWND se va al carajo, ver cual es el limite (ej: flight_size = std::min(state->snd_cwnd, state->snd_wnd); o hasta
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: **ACK_INVALIDO(T)** LoggerToScilab cwnd=%u\n",t,WND_CWND);
			this->logger->logSignal(t,(double)WND_CWND,"cwnd");
		}

		// Handle ECN mark ("quick and dirty" implementation, several things from the RFC missing)
		if(tcpProtocol->getMarkCongestion() && // packet has the congestion notification mark
				ReceivedACKedFromPeerLayer >= SEQ_NextAcceptedECNMark &&  // See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"
				!_IS_DUP_ACK_LIMIT_PACKET ){
			// should be similar to the single packet-loss (dup-ACK).
			// See RFC3168, 6.1.2. The TCP Sender:  https://tools.ietf.org/html/rfc3168#section-6.1.2
			// "TCP source halves the congestion window "cwnd" and reduces the slow start threshold "ssthresh"
			this->logger->logSignal(t, 1,"ECNMark");

			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[ext] Packet #%d has ECN mark. (sigma=%g, WND_CWND(%g), WND_SSTHRESH(%d), WND_DUP_ACK(%d))\n",t, this->getFullName().data(), pack->getId(), sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK);
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: BUF_SIZE=%u ; WND_CWND=%u  ; WND_NXT_SND=%u \n", t, this->getFullName().data(), BUF_SIZE, WND_CWND, WND_NXT_SND);


			SEQ_NextAcceptedECNMark = ReceivedACKedFromPeerLayer + (WND_CWND * MSS); // Ignore ECN marks for the next window of data. See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"

			WND_SSTHRESH = WND_CWND / 2;
			WND_SSTHRESH = std::max(1, WND_SSTHRESH); // Matias fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)
			WND_CWND = std::max(1.0, std::abs(WND_CWND / 2)) ;//KIKE asi estaba hasta ahora://WND_CWND = abs(WND_CWND / 2);

			WND_CWND_precise = WND_CWND ;
			WND_DUP_ACK=0 ; // reset dup-ack counter. "TCPP source should not decrease the slow-start threshold, ssthresh, if it has been decreased within the last round trip time.  "

			// things different from the dup-ACK case
			//WND_NXT_SND = 1 ;	 // We do not retransmit, keep pointing to the same next packet as before
			//BUF_Fully_SNT = false ;
			//WND_Fully_SNT = false ;
			//			SEQ_CYCLE_ACK = 0;  // A new measurement cycle starts all over again

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext] ECN mark reduced window :  (sigma=%g, WND_CWND(%g), WND_SSTHRESH(%d), WND_DUP_ACK(%d)) \n",t, this->getFullName().data(), sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK);
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: BUF_SIZE=%u ; WND_CWND=%u  ; WND_NXT_SND=%u \n", t, this->getFullName().data(), BUF_SIZE, WND_CWND, WND_NXT_SND);

			//			SEQ_NextAcceptedECNMark = SendBuffer.at(WND_NXT_SND-1)->auxiliar_SEQ; // ignore ECN marks for already sent packets. It is not exactly the same as in the RFC (because in-flight != CWND). See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"


			this->logger->logSignal(t,(double) WND_CWND,"cwnd");
		}
	}


	uint first =0; uint last=0;  // Matias: These variables are set but not used except for debug. The following 'if' is useless
	//int snd_una; int snd_nxt; double maxWindow; double effectiveWin;//KIKE//Prueba
	if (BUF_SIZE>0) {
		first=(int)(SendBuffer.at(0)->getId()) ;//KIKE//Prueba: lo veo equivalente a "snd_una"
		last=(int)(SendBuffer.at(BUF_SIZE-1)->getId()) ;//KIKE//Prueba: es el ultimo paquete encolado y necesito el ultimo enviado para "snd_nxt"
		// //KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: BUFFER \n",t);
		// for (int j=0;j<=(BUF_SIZE-1);j++) {
		// 	int id = (int)(SendBuffer.at(j)->getId()) ;
		// 	//KIKE//debugMsg(LOG_LEVEL_DEBUG, "%i \n",id);
		// };
		/*maxWindow = std::min(131.0, WND_CWND);//rwnd,cwnd
				snd_una = first;
				if(pout==NULL){
					snd_nxt = last;
					debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]:a)try set snd_nxt=last\n",t);
				}
				else{
					snd_nxt = pout->getId();//last;//SEQ_NXT;//(pout->getId()) + 1;
					debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]:a)try set snd_nxt=pout->getId()\n",t);
				}

				effectiveWin = maxWindow - (snd_nxt - snd_una);
		 */
	}
	//char *sEf = (char *)((effectiveWin < 0) ? "NEGATIVO" : "Positivo");

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[ExT]: Finished TCP calculations. Now will define new state. Current state: mystate=%i ; BUF_SIZE %u (firstID:#%i-lastID:#%i); WND_CWND=%g; WND_NXT_SND=%i ;  \n",
			t, this->getFullName().data(), mystate, BUF_SIZE, first,last, WND_CWND, WND_NXT_SND);

	// According to the State do:
	switch (mystate)
	{
	case WAITxAPP: //  Waiting for data from the upper layer
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: ext@ WAITxAPP [BUF_SIZE(%u)<WND_CWND(%g) && p->lastQueueSizeAfterLeaving(%g)>0] \n",t, this->getFullName().data(),BUF_SIZE,WND_CWND,p->lastQueueSizeAfterLeaving);

		if(_IS_DATA_PACKET) {
			if (BUF_SIZE<WND_CWND) {   // The Congestion Window allows the Buffer to be filled with more packets
				// (Matias fix) before:  && p->lastQueueSizeAfterLeaving>0 ) // AND there are more to grab from the Upper Layer
				// (Matias)  there is no notification when the Upper Layer has new packets --> model gets stuck until ACK is received. But if lastQueueSizeAfterLeaving is not taken into account the model will stop tracking the rest (ej:RTO) until next packet from upper layer arrives. (I dont see a fix with few lines)
				if(p->lastQueueSizeAfterLeaving<=0 )
					debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[Ext] @WAITxAPP: WARNING: Entering Matias quick workaround. Will request more data from upper layer even if the upper layer queue is empty. Might loose track of sigma(%f)=RTO(%f) if upper layer does not respond 'fast'  \n", t, this->getFullName().data(), this->sigma, T_RTO);
				// State Transition
				mystate=tr_REQxAPP;		//
				sigma=0;
				break;
			} else { 					// The upper layer buffer is empty
				// OR The Congestion Window is Full
				// State Transition
				mystate=SENDING;		//
				sigma=INTERPACKET_SND_TIME;
				if(BUF_SIZE<WND_CWND) debugMsg(LOG_LEVEL_ERROR, "[%g] %s[Ext] @WAITxAPP: WARNING: Possible wrong behavior of TCP model. Model should request data from upper layer, but upper layer queue is empty, so the model will continue sending. The model will never get notified when new data arrived to upper layer. This might cause low down of transmition to one packet at a time (even with a big window)  \n", t, this->getFullName().data());
				//KIKE//Prueba: intento que si tengo data enviarla inmediatamente //KIKE-MEJORA
				//sigma=0;//(==INTERPACKET_SND_TIME=0)(K1)
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Ext]: to SENDING during %g (**modificado**) \n",t,sigma);
				break;
				//
			};
		};
		break;
	case tr_REQxAPP: // Asks for more data from the upper layer
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: @ tr_REQxAPP \n", t, this->getFullName().data());
		// State Transition
		sigma=sigma-e;
		break;

	case SENDING: // Flushing the sender window
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: @ SENDING \n",t, this->getFullName().data());
		// State Transition
		sigma=sigma-e;
		break;

	case WAITxACK:  //  Waiting for ACKs from the pair RCV node
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: @ WAITxACK \n", t, this->getFullName().data());
		if(_IS_ACK_PACKET) {
			if (_IS_VALID_ACK_PACKET) {
				// State Transition
				mystate=tr_REQxAPP; 	//  Forces an ACK timeout, to consider retransmissions (but here a triple dup ACK happened)
				sigma=0;
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND:[WAITxACK:] to tr_REQxAPP during %g \n",t,sigma);
				break;
			} else {
				if (_IS_DUP_ACK_LIMIT_PACKET) {
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: DUP_ACK_LIMIT -> Try to retransmit buffer \n", t, this->getFullName().data());
					// State Transition
					mystate=SENDING; 	//  Retry sending again with the new WND_CWND and WND_NXT_SND
					sigma=INTERPACKET_SND_TIME;
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND:[WAITxACK:] to SENDING during %g \n",t,sigma);
					//KIKE//Prueba: intento que si tengo data enviarla inmediatamente //KIKE-MEJORA
					//sigma=0;//(==INTERPACKET_SND_TIME=0)(K1)
					break;
				} else {
					// State Transition
					mystate=WAITxACK; 	//  Remains Waiting for a valid ACK
					sigma=sigma-e;		//  The TIME OUT timer keeps running
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND:[WAITxACK:] to WAITxACK during %g \n",t,sigma);
					break;
				}
			}
		}

		if(_IS_DATA_PACKET) { // its not an ACK, its data coming from upper layer
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[ExT]: WAITxACK + _IS_DATA_PACKET. sigma=%g ; e=%g ;  RTO=%g \n", t, this->getFullName().data(), sigma, e, T_RTO);
			// State Transition
			mystate=WAITxACK;  	//  Remains where it was
			this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
//			sigma=T_RTO ;  		//  Waits again a full TIMEOUT period
		}

		break ;
	default:
		debugMsg(LOG_LEVEL_ERROR, "[%g] tcpSND[ext]: Error: Do not know what to do... \n",t);
		break;
	}

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[ExT]: New state: mystate=%i ; sigma=%g;  \n", t, this->getFullName().data(), mystate, sigma);

	return ;
}

Event packettcpsnd::lambda(double t) {
	////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: [OUT] \n",t);

	// For ANY current State do:

	double out_cwnd[10]; for (int i=0;i<10;i++) {out_cwnd[i]=0;}
	out_cwnd[0]=(double)WND_CWND;

	// Matias: this is dead code (MAX_ALLOWED_TO_SND defined and never used)
	if (BUF_SIZE<WND_CWND) {
		int MAX_ALLOWED_TO_SND=BUF_SIZE;
	} else {
		int MAX_ALLOWED_TO_SND=WND_CWND;
	};

	// According to the current State do:

	switch (mystate)
	{
	case tr_REQxAPP: // Asks for more data from the upper layer
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: Asks for more data from the upper layer. tr_REQxAPP (out_cwnd=%g) \n",t, this->getFullName().data(), out_cwnd[0]);
		// We only need to send ANY event value to the queue, but we send the cwnd value to be able to route this signal to a plot
		return Event(&out_cwnd,1);
		//return Event((void *)1,1);
		break;

	case WAITxAPP:
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: out@ WAITxAPP [outs nothing] \n",t);
		// Do not send any Output
		return Event();
		break;

	case SENDING: // Flushing the sender window
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: [SENDING] BUF_SIZE(%u)> 0 && BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g) [SendBuffer.size(%u)]\n",t, this->getFullName().data(),BUF_SIZE, BUF_SIZE,WND_NXT_SND,WND_CWND,(int)SendBuffer.size());

		if (BUF_SIZE>0 && WND_NXT_SND<=BUF_SIZE && WND_NXT_SND<=WND_CWND)  { // Send Buffer !empty AND !completely sent KIKE//Prueba: && effectiveWin>0
			//if(SndPktAcks_Record_log)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Out]: BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g)\n", t, BUF_SIZE, WND_NXT_SND, WND_CWND);
			auto packetToSend = SendBuffer.at(WND_NXT_SND-1);  // -1 is because first element is pointed as 0
			this->pout = std::shared_ptr<NetworkPacket>(packetToSend->clone()); // send a copy of the packet as it will change along the way

			//// FOR TESTING: Drops a packet randomly ->  Emulates a "SOURCE FAILURE"
			//		            if (mySTDEVS->uniform()<=0.001) {
			//						//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: SOURCE FAILURE for Packet ID %u (at position %u) [sends nothing]\n",t, pout->getId(),WND_NXT_SND);
			//		 				this->logger->logSignal(t,1,"packetdrop");
			//		                return Event();
			//					} else {
			//						pout->lastModifTime=t; // Sets the sending timestamp
			//						//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: Sending Packet ID %u (at position %u) with timestamp %g \n",t, pout->getId(),WND_NXT_SND,pout->lastModifTime);
			//						return Event(pout,0);
			//				};
			//// NORMAL:
			this->pout->lastModifTime=t; // Sets the sending timestamp
			////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: Sending Packet ID %u (at position %u) with timestamp %g \n",t, pout->getId(),WND_NXT_SND,pout->lastModifTime);
			this->logger->logSignal(t,this->pout->getId(),"packetsend");
			//KIKE-MEJORA: logueo al PCAP el Pkt
			//myPcap->writeFrame(t, "10.0.0.1", "10.0.0.2", 1001, 10004, pout->auxiliar_SEQ, pout->getId(), (int)(pout->getLength_bits()/8));

			debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[lambda]: Sending Packet #%u size=%i bytes (SEQ=%u). Because BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g)\n",t, this->getFullName().data(), pout->getId(), pout->getLength_bits()/8, pout->auxiliar_SEQ, BUF_SIZE, WND_NXT_SND, WND_CWND);
			this->pout->printInfo(this->debugLevel >= LOG_LEVEL_IMPORTANT? LOG_LEVEL_ALWAYS:LOG_LEVEL_DEBUG );
			return Event(this->pout,0);
		} else {				// Send Buffer empty
			debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[Out]: NoTx (SENDING) Buffer Empty or Fully Sent ! [outs nothing] \n",t, this->getFullName().data());
			return Event();
			// Do not send any Output
		};
		break;

	case WAITxACK:
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: out@ WAITxACK [outs nothing] \n",t);
		// Do not send any Output
		return Event();
		break;

	default:
		debugMsg(LOG_LEVEL_ERROR, "[%g] tcpSND: Error: Do not know what to do... \n",t);
		throw std::runtime_error("packettcpsnd::lambda - tcpSND: Error: Do not know what to do... \n");
		return Event();
		break;
	}
}

bool packettcpsnd::validateAck(std::shared_ptr<NetworkPacket> packet){
	// Get the IP (from the TCP protocol)
	auto protocol = packet->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "%s_validatePacket: received packet #%u without tcp protocol. discarding. \n ", this->getFullName().data(), packet->getId());
		packet->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("packettcpsnd::validatePacket: received packet without tcp protocol.");
	}
	// cast (although not safetely)
	std::shared_ptr<TcpIpProtocol> ipProtocol = std::static_pointer_cast<TcpIpProtocol>(protocol);

	// validate
	if(ipProtocol->getIpSrc() != this->ipReceiver ||
			ipProtocol->getIpDst() != this->ipSender ||
			ipProtocol->getPortSrc() != this->portReceiver ||
			ipProtocol->getPortDst() != this->portSender){
		return false;
	}

	return true;
}

