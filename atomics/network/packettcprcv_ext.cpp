#include "packettcprcv_ext.h"
void packettcprcv_ext::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	// Read parameters
	char* fvar;
	fvar = va_arg(parameters,char*);
	MSS = readDefaultParameterValue<uint>(fvar);	// MAX Segment Size
	debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpRCV: Param MSS: %u \n",t,MSS);

	fvar = va_arg(parameters,char*);
	this->ipSender = fvar;
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: ipSender: %s  \n",t, this->getFullName().data(), this->ipSender.data());

	fvar = va_arg(parameters,char*);
	this->ipReceiver = fvar;
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: ipReceiver: %s  \n",t, this->getFullName().data(), this->ipReceiver.data());

	fvar = va_arg(parameters,char*);
	this->portSender = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: portSender: %d  \n",t, this->getFullName().data(), this->portSender);

	fvar = va_arg(parameters,char*);
	this->portReceiver = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: portReceiver: %d  \n",t, this->getFullName().data(), this->portReceiver);

	fvar = va_arg(parameters,char*);
	this->delayACK = readDefaultParameterValue<double>(fvar);
	if(this->delayACK<0) this->delayACK=0;
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: delayACK: %f sec \n",t, this->getFullName().data(), this->delayACK);


	this->logger = std::make_shared<ConfigurationLogger>(this);
	this->logger->initSignals(std::vector<std::string>{"ACK","packetreceive"});

	SEQ_NEXT_Expected=1;

	SEQ_NXT = 1;			// Next Sequence Number to use for sending
	DATA_SEQ_NUMBER = 0 ;

	sigma = std::numeric_limits<double>::infinity();

	return;
}

void packettcprcv_ext::dint(double t) {
	this->arrivedPacket.reset();
	sigma = std::numeric_limits<double>::infinity();
	debugMsg(LOG_LEVEL_DEBUG, "[%g]  %s[dint]: sigma=%g \n",t, this->getFullName().data(), this->sigma);
	return;
}

void packettcprcv_ext::dext(Event x, double t) {
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpRCV: [EXT] \n",t);

	// For ANY current State do:
	if (x.port==0)	{ // Data
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s_dext: received packet #%u. \n ", t, this->getFullName().data(), packet->getId());
		bool seqOverflow = false;

		// validate if the packet is for me
		if(!validatePacket(packet)){
			debugMsg(LOG_LEVEL_ERROR, "[%g] WARNING - %s_dext: packet #%u was not intended for this TCP session (different SRC/DST or PORTS). Ignoring. \n ", t, this->getFullName().data(), packet->getId());
			this->sigma = this->sigma - e; // continue as before
			return;
		}

//		if(this->arrivedPacket){
//			debugMsg(LOG_LEVEL_ERROR, "[%g] WARNING - %s_dext: packet #%u arrived before the previous ACK (from packet #%u) was sent. TODO: implement a packet queue \n ", t, this->getFullName().data(), packet->getId(), this->arrivedPacket->getId());
//			throw std::runtime_error("packettcprcv_ext::dext: packet arrived before the previous ACK was sent. TODO: implement a packet queue in the TCP_rcv");
//		}

		this->arrivedPacket = packet;
		DATA_SEQ_NUMBER = arrivedPacket->auxiliar_SEQ;
		DATA_TCP_SND_TIMESTAMP = arrivedPacket->lastModifTime;
		this->logger->logSignal(t,arrivedPacket->getId(),"packetreceive");
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] tcpRCV: Packet ID %u birthtime %g SEQ %u ExpSEQ %u Length %u \n",t, arrivedPacket->getId(),arrivedPacket->birthTime,DATA_SEQ_NUMBER,SEQ_NEXT_Expected,arrivedPacket->getLength());

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: [Packet ID: %u] SEQ: %u ExpSEQ: %u \n",t, this->getFullName().data(), arrivedPacket->getId(),arrivedPacket->auxiliar_SEQ,SEQ_NEXT_Expected);


		if (DATA_SEQ_NUMBER==SEQ_NEXT_Expected)	{ // This is a VALID ACK, the expected sequence number
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: llego paquete #%u con la secuencia esperada. Arrived DATA_SEQ_NUMBER=%u  ; SEQ_NEXT_Expected=%u \n",t, this->getFullName().data(), arrivedPacket->getId(), DATA_SEQ_NUMBER, SEQ_NEXT_Expected);
			SEQ_DUP = 0 ;			// Clears the Duplicate ACK counter
			//KIKE-prueba: considero long de los paquetes en bytes
			//SEQ_NEXT_Expected ++ ;
			int before=SEQ_NEXT_Expected;
			SEQ_NEXT_Expected = SEQ_NEXT_Expected + MSS;//500;
			if(SEQ_NEXT_Expected<0 && before>0) {// there was an overflow
//				SEQ_NEXT_Expected = MSS; // start again
				seqOverflow = true;
			}
		} else {						// This is an INVALID ACK, an unexpected acked sequence number
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: Llego paquete #%u sin la secuencia esperada. Arrived DATA_SEQ_NUMBER=%u  ; SEQ_NEXT_Expected=%u  \n",t, this->getFullName().data(), arrivedPacket->getId(), DATA_SEQ_NUMBER, SEQ_NEXT_Expected);
			SEQ_DUP ++ ;			// Clears the Duplicate ACK counter
		}

		// Delayed ACK
		if(std::isinf(this->sigma)){ // first ACK before delay
			sigma = this->delayACK;
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: First ACKed packets. Will hold for %fs to deliver it.  \n",t, this->getFullName().data(), this->sigma);
		} else { // we were already delaying another ACK, so continue with the same transition as before
			this->sigma = this->sigma - e; // continue as before
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: ACKs already waiting on hold. Will hold for %fs more to deliver them in one go.  \n",t, this->getFullName().data(), this->sigma);
		}

		if(seqOverflow){
			sigma = 0; // transition immediately
			debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[dext]: OVERFLOW. Deliverig now %f  \n",t, this->getFullName().data(), this->sigma);
		}
	}
}

bool packettcprcv_ext::validatePacket(std::shared_ptr<NetworkPacket>& packet){
	// Get the IP (from the TCP protocol)
	auto protocol = packet->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "%s_validatePacket: received packet #%u without tcp protocol. discarding. \n ", this->getFullName().data(), packet->getId());
		packet->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("packettcprcv_ext::validatePacket: received packet without tcp protocol.");
	}

	// cast
	std::shared_ptr<TcpIpProtocol> ipProtocol = std::static_pointer_cast<TcpIpProtocol>(protocol);

	// validate
	if(ipProtocol->getIpSrc() != this->ipSender ||
	   ipProtocol->getIpDst() != this->ipReceiver ||
	   ipProtocol->getPortSrc() != this->portSender ||
	   ipProtocol->getPortDst() != this->portReceiver){
		return false;
	}

	return true;
}

Event packettcprcv_ext::lambda(double t) {
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda]: SEQ_LAST_Valid:%u \n", t, this->getFullName().data(), SEQ_LAST_Valid);
	this->logger->logSignal(t,(double)SEQ_NEXT_Expected,"ACK");

	auto packet = std::make_shared<NetworkPacket>(t, this->arrivedPacket->flowId.value);
	packet->auxiliar_SEQ = 1;//Son ACKs
	packet->ComplexityFactor=1;
	packet->lastModifTime = (double)DATA_TCP_SND_TIMESTAMP; // Matias: Added 'lastModifTime' to measure RTT in the sender (TODO:change name of the variable and put it in the protocol not the packet!)

	// TCP protocol
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda]: creating TCP packet #%u  \n", t, this->getFullName().data(),packet->getId());
	auto arrivedTcpProtocol = std::static_pointer_cast<TcpIpProtocol>(arrivedPacket->getProtocol(PROTOCOL_ID_TCP_IP));
	auto tcpProt = std::make_shared<TcpIpProtocol>(0, arrivedTcpProtocol->getIpDst(), arrivedTcpProtocol->getIpSrc(), arrivedTcpProtocol->getPortDst(), arrivedTcpProtocol->getPortSrc()); // swap ip::port Src<->Dst
	tcpProt->setACK(SEQ_NEXT_Expected);
	tcpProt->setPayload(""); // Matias: is this needed?
	tcpProt->setMarkCongestion(arrivedTcpProtocol->getMarkCongestion()); // forward ECN mark
	packet->addProtocol(tcpProt);


	// Routing protocol with the route determined by incomming packet
	if(!arrivedPacket->hasProtocol(PROTOCOL_ID_ABSTRACT_ROUTING)){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: WARNING: received packet #%u without routing protocol. Continuing \n ", t, this->getFullName().data(), arrivedPacket->getId());
	} else {
	    auto protocol = arrivedPacket->getProtocol(PROTOCOL_ID_ABSTRACT_ROUTING); // get the route (from the protocol)
		auto route = std::static_pointer_cast<RoutingProtocol>(protocol)->getRoute();
		// Check this is the appropriate node in the route
		if(!(route->getNextJump().nodeName == this->getName() ||
				route->getNextJump().nodeName == this->father->getName() ||
				(this->father->father != nullptr && route->getNextJump().nodeName == this->father->father->getName()))){
			debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u which was not destined for this node (next jump: %s). Discarding.  \n ", t, this->getFullName().data(), arrivedPacket->getId(), route->getNextJump().nodeName.data());
			arrivedPacket->printInfo(LOG_LEVEL_ERROR);
			throw std::runtime_error(this->getFullName() + ":received packet which was not destined for this node (next jump was '"+ route->getNextJump().nodeName +"') \n");
		}

		auto routingProt = std::make_shared<RoutingProtocol>(route); // we need a copy of the route, as it will be modified along the way
		routingProt->getRoute()->popJump(); // before sending the job, remove this node from the route
		packet->addProtocol(routingProt);
	}


//	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: Packet ID(%i) myprotoStack.back().METADATA[PAYLOADSIZE](%g) \n",t, this->getFullName().data(),pout->getId(), myprotoStack.back().METADATA[PAYLOADSIZE]);
//	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: Packet ID(%i) flowId(%s) pout->getPayloadLength(%g) \n",t, this->getFullName().data(),pout->getId(), pout->flowId.value.data(), pout->getPayloadLength());

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[lambda]: Sending packet ID(#%u) SEQ(%u) ACK(%u) Len(%u) FlowID(%s)\n",t, this->getFullName().data(),
			packet->getId(),packet->auxiliar_SEQ, arrivedTcpProtocol->getACK(),packet->getLength_bits(),packet->flowId.value.data());
	return Event(packet,0);
}
