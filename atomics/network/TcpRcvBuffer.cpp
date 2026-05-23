#include "TcpRcvBuffer.h"

#include "NetworkData/Protocol/TcpPacketizationProtocol.h"

void TcpRcvBuffer::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char* fvar;
	fvar = va_arg(parameters,char*);
	MSS = readDefaultParameterValue<int>(fvar);	// MAX Segment Size
	debugMsg(LOG_LEVEL_INIT, "[%f] MSS = %d \n",t, this->getFullName().data(), MSS);

	nextExpectedSeqNumber = 1; // as in packetTcpRcv.cpp

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void TcpRcvBuffer::dint(double t) {
	if(outputPackets.size() != 0){ // there are pending packet to be forwarded
		sigma = 0;
	} else {
		sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives;
	}
}

void TcpRcvBuffer::dext(Event x, double t) {
	auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Arrived packet #%u FlowID=%s \n",t, this->getFullName().data(), packet->getId(), packet->flowId.value.data());

	// Non-TCP packets
	int port;
	if(!packet->hasProtocol(PROTOCOL_ID_TCP_IP)){
		port = NON_TCP_OUTPORT;
		debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received packet #%u FlowID=%s without TCP protocol. Scheduling send on port=%d. \n ", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), port);
	} else{
		auto protocol = packet->getProtocol(PROTOCOL_ID_TCP_IP);
		if(!protocol){
			port = NON_TCP_OUTPORT;
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received packet #%u FlowID=%s failed to get TCP protocol. Scheduling send on port=%d. \n ", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), port);
		} else if(std::static_pointer_cast<TcpIpProtocol>(protocol)->getACK() < 0){ // invalid ACK treated as non TCP
			port = NON_TCP_OUTPORT;
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received packet #%u FlowID=%s with invalid ACK. Scheduling send on port=%d. \n ", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), port);
		} else { // valid TCP packet
			port = ALL_TCP_OUTPORT;
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received TCP packet #%u with valid ACK, FlowID=%s.  Scheduling send on port=%d (all TCP) and processing packet for ordered forwarding  \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), port);

			// Order and remove duplicated TCP packets
			proccessTcpPacket(packet, t);
		}
	}

	// set packet to be forwarded
	this->outputPackets.push({ port, packet});
	sigma = 0;
}

Event TcpRcvBuffer::lambda(double t) {
	auto p = outputPackets.front();
	outputPackets.pop();

	auto packet2Send = p.packet;
	auto port = p.outgoingPort;

	// de-fragment packets that were fragmented during TCP packetization
	// only forward the last fragment which contains the original message
	if(port == ORDERED_TCP_OUTPORT && packet2Send->hasProtocol(PROTOCOL_ID_TCP_PACKETIZATION)){
		auto protocol = std::static_pointer_cast<TcpPacketizationProtocol>(packet2Send->getProtocol(PROTOCOL_ID_TCP_PACKETIZATION));
		if(protocol->isLastFragment()){ // forward orginal packet in last segment
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: swapping packet #%u (FlowID=%s, SEQ=%u) it is the last fragment of the packetization \n", t, this->getFullName().data(), packet2Send->getId(), packet2Send->flowId.value.data(), packet2Send->auxiliar_SEQ);
			packet2Send = protocol->getOriginalPacket();			
		} else { // skip intermediate fragments
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: skipping packet #%u (FlowID=%s, SEQ=%u) as it is not last fragment of the packetization \n", t, this->getFullName().data(), packet2Send->getId(), packet2Send->flowId.value.data(), packet2Send->auxiliar_SEQ);
			return Event(); // do not output
		}
	}

	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: forwarding packet #%u (FlowID=%s, SEQ=%u) on port=%d. nextExpectedSeqNumber=%u ; out-of-order buffer size=%u ; outputPackets size=%u\n", t, this->getFullName().data(), packet2Send->getId(), packet2Send->flowId.value.data(), packet2Send->auxiliar_SEQ, p.outgoingPort, nextExpectedSeqNumber, oooBuffer.size(), outputPackets.size());
	return Event(packet2Send, port);
}


void TcpRcvBuffer::proccessTcpPacket(std::shared_ptr<NetworkPacket> packet, double t){
	auto tcp_protocol = packet->getProtocol(PROTOCOL_ID_TCP_IP); // already validated before
	auto seqNumber = packet->auxiliar_SEQ;

	// TODO: support wrapping of SEQ number
	if(seqNumber < nextExpectedSeqNumber) { // packet already forwarded, discard it
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_proccessTcpPacket: packet #%u (FlowID=%s, SEQ=%u) already forwarded before (ignoring). nextExpectedSeqNumber=%u \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), seqNumber, nextExpectedSeqNumber);
	} else if(oooBuffer.count(seqNumber)) {// packet already in buffer, discard it
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_proccessTcpPacket: packet #%u (FlowID=%s, SEQ=%u) already in buffer (ignoring). nextExpectedSeqNumber=%u \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), seqNumber, nextExpectedSeqNumber);

	} else if (seqNumber == nextExpectedSeqNumber){ // it is the packet we were expecting, forward it
		nextExpectedSeqNumber += MSS; // increase expected SEQ
		this->outputPackets.push({ ORDERED_TCP_OUTPORT, packet});
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_proccessTcpPacket: packet #%u (FlowID=%s, SEQ=%u) has the expected SEQ, scheduling to forward it. nextExpectedSeqNumber=%u \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), seqNumber, nextExpectedSeqNumber);

		// check if now we can forward more packets that were OoO
		auto it = oooBuffer.begin(); // std::map guarantees orders iterators: begin() gets the smaller SEQ
		while(oooBuffer.size() > 0 && it != oooBuffer.end() && it->second->auxiliar_SEQ == nextExpectedSeqNumber){
			auto oooPacket = it->second;
			this->outputPackets.push({ ORDERED_TCP_OUTPORT, oooPacket});
			nextExpectedSeqNumber += MSS; // increase expected SEQ
			oooBuffer.erase(oooPacket->auxiliar_SEQ);

			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_proccessTcpPacket: packet #%u (FlowID=%s, SEQ=%u) was out-of-order and now has the expected SEQ, scheduling to forward it. nextExpectedSeqNumber=%u; out-of-order buffer size=%u \n", t, this->getFullName().data(), oooPacket->getId(), oooPacket->flowId.value.data(), oooPacket->auxiliar_SEQ, nextExpectedSeqNumber, oooBuffer.size());

			it = oooBuffer.begin(); // try with the next one
		}
	} else { // out-of-order (seqNumber > nextExpectedSeqNumber) and not in the buffer. Buffer it, do not forward
		// insert packet into the buffer
		oooBuffer.insert({seqNumber, packet});

		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_proccessTcpPacket: packet #%u (FlowID=%s, SEQ=%u) is out-of-order. buffering. nextExpectedSeqNumber=%u ; out-of-order buffer size=%u ; First SEQ on buffer=%u \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), seqNumber, nextExpectedSeqNumber, oooBuffer.size(), oooBuffer.begin()->second->auxiliar_SEQ);
	}
}

