#include "packetgen.h"
void packetgen::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);
	char* fvar;

	// read parameters
	this->flowId = va_arg(parameters,char*);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: FlowID: %s \n", t, this->getFullName().data(), this->flowId.data());

	fvar = va_arg(parameters,char*);
	this->period = readDistributionParameter(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: Period: %s \n",t, this->getFullName().data(), this->period->getFullName().data());

	fvar = va_arg(parameters,char*);
	this->packetSize = readDistributionParameter(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: Packet Size: %g bits \n",t, this->getFullName().data(), this->packetSize->getFullName().data());

	fvar = va_arg(parameters,char*);
	this->packetOverheadSize = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: Packet Overhead Size: %g bits \n",t, this->getFullName().data(), packetOverheadSize);

	fvar = va_arg(parameters,char*);
	auto startStopTimes_vector = readDefaultParameterValue<std::vector<double>>(fvar);
	this->startStopTimes = std::deque<double>(startStopTimes_vector.begin(), startStopTimes_vector.end());
	this->startStopTimes.push_back(std::numeric_limits<double>::infinity()); // add a last value with inf (last stop/start is at infinity) to facilitate implementation (startStopTimes.front() should never fail)
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: StartTime: %g secs. \n",t, this->getFullName().data(),startStopTimes.front());

	sigma=startStopTimes.front();
	if (this->sigma>0){
		this->generationStopped = true;
	} else {
		this->generationStopped = false;
		startStopTimes.pop_front(); // pop the zero
	}

	counter=0;
	this->logger->initSignals(std::vector<std::string>{"pktlength","intergen","forcedGeneration"});

	return;
}

void packetgen::dint(double t) {
	if(t >= startStopTimes.front()){
		this->generationStopped = !this->generationStopped;
		startStopTimes.pop_front();
	}

	sigma = this->period->nextValue();
	if(this->generationStopped){ // stop until next start
		sigma = startStopTimes.front() - t; // use start time
	}

	this->logger->logSignal(t,sigma,"intergen");
}

void packetgen::dext(Event x, double t) {
	// check if the startStop time already elapsed
	if(t >= startStopTimes.front()){
		this->generationStopped = !this->generationStopped;
		startStopTimes.pop_front();
	}

	if (x.port==0) {   // force generation of new packet
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: force generation signal arrived \n",t, this->getFullName().data());

		this->logger->logSignal(t, 1, "forcedGeneration");

		if(this->generationStopped){ // ignore message and continue as before
			this->sigma = startStopTimes.front() - t; // continue as before
		} else {
			this->sigma = 0; // program transition immediately
			this->logger->logSignal(t, e, "intergen");
		}

	} else  {
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s::dext: unknown port %i \n ", t, this->getFullName().data(), x.port);
		throw std::runtime_error("packetgen::dext unknown port \n");
	}
}

Event packetgen::lambda(double t) {
	///////////////
	// ADDs new protocols to the packet to track them along a full network path
	//  for (int i=1; i<=1; i++) {
	//  	  myprotoStack.push_back(myprotocol); // 1 2 3 4 5
	//	  myprotocol.protoID++;
	//	  myFields[SRC]++;
	//	  myFields[DST]++;
	//	  myFields[SEQ]++;
	//	  myprotocol.headerFields=myFields;
	//	  };

	///////////////
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] PacketGen: myprotoStack contains: \n",t);
//	for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
//		//debugMsg(LOG_LEVEL_DEBUG, "[%g] PacketGen: ID %u \n",t,(*myprotoStackit).protoID);
//		myFields = (*myprotoStackit).headerFields;
//		for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ ){
//			//debugMsg(LOG_LEVEL_DEBUG, "[%g] PacketGen: field value %g \n",t,(*mapIt).second);
//		}
//	}

	counter++;
	auto packet = std::make_shared<NetworkPacket>(t, this->flowId);

	// TCP protocol
	int pSize = this->packetSize->nextValue();
	if(pSize<=0){
		pSize=0;
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: packetgen::lambda distribution (%s) yield a <=0 packet size %i. Review distribution parameters: %s. \n ", t, this->getFullName().data(), this->packetSize->getConfigurationName().data(), pSize, this->packetSize->getFullName().data());
		//throw std::runtime_error(std::string("packetgen::lambda distribution yield a <=0 packet size. Review distribution ") +  this->packetSize->getConfigurationName().data() + "\n");
	}
	auto dummyProtocol = std::make_shared<DummyProtocol>(pSize);

	// add protocols to packets
	packet->addProtocol(dummyProtocol);

//	p->IPsrc = "10.0.0.1";
//	p->IPdst = "10.0.0.2";
//	p->PORTsrc = 1000;
//	p->PORTdst = 20001;
//	p->ACK = 1;
//	p->payload = "DataFromApp";
	///////////////
	// EXPERIMENTAL
	//debugMsg(LOG_LEVEL_DEBUG, "executing experimental assingments... \n");
	//int experiment_int=45;
	//char experiment_char='W';
	//double experiment_double=5.456;
	//p->experimentalList.push_back (&experiment_int);
	//p->experimentalList.push_back (&experiment_char);
	//p->experimentalList.push_back (&experiment_double);
	//debugMsg(LOG_LEVEL_DEBUG, "... int %u \n",*(int*)p->experimentalList.front());
	//p->experimentalList.pop_front();
	//debugMsg(LOG_LEVEL_DEBUG, "... int %c \n",*(char*)p->experimentalList.front());
	//p->experimentalList.pop_front();
	//debugMsg(LOG_LEVEL_DEBUG, "... int %g \n",*(double*)p->experimentalList.front());
	//p->experimentalList.pop_front();
	//debugMsg(LOG_LEVEL_DEBUG, "... done ! \n");

//	p->ComplexityFactor=1;
//	p->birthTime=t;



	// FOR TESTING PURPOSES ONLY:
	// myprotoStack.back().METADATA[OVERHEAD] = 10*8;
	// myprotoStack.back().METADATA[PAYLOADSIZE] = (int)mySTDEVS->exponential(490*8)%(1000*8);
	//KIKE-Prueba: separar Header del payload para loguear a PCAP.
//	myprotoStack.back().METADATA[OVERHEAD] = this->packetOverheadSize;//47*8;//Equiv a OMNeT Packet Headers: TCPh(20) + IPh(20) + Frh(7)
//	myprotoStack.back().METADATA[PAYLOADSIZE] = pSize;
	// myprotoStack.back().METADATA[PAYLOADSIZE] = (int)mySTDEVS->exponential(Psize); // Random size
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] PacketGen: created packet #%i with tiemstamp %g \n",t, p->ID, p->birthTime);

//	p->ProtocolList = myprotoStack;


	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda]: Sending Packet #%u (%s) with %u bits. \n",t,this->getFullName().data(), packet->getId(), packet->flowId.value.data(), packet->getLength_bits());
	this->logger->logSignal(t, packet->getLength_bits(),"pktlength");

	return Event(packet, 0);
}
