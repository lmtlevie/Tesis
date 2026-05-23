#include "packetLogger.h"
void packetLogger::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	sigma = std::numeric_limits<double>::infinity();

	// latency: latency since creation: t - birthtime
	this->logger->initSignals(std::map<std::string, int>{
		{"size_bits", LOG_LEVEL_IMPORTANT},
		{"id", LOG_LEVEL_IMPORTANT},
		{"overhead_bits", LOG_LEVEL_PRIORITY},
		{"payload_bits",  LOG_LEVEL_PRIORITY}, // For some reason Scilab does not like more than 5 variables
		//{"latency", LOG_LEVEL_IMPORTANT},
		// TCP
		{"tcp_latency", LOG_LEVEL_IMPORTANT} // time since packet was sent from the TCP sender
	});


	return;
}

void packetLogger::dint(double t) {
	sigma = std::numeric_limits<double>::infinity();
}

void packetLogger::dext(Event x, double t) {
	if (x.port==0) { // Data
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
//		auto tcpProtocol = std::static_pointer_cast<TcpIpProtocol>(packet->getProtocol(PROTOCOL_ID_TCP_IP));

		this->logger->logSignal(t, ((double) packet->getLength_bits()), "size_bits");
		this->logger->logSignal(t, ((double) packet->getId()), "id");
		//this->logger->logSignal(t, ((double) tcpProtocol->getSize_bits()), "payload_bits");
		this->logger->logSignal(t, t - packet->getBirthTime(), "latency");

		// TCP
		if(packet->lastModifTime != -1){
			this->logger->logSignal(t, t - packet->lastModifTime, "tcp_latency"); // time since packet was sent from the TCP sender
		}

		// log a message every 10K packets
		if(packet->getId() % 10000 == 0){
			debugMsg(LOG_LEVEL_ALWAYS, "[%g] %s[dext]: Packet #%u logged \n",t, this->getFullName().data(), packet->getId());
		}

		if(packet->getId() % 100000000 == 0){// every 10G packets
			packet->printInfo(LOG_LEVEL_ALWAYS);
		}

	} else	{
		throw std::runtime_error("packetLogger::dext unknown port /n");
	}

	return ;
}

Event packetLogger::lambda(double t) {
	return Event();
}
