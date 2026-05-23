#include "TcpACKSplit.h"

void TcpACKSplit::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

//	char* fvar;
//	fvar = va_arg(parameters,char*);
//	this->modulo = readDefaultParameterValue<int>(fvar);
//	debugMsg(LOG_LEVEL_INIT, "[%g]%s: modulo=%u \n", t, this->getFullName().data(), modulo);

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void TcpACKSplit::dint(double t) {
	sigma=std::numeric_limits<double>::infinity(); //wait until next packet arrives;
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}

void TcpACKSplit::dext(Event x, double t) {
	auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
	packetBuffer.push(arrivedPacket);

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Arrived packet #%u FlowID=%s \n",t, this->getFullName().data(), arrivedPacket->getId(), arrivedPacket->flowId.value.data());
	sigma = 0;
}

Event TcpACKSplit::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	uint port = 1;

	// Get the port (from the TCP protocol)
	if(!packetToSend->hasProtocol(PROTOCOL_ID_TCP_IP)){
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: received packet #%u FlowID=%s without tcp protocol. Sending on port=%d. \n ", t, this->getFullName().data(), packetToSend->getId(), packetToSend->flowId.value.data(), port);
		return Event(packetToSend, port);
	}

	auto protocol = packetToSend->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: received packet #%u FlowID=%s failed to get TCP protocol. Sending on port=%d. \n ", t, this->getFullName().data(), packetToSend->getId(), packetToSend->flowId.value.data(), port);
		return Event(packetToSend, port);
	}

	if(std::static_pointer_cast<TcpIpProtocol>(protocol)->getACK() < 0){
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: received packet #%u FlowID=%s with invalid ACK. Sending on port=%d. \n ", t, this->getFullName().data(), packetToSend->getId(), packetToSend->flowId.value.data(), port);
		return Event(packetToSend, port);
	}

	port = 0;
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: forwarding packet #%u FlowID=%s on outport %u \n", t, this->getFullName().data(), packetToSend->getId(), packetToSend->flowId.value.data(), port);
	return Event(packetToSend, port);
}

