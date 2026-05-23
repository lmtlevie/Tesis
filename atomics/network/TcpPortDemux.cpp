#include "TcpPortDemux.h"

void TcpPortDemux::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char* fvar;
	fvar = va_arg(parameters,char*);
	this->modulo = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g]%s: modulo=%u \n", t, this->getFullName().data(), modulo);

	fvar = va_arg(parameters,char*);
	this->divisor = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g]%s: divisor=%u \n", t, this->getFullName().data(), divisor);


	fvar = va_arg(parameters,char*);
	this->use_dst= readDefaultParameterValue<bool>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g]%s: use_dst=%d \n", t, this->getFullName().data(), use_dst);

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void TcpPortDemux::dint(double t) {
	sigma=INF;
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}

void TcpPortDemux::dext(Event x, double t) {
	auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
	packetBuffer.push(arrivedPacket);

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_ext: Packet arrived with FlowID=%s \n",t, this->getFullName().data(), arrivedPacket->flowId.value.data());
	sigma = 0;
}

Event TcpPortDemux::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	// Get the port (from the TCP protocol)
	auto protocol = packetToSend->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without tcp protocol. discarding. \n ", t, this->getFullName().data(), packetToSend->getId());
		packetToSend->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("TcpPortDemux::lambda: received packet without tcp protocol.");
	}
	int port;
	if (this->use_dst){
		port = std::static_pointer_cast<TcpIpProtocol>(protocol)->getPortDst();
	} else {
		port = std::static_pointer_cast<TcpIpProtocol>(protocol)->getPortSrc();
	}
	port = floor(port/this->divisor) ; // calculate
	port = port % this->modulo;

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: forwarding packet %u(%s) by outport %u \n", t, this->getFullName().data(), packetToSend->getId(), packetToSend->flowId.value.data(), port);
	return Event(packetToSend, port);
}

