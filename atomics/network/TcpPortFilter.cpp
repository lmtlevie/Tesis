#include "TcpPortFilter.h"

void TcpPortFilter::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	// start forwarding everything
	for (int i=0; i<10; i++) { // copy delay function (QSS values)
		this->maxPort[i] = std::numeric_limits<double>::infinity();
	}

	// latency: latency since creation: t - birthtime
	this->logger->initSignals(std::map<std::string, int>{
		{"maxPort", LOG_LEVEL_IMPORTANT}
	});

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void TcpPortFilter::dint(double t) {
	advance_time(this->maxPort, e, -1); // advance time for the delay function to match current time (continuous QSS function)

	sigma=std::numeric_limits<double>::infinity();
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}

void TcpPortFilter::dext(Event x, double t) {

	switch (x.port) {
	case 0: {// packet arrive, filter or forward
		advance_time(this->maxPort, e, -1); // advance time for the delay function to match current time (continuous QSS function)

		auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
		debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_ext: Packet arrived with FlowID=%s \n",t, this->getName(), arrivedPacket->flowId.value.data());

		// Get the port (from the TCP protocol)
		auto protocol = arrivedPacket->getProtocol(PROTOCOL_ID_TCP_IP);
		if(!protocol){
			debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without tcp protocol. discarding. \n ", t, this->getName(), arrivedPacket->getId());
			arrivedPacket->printInfo(LOG_LEVEL_ERROR);
			throw std::runtime_error("TcpPortFilter::lambda: received packet without tcp protocol.");
		}
		int port = std::static_pointer_cast<TcpIpProtocol>(protocol)->getPortDst();

		// filter or forward
		this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
		if(port < this->maxPort[0]){
			sigma = 0; // forward
			packetBuffer.push(arrivedPacket);
		} else {
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: rejected packet #%u with tcp dstPort=%d (maxPort=%g).\n ", t, this->getName(), arrivedPacket->getId(), port, this->maxPort[0]);
		}

		break;
	} case 1: { // inport1: new maxPort
		double* newValue = (double*)x.value;
		for (int i=0; i<10; i++) { // copy delay function (QSS values)
			this->maxPort[i] = newValue[i];
		}

		this->sigma = std::max(0.0, this->sigma - this->e); // continue as before

		this->logger->logSignal(t, this->maxPort[0], "maxPort");
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: New maxPort received. [0]=%f ; \n", t, this->getFullName().data(), this->maxPort[0] );
		break;
	}
	default:
		throw std::runtime_error("TcpPortFilter::dext: wrong port /n");
		break;
	}

}

Event TcpPortFilter::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: forwarding packet %u(%s) by outport  \n", t, this->getName(), packetToSend->getId(), packetToSend->flowId.value.data());
	return Event(packetToSend, 0);
}

