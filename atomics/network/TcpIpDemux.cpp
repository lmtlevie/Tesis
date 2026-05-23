#include "TcpIpDemux.h"

void TcpIpDemux::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	// TODO: using 4 is too rigid. Make the first param to be n and then iterate n times reading args
	for (int i=0;i<4;i++) {
		ipLabels[i] = va_arg(parameters,char*);
		debugMsg(LOG_LEVEL_INIT, "[INIT] %s: Label %u = %s \n", this->getName(), i, ipLabels[i].data());
	};

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void TcpIpDemux::dint(double t) {
	sigma=INF;
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}

void TcpIpDemux::dext(Event x, double t) {
	auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
	packetBuffer.push(arrivedPacket);

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_ext: Packet arrived with FlowID=%s \n",t, this->getFullName().data(), arrivedPacket->flowId.value.data());
	sigma = 0;
}

Event TcpIpDemux::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	// Get the IP (from the TCP protocol)
	auto protocol = packetToSend->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without tcp protocol. discarding. \n ", t, this->getFullName().data(), packetToSend->getId());
		packetToSend->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("TcpIpDemux::lambda: received packet without tcp protocol.");
	}
	std::string ip = std::static_pointer_cast<TcpIpProtocol>(protocol)->getIpSrc();

	for (int i=0;i<N;i++) {
		if (ipLabels[i].compare("*") == 0 || ipLabels[i].compare(ip) == 0) {
			debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: forwarding packet %u(%s) with IP='%s' by outport %u (matched label: %s) \n", t, this->getName(), packetToSend->getId(), packetToSend->flowId.value.data(), ip.data(),  i, ipLabels[i].data());
			return Event(packetToSend,i);
		}
	}

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: packet %u(%s) with IP='%s'  did not match any label, discarding.  \n", t, this->getName(), packetToSend->getId(), packetToSend->flowId.value.data(), ip.data());
	return Event(NULL,0);
}

