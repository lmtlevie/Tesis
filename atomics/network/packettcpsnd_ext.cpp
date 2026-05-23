#include "packettcpsnd_ext.h"
////IMPORTANTE: NO MODIFICAR POR EL EDITOR DE PD PUES RECORTA CODIGO (EN EL APARTADO DE &ext)!
void packettcpsnd_ext::init(double t,...) {
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
	MSS = readDefaultParameterValue<uint>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param MSS: %u \n", t, MSS);

	fvar = va_arg(parameters,char*);
	this->ipSender = fvar;
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: ipSender: %s  \n",t, this->getFullName().data(), this->ipSender.data());

	fvar = va_arg(parameters,char*);
	this->ipReceiver = fvar;
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: ipReceiver: %s  \n",t, this->getFullName().data(), this->ipReceiver.data());

	fvar = va_arg(parameters,char*);
	this->portSender = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: portSender: %u  \n",t, this->getFullName().data(), this->portSender);

	fvar = va_arg(parameters,char*);
	this->portReceiver = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: portReceiver: %u  \n",t, this->getFullName().data(), this->portReceiver);

	fvar = va_arg(parameters,char*);
	// MAX Congestion Window (in packets) inf==NONE, Typical: 64
	WND_CWND_MAX = readDefaultParameterValue<uint>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param WND_CWND_MAX: %i \n",t,WND_CWND_MAX);

	fvar = va_arg(parameters,char*);
	// Experimental Slow Start, 0==OFF
	WND_SSTHRESH = readDefaultParameterValue<uint>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param WND_SSTHRESH: %i \n",t,WND_SSTHRESH);

	fvar = va_arg(parameters,char*);
	// Typical value 0.125. RTT_alfa==0 => No moving average for RTT, only instantaneous measurements
	RTT_alfa = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param RTT_alfa: %g \n",t,RTT_alfa);

	fvar = va_arg(parameters,char*);
	// Initial Calculated Round Trip Time
	T_RTT = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param Initial T_RTT: %g \n",t,T_RTT);

	fvar = va_arg(parameters,char*);
	// Typical value 3
	DUP_ACK_LIMIT = readDefaultParameterValue<uint>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param DUP_ACK_LIMIT: %u \n",t,DUP_ACK_LIMIT);

	fvar = va_arg(parameters,char*);
	// T_FORCED_RTO>=0 => Forces RTO instead of calculating it
	T_FORCED_RTO = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param T_FORCED_RTO: %g \n",t,T_FORCED_RTO);

	fvar = va_arg(parameters,char*);
	// Arbitrary Practical Criteria. If 1 mseg. (i.e. Limits (P)acket (T)hroughput to T=1 KPps, which at 1KByte per Segment renders T=1MBps of user data. or T=8Mbps of bandwidth
	// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
	INTERPACKET_SND_TIME = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param INTERPACKET_SND_TIME: %g \n",t,INTERPACKET_SND_TIME);

	fvar = va_arg(parameters,char*);
	// Arbitrary Practical Criteria. 0.1 mseg. Time to wait
	// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
	INTER_REQ_TIME = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param INTER_REQ_TIME: %g \n",t,INTER_REQ_TIME);

	fvar = va_arg(parameters,char*);
	MIN_RTO = readDefaultParameterValue<double>(fvar); // usually in linux is 200ms
	debugMsg(LOG_LEVEL_INIT, "[%g] tcpSND: Param MIN_RTO: %g \n",t,MIN_RTO);

	// EXPERIMENTAL:
	NchangeFactor=1;  // 1=No Effect (i.e. this is disabled)
	NchangeFactorInit=-1;//NchangeFactorInit=10; // Matias: dissable this thing
	NchangeFactorEnd=-1;//NchangeFactorEnd=20;
	//

	this->logger->initSignals(std::map<std::string, int>{
		{"cwnd", LOG_LEVEL_PRIORITY},
		{"measuredRTT", LOG_LEVEL_PRIORITY},
		{"RTO", LOG_LEVEL_PRIORITY},
		{"RTO_dev", LOG_LEVEL_PRIORITY},
		{"packetdrop", LOG_LEVEL_DEBUG},
		{"calcRTT", LOG_LEVEL_DEBUG},
		{"receiveAck", LOG_LEVEL_DEBUG},
		{"packetsend", LOG_LEVEL_DEBUG},
		{"timeout", LOG_LEVEL_DEBUG},
		{"dupACK", LOG_LEVEL_DEBUG},
		{"tripleDupACK", LOG_LEVEL_DEBUG},
		{"ECNMark", LOG_LEVEL_DEBUG}
	});

	//Para logueo en PCAP
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[init] LoggerToPcap: PCAPTCPSND.pcap\n",t);
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
	// -> This will be the expected ACK capable of growing W_CWND 1 unit
	SEQ_NXT = 1;			// Next Sequence Number to use for sending

	ReceivedACKedFromPeerLayer = 0;			// This is the most recent ACK number that
	// "the other end is confirming as valid"
	SendACKtoPeerLayer = 0;			// This is the most actual ACK number
	// "that can be confirmed to the other end"
	T_MEASURED_RTT =  0;		// [[PENDING: init to something standard
	T_RTT_DEVIATION = 0; 		// [[PENDING: init to something standard

	if (T_FORCED_RTO>=0) {
		T_RTO = T_FORCED_RTO ;                  // FORCED
	} else {
		T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
	}
	T_RTO = std::max(MIN_RTO, T_RTO);

	this->logger->logSignal(t,T_RTO,"RTO");
	debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[init] T_RTT(%g), T_RTO(%g)\n",t,T_RTT,T_RTO);
	////////////////////////////////////////////////////////////////


	// State Transition
	mystate = tr_REQxAPP;	// Inmediately asks for the first new packet
	sigma = 0;  			//

	return;
}

void packettcpsnd_ext::dint(double t) {
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: [INT] \n",t);

	// According to the State do:
	switch (mystate)
	{
	case tr_REQxAPP: // Asks for more data from the upper layer
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: int@ tr_REQxAPP \n",t);
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@tr_REQxAPP\n",t);
		// State Transition
		mystate=WAITxAPP;
		sigma=INTER_REQ_TIME;
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: to WAITxAPP during %g \n",t,sigma);
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@tr_REQxAPP to WAITxAPP\n",t);
		break;

	case WAITxAPP: // Waits for data from the upper layer
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: int@ WAITxAPP \n",t);
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@WAITxAPP\n",t);
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
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[InT]@WAITxACK: TCP TIMEOUT -> Try to retransmit buffer (sigma=%g, WND_CWND(%g), WND_SSTHRESH(%u), WND_DUP_ACK(%u))\n",t, this->getFullName().data(),sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK);

		WND_DUP_ACK=0 ; // [[ to Reconsider
		// CLASSICAL OPTION:
		// WND_CWND = 1;
		// MODIFIED OPTION: (Treats TO as a DUP ACK) //KIKE-MEJORA: cuando hay timeout debe ir a SS comenzando con cwnd = 1 MSS
		//WND_CWND = abs(WND_CWND / 2);

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[InT] TCP TIMEOUT Init \"Slow Start\"(SS) WND_CWND tiene valor (%g) \n",t, this->getFullName().data(),WND_CWND);
		//set SSTHRESH a la mitad de los paquetes en vuelo y a CWND en 1 MSS
		WND_SSTHRESH = WND_CWND / 2;
		WND_SSTHRESH = std::max<uint>(1, WND_SSTHRESH); // Matias: fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)

		WND_CWND = 1;
		WND_firsttime_SSTHRESH=false;
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[InT]@WAITxACK: TCP TIMEOUT -> se reduce a la mitad WND_SSTHRESH(%u),set WND_CWND(%g)=1, set WND_DUP_ACK(%u)=0 \n",t, this->getFullName().data(), WND_SSTHRESH, WND_CWND, WND_DUP_ACK);
		// if (WND_CWND<1) //Matias fix: it was set 'WND_CWND=1', so the if does not make sense.
		//		{
		//			WND_CWND=1;			//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@WAITxACK: WND_CWND<1 entonces seteo WND_CWND=1\n",t);
		//		};
		WND_CWND_precise = WND_CWND ;

		this->logger->logSignal(t,(double)WND_CWND,"cwnd");
		this->logger->logSignal(t, 1,"timeout");

		debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT] TCP TIMEOUT Init \"Slow Start\"(SS) WND_CWND(%g), WND_NXT_SND(%u) \n",t,WND_CWND, WND_NXT_SND);
		//KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]@WAITxACK: LoggerToScilab cwnd=%u\n",t,WND_CWND);
		this->logger->logSignal(t,(double)WND_CWND,"cwnd");
		WND_NXT_SND = 1 ;	   // Update de pointer , NORMAL case
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: WAITxACK (WND_NXT_SND set a valor %u) \n",t,WND_NXT_SND);
		////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[InT]: BUF_SIZE %u WND_CWND %u WND_NXT_SND %u \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND);
		BUF_Fully_SNT = false ; 	//
		WND_Fully_SNT = false ; 	//

		SEQ_LAST_Valid_ACK = -1; 	// prevents identifying the next dup acks that might be on-fly as a repetition of this condition (dup ack is detected only once inside the same cycle)
		// This Signals the SENDING state that
		// the next SEQ number will represent a CYCLE_ACK

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
	int first; int last;
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

	return;
}
void packettcpsnd_ext::dext(Event x, double t) {
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


		p->auxiliar_SEQ = SEQ_NXT ; // Matias: Add to TCP protocol. Why is it named "auxiliar"?
		////KIKE-Prueba
		//SEQ_NXT++ ;
		SEQ_NXT = SEQ_NXT + MSS;//500; //+ packet length
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Data from upper layer - set p->auxiliar_SEQ(%u) Next SEQ(%u) \n",t, this->getFullName().data(),p->auxiliar_SEQ,SEQ_NXT);

		// NEW packet enters the buffer
		SendBuffer.push_back(p);   			// New packets are stored at the tail of the double ended queue
		BUF_Fully_SNT = false; 				// The Buffer gets a new packet, so it is not fully sent for sure
		BUF_SIZE = SendBuffer.size();
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
		//		ReceivedACKedFromPeerLayer=(int)xv[0];	// This is the most recent ACK number that "the other end is confirming as valid"
//		ReceivedACKedFromPeerLayer=pack->auxiliar_SEQ;
		ReceivedACKedFromPeerLayer=tcpProtocol->getACK();
		//		OriginalACKedSEQ_TimeStamp=xv[1];	// TimeStamp of the DATA Packet that Originated this ACK
		OriginalACKedSEQ_TimeStamp=pack->lastModifTime;
		//		SendACKtoPeerLayer=(int)xv[2];			// This is the most actual ACK number "that can be confirmed to the other end"
		SendACKtoPeerLayer=0;
		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: received ACK packet (expecting ACK=%u). pack->ACK=%u ; pack->getId()=#%u ;  ReceivedACKedFromPeerLayer=%u. TCP: WND_CWND(%g), BUF_SIZE(%u), WND_SSTHRESH(%u)  \n",t, this->getFullName().data(),
				SEQ_NEXT_Expected_ACK, tcpProtocol->getACK(), pack->getId(), ReceivedACKedFromPeerLayer, OriginalACKedSEQ_TimeStamp, SendACKtoPeerLayer, WND_CWND, BUF_SIZE, WND_SSTHRESH);

		this->logger->logSignal(t,ReceivedACKedFromPeerLayer,"receiveAck");
		//KIKE-MEJORA: logueo al PCAP el ACK
		//myPcap->writeFrame(t, "10.0.0.2", "10.0.0.1", 10004, 1001, ReceivedACKedFromPeerLayer, ReceivedACKedFromPeerLayer,0);

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Check if ACK is valid. It the following true?: SEQ_NEXT_Expected_ACK(%u) <= ReceivedACKedFromPeerLayer(%u) < SEQ_NXT+ MSS(%u) (modulo-32 arithmetic) \n",t, this->getFullName().data(), SEQ_NEXT_Expected_ACK, ReceivedACKedFromPeerLayer, SEQ_NXT+ MSS);
		if (checkWindow(SEQ_NEXT_Expected_ACK, ReceivedACKedFromPeerLayer, SEQ_NXT+ MSS))	{ // This is a VALID ACK, i.e., the expected sequence number (RFC 793 page 23: http://www.faqs.org/rfcs/rfc793.html#ixzz0lcD37K7J)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: This is a VALID ACK. SEQ_NEXT_Expected_ACK(%u) <= ReceivedACKedFromPeerLayer(%u) < SEQ_NXT+ MSS(%u) (modulo-32 arithmetic) \n",t, this->getFullName().data(), SEQ_NEXT_Expected_ACK, ReceivedACKedFromPeerLayer, SEQ_NXT+ MSS);
			_IS_VALID_ACK_PACKET = true;
			_IS_DUP_ACK_LIMIT_PACKET = false;

			T_MEASURED_RTT = t - OriginalACKedSEQ_TimeStamp ;
			this->logger->logSignal(t,T_MEASURED_RTT,"measuredRTT");

			// move the sendBuffer for each ACK packet
			int count = 0;
			while(checkWindow(SEQ_NEXT_Expected_ACK, ReceivedACKedFromPeerLayer, SEQ_NXT+ MSS)){
				count++;
				debugMsg(LOG_LEVEL_DEBUG,"[%g] %s[dext]: arrived packet #%u (ACK=%u) for %u packets. SEQ_NEXT_Expected_ACK= %u; \n",
						t, this->getFullName().data(), pack->getId(), ReceivedACKedFromPeerLayer, count, SEQ_NEXT_Expected_ACK);


				// OLD packet leaves the buffer
				SendBuffer.pop_front();	// DROP the just ACKNOWLEDGED PACKET FROM THE SEND BUFFER
				BUF_SIZE = SendBuffer.size();

				SEQ_LAST_Valid_ACK = ReceivedACKedFromPeerLayer ;
				////KIKE-Prueba:
				//SEQ_NEXT_Expected_ACK++ ;
				SEQ_NEXT_Expected_ACK += MSS;//500; //+ packet length
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: NEXT Expected ACK is %u \n",t, this->getFullName().data(),SEQ_NEXT_Expected_ACK);

				//KIKE-MEJORA: comento la siguiente linea para agregarla mas abajo con "Fast Recovery"
				//WND_DUP_ACK=0 ;		// Clears the Duplicate ACK counter
				WND_NXT_SND-- ;		// Updates the pointer to the next packet to be sent, because it is relative to the back of the queue
				if (WND_NXT_SND<1) {WND_NXT_SND=1;}; // Saturates at 1 at low values
				debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: ACK valido. Updates the pointer to the next packet to be sent(decremento WND_NXT_SND:%u) \n",t,WND_NXT_SND);

//				if(SEQ_NEXT_Expected_ACK<(SEQ_NEXT_Expected_ACK-MSS)) {// there was an overflow
////					SEQ_NEXT_Expected_ACK = MSS; // start again
//					debugMsg(LOG_LEVEL_DEBUG,"[%g] %s[dext]: Overflow in the received ACK. SEQ_NEXT_Expected_ACK=%f \n", t, this->getFullName().data(), SEQ_NEXT_Expected_ACK);
//					//break; // accept the ACK until here (we are sure the tcp_rx will not send cumulative ACK when overflow
//				}


				//KIKE-MEJORA: agrego "Fast Recovery"
				if(WND_DUP_ACK >= DUP_ACK_LIMIT )
				{
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: ACK arrived out of order WND_DUP_ACK(%u)>=DUP_ACK_LIMIT(%u)\n",t, this->getFullName().data(), WND_DUP_ACK,DUP_ACK_LIMIT);

					//Perform "Fast Recovery":
					WND_CWND = WND_SSTHRESH; // exit FR
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: (PRUEBO GUARDAR CADA VEZ que CAMBIA) LoggerToScilab cwnd=%g\n",t, this->getFullName().data(),WND_CWND);
					this->logger->logSignal(t,(double)WND_CWND,"cwnd");

					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Valid Ack, Perform \"Fast Recovery\": WND_CWND = WND_SSTHRESH(%u) and go to CA\n",t, this->getFullName().data(),WND_SSTHRESH);
				} else {
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Not a duplicate ACK => perform window and RTT calculations \n",t, this->getFullName().data(),ReceivedACKedFromPeerLayer);



						T_RTT = RTT_alfa * T_RTT + (1-RTT_alfa) * T_MEASURED_RTT ;
						this->logger->logSignal(t,(double)T_RTT,"calcRTT");

						T_RTT_DEVIATION = RTT_alfa * T_RTT_DEVIATION + (1-RTT_alfa) * fabs(T_RTT - T_MEASURED_RTT);

						if (T_FORCED_RTO>=0) {
							T_RTO = T_FORCED_RTO ;                  // FORCED
						} else {
							T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
						}
						T_RTO = std::max(MIN_RTO, T_RTO);

						debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: New calculation of the RTO=%g. T_RTT=%g ; T_MEASURED_RTT=%g; T_RTT_DEVIATION=%g \n",t, this->getFullName().data(), T_RTO, T_RTT, T_MEASURED_RTT, T_RTT_DEVIATION);
						this->logger->logSignal(t,T_RTO,"RTO");
						this->logger->logSignal(t,T_RTT_DEVIATION,"RTO_dev");

						//KIKE//if (WND_CWND<WND_SSTHRESH && WND_firsttime_SSTHRESH==true) {
						if (WND_CWND<WND_SSTHRESH) { // Slow Start
							debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: we are in Slow Start (WND_CWND=%g >= WND_SSTHRESH=%u) \n", t, this->getFullName().data(), WND_CWND, WND_SSTHRESH);
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
							debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: we are in Congestion Avoidance (WND_CWND=%g >= WND_SSTHRESH=%u) \n", t, this->getFullName().data(), WND_CWND, WND_SSTHRESH);
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

						debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Updated window: WND_CWND=%g ; WND_SSTHRESH=%g   \n",t, this->getFullName().data(),WND_CWND, WND_SSTHRESH);
						this->logger->logSignal(t,(double)WND_CWND,"cwnd");
				}
				WND_DUP_ACK=0 ;
		}
		} else {					// This is an INVALID ACK, an unexpected acked sequence number
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: ACK %u is INVALID. last valid: %u ; expected: %u  \n", t, this->getFullName().data(), ReceivedACKedFromPeerLayer, SEQ_LAST_Valid_ACK, SEQ_NEXT_Expected_ACK);

			_IS_VALID_ACK_PACKET = false;
			_IS_DUP_ACK_LIMIT_PACKET = false;

			if (ReceivedACKedFromPeerLayer==SEQ_LAST_Valid_ACK) {  // It is a DUPLICATED ACK
				WND_DUP_ACK++;
				this->logger->logSignal(t,(double)WND_DUP_ACK,"dupACK");
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: This is DupACK number %u  \n", t, this->getFullName().data(), WND_DUP_ACK);

				if (WND_DUP_ACK==DUP_ACK_LIMIT) {		// and Reached LIMIT duplicate ACKs
					debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Reached the DupACK Limit (DUP_ACK_LIMIT=%u), so start Fast Recovery. Current WND_CWND=%g ; WND_SSTHRESH=%g \n", t, this->getFullName().data(), DUP_ACK_LIMIT, WND_CWND, WND_SSTHRESH);

					_IS_DUP_ACK_LIMIT_PACKET = true;
					this->logger->logSignal(t,(double)WND_DUP_ACK,"tripleDupACK");

					//SEQ_NextAcceptedECNMark = SendBuffer.at(WND_NXT_SND-1)->auxiliar_SEQ; // ignore ECN marks for already sent packets. It is not exactly the same as in the RFC (because in-flight != CWND). See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"
					SEQ_NextAcceptedECNMark = ReceivedACKedFromPeerLayer + (WND_CWND * MSS);; // ignore ECN marks for the next window of data. See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"

					//WND_DUP_ACK=0 ;//KIKE-MEJORA: comento para poder manejar los siguientes dupacks...
					//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: WND_CWND / 2\n",t);
					//KIKE HOY el tema es esto seria FR pero debe hacerlo al comienzo y luego cwnd=cwnd+1 x c/DupACK....
					WND_SSTHRESH = WND_CWND / 2;//KIKE-MEJORA: Efecto: en CA la WND_CWND aumenta menos?: xq en SS compara con Ssthresh y define el Max cwnd....
					WND_SSTHRESH = std::max<uint>(1, WND_SSTHRESH); // Matias fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)
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

			//KIKE-MEJORA: El valor de CWND se va al carajo, ver cual es el limite (ej: flight_size = std::min(state->snd_cwnd, state->snd_wnd); ó hasta
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

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext] Packet #%u has ECN mark. (sigma=%g, WND_CWND(%g), WND_SSTHRESH(%u), WND_DUP_ACK(%u))\n",t, this->getFullName().data(), pack->getId(), sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK);
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: BUF_SIZE=%u ; WND_CWND=%u  ; WND_NXT_SND=%u \n", t, this->getFullName().data(), BUF_SIZE, WND_CWND, WND_NXT_SND);


			SEQ_NextAcceptedECNMark = ReceivedACKedFromPeerLayer + (WND_CWND * MSS); // Ignore ECN marks for the next window of data. See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"

			WND_SSTHRESH = WND_CWND / 2;
			WND_SSTHRESH = std::max<uint>(1, WND_SSTHRESH); // Matias fix to avoid WND_SSTHRESH=0 (illegitimate model on fast-retransmit)
			WND_CWND = std::max(1.0, std::abs(WND_CWND / 2)) ;//KIKE asi estaba hasta ahora://WND_CWND = abs(WND_CWND / 2);

			WND_CWND_precise = WND_CWND ;
			WND_DUP_ACK=0 ; // reset dup-ack counter. "TCPP source should not decrease the slow-start threshold, ssthresh, if it has been decreased within the last round trip time.  "

			// things different from the dup-ACK case
			//WND_NXT_SND = 1 ;	 // We do not retransmit, keep pointing to the same next packet as before
			//BUF_Fully_SNT = false ;
			//WND_Fully_SNT = false ;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext] ECN mark reduced window :  (sigma=%g, WND_CWND(%g), WND_SSTHRESH(%u), WND_DUP_ACK(%u)) \n",t, this->getFullName().data(), sigma, WND_CWND, WND_SSTHRESH, WND_DUP_ACK);
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: BUF_SIZE=%u ; WND_CWND=%u  ; WND_NXT_SND=%u \n", t, this->getFullName().data(), BUF_SIZE, WND_CWND, WND_NXT_SND);

			//			SEQ_NextAcceptedECNMark = SendBuffer.at(WND_NXT_SND-1)->auxiliar_SEQ; // ignore ECN marks for already sent packets. It is not exactly the same as in the RFC (because in-flight != CWND). See RFC3168, 6.1.2: "TCP should not react to congestion indications more than once every window of data"


			this->logger->logSignal(t,(double) WND_CWND,"cwnd");
		}
	}


	int first; int last; // Matias: These variables are set but not used except for debug. The following 'if' is useless
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
	} else {
		first=0;
		last=0;
		//effectiveWin = std::min(131.0, WND_CWND);
	};
	//char *sEf = (char *)((effectiveWin < 0) ? "NEGATIVO" : "Positivo");

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: BUF_SIZE %u WND_CWND %g WND_NXT_SND %u (%i-%i) \n",t, this->getFullName().data(), BUF_SIZE, WND_CWND, WND_NXT_SND,first,last);



	// According to the State do:
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: manejo el estado:%u\n",t,mystate);
	switch (mystate)
	{
	case WAITxAPP: //  Waiting for data from the upper layer
		if(SndPktAcks_Record_log)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: ext@ WAITxAPP [BUF_SIZE(%u)<WND_CWND(%g) && p->lastQueueSizeAfterLeaving(%u)>0]\n",t, this->getFullName().data(),BUF_SIZE,WND_CWND,p->lastQueueSizeAfterLeaving);
		if(_IS_DATA_PACKET) {
			if (BUF_SIZE<WND_CWND && p->lastQueueSizeAfterLeaving>0) {   // The Congestion Window allows the Buffer to be filled with more packets
				// AND there are more to grab from the Upper Layer
				// (Matias: ¿and how does it get notified when there is more to grap from the Upper Layer? -->(model gets stuck until ACK is received). But if lastQueueSizeAfterLeaving is not taken into account the model will stop tracking the rest (ej:RTO) until next packet from upper layer arrives. (I dont see an easy fix)
				// State Transition
				mystate=tr_REQxAPP;		//
				sigma=0;
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Ext]: to tr_REQxAPP during %g \n",t,sigma);
				//
				break;
			} else { 					// The upper layer buffer is empty
				// OR The Congestion Window is Full
				// State Transition
				mystate=SENDING;		//
				sigma=INTERPACKET_SND_TIME;
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Ext]: to SENDING during %g \n",t,sigma);
				//KIKE//Prueba: intento que si tengo data enviarla inmediatamente //KIKE-MEJORA
				//sigma=0;//(==INTERPACKET_SND_TIME=0)(K1)
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Ext]: to SENDING during %g (**modificado**) \n",t,sigma);
				break;
				//
			};
		};

	case tr_REQxAPP: // Asks for more data from the upper layer
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: ext@ tr_REQxAPP \n",t);
		// State Transition
		sigma=sigma-e;
		break;

	case SENDING: // Flushing the sender window
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: ext@ SENDING \n",t);
		// State Transition
		sigma=sigma-e;
		break;

	case WAITxACK:  //  Waiting for ACKs from the pair RCV node
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: ext@ WAITxACK \n",t);
		if(_IS_ACK_PACKET) {
			if (_IS_VALID_ACK_PACKET) {
				// State Transition
				mystate=tr_REQxAPP; 	//  Forces an ACK timeout, to consider retransmissions (but here a triple dup ACK happened)
				sigma=0;
				//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND:[WAITxACK:] to tr_REQxAPP during %g \n",t,sigma);
				break;
			} else {
				if (_IS_DUP_ACK_LIMIT_PACKET) {
					if(SndPktAcks_Record_log)
						debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[ExT]: DUP_ACK_LIMIT -> Try to retransmit buffer \n",t,sigma);
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
				};
			};
		};

		if(_IS_DATA_PACKET) {
			// State Transition
			mystate=WAITxACK;  	//  Remains where it was
			sigma=T_RTO ;  		//  Waits again a full TIMEOUT period
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND:[Is Data] to WAITxACK during %g \n",t,sigma);
		}

		break ;
	default:
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: Error: Do not know what to do... \n",t);
		break;
	}
	return ;
}

Event packettcpsnd_ext::lambda(double t) {
	////KIKE//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND: [OUT] \n",t);

	// For ANY current State do:

	double out_cwnd[10]; for (int i=0;i<10;i++) {out_cwnd[i]=0;}
	out_cwnd[0]=(double)WND_CWND;

	// According to the current State do:

	switch (mystate)
	{
	case tr_REQxAPP: // Asks for more data from the upper layer
		debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[Out]: Asks for more data from the upper layer. tr_REQxAPP (out_cwnd=%g) \n",t, this->getFullName().data(), out_cwnd[0]);
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
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: [SENDING] BUF_SIZE(%u)> 0 && BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g) [SendBuffer.size(%u)]\n",t, this->getFullName().data(),BUF_SIZE, BUF_SIZE,WND_NXT_SND,WND_CWND, SendBuffer.size());

		if (BUF_SIZE>0 && WND_NXT_SND<=BUF_SIZE && WND_NXT_SND<=WND_CWND)  { // Send Buffer !empty AND !completely sent KIKE//Prueba: && effectiveWin>0
			//if(SndPktAcks_Record_log)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpSND[Out]: BUF_SIZE(%u)>=WND_NXT_SND(%u)<=WND_CWND(%g)\n",t,BUF_SIZE,WND_NXT_SND,WND_CWND);
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
		throw std::runtime_error("packettcpsnd_ext::lambda - tcpSND: Error: Do not know what to do... \n");
		return Event();
		break;
	}
}

bool packettcpsnd_ext::validateAck(std::shared_ptr<NetworkPacket> packet){
	// Get the IP (from the TCP protocol)
	auto protocol = packet->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "%s_validatePacket: received packet #%u without tcp protocol. discarding. \n ", this->getFullName().data(), packet->getId());
		packet->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("packettcpsnd_ext::validatePacket: received packet without tcp protocol.");
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

/*
 The  fundamental  function  for  comparing  sequence numbers.
 This function returns true if Sequence is contained in the semi-open segment between Left  and  Right.
 RFC for TCP modulo 32 arithmetics: https://www.rfc-editor.org/ien/ien74.txt
 */
bool packettcpsnd_ext::checkWindow(uint left, uint sequence, uint right){
	if(left <= right){ // (L le R) =>
		 return left <= sequence && sequence < right; // L le S and S lt R  ,
	} else {
		 return ! (right <= sequence && sequence < left) ;// not ( R le S and S lt L)
	}
}

