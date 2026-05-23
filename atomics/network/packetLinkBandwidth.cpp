#include "packetLinkBandwidth.h"
void packetLinkBandwidth::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char* fvar;

	fvar=va_arg(parameters,char*);
	bw_bit_s = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: Datarate: %g bit/s \n",t, this->getFullName().data(),bw_bit_s);

	this->sigma = std::numeric_limits<double>::infinity();

	this->logger->initSignals(std::map<std::string,int>{
		{"queuesize", LOG_LEVEL_IMPORTANT},
		{"serviceTime", LOG_LEVEL_IMPORTANT},
		{"sentPacket", LOG_LEVEL_DEBUG}
	});

	return;
}

void packetLinkBandwidth::dint(double t) {
	this->packetQueue.pop_front(); // dequeue the packet we just sent

	// set next transition
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->packetQueue.empty()){
		std::shared_ptr<NetworkPacket> nextPacket = this->packetQueue.front();
		this->sigma = this->getServiceTime(nextPacket);
		this->logger->logSignal(t, this->sigma, "serviceTime");
	}

	this->logger->logSignal(t, packetQueue.size(), "queuesize");

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint]: next transition programmed in %g sec. %d packets in queue.\n", t, this->getFullName().data(), this->packetQueue.size(), sigma);
}

void packetLinkBandwidth::dext(Event x, double t) {
	if (x.port==0) { // New packet arrived
		// queue it
		auto packet = castEventPointer<NetworkPacket>(x); // get the job from the incoming event
		this->packetQueue.push_back(packet);

		// set next transition
		this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
		if(this->packetQueue.size() == 1){ // if there were  no previous packets
			this->sigma = this->getServiceTime(packet); // send the packet that just arrived
			this->logger->logSignal(t, this->sigma, "serviceTime");
		}

		this->logger->logSignal(t, packetQueue.size(), "queuesize");

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Packet #%u enqueued. %d packets in queue, next packet will be sent at t=%g \n",t, this->getFullName().data(), packet->getId(),  this->packetQueue.size(), t+sigma);
	}

	return ;
}

Event packetLinkBandwidth::lambda(double t) {
	auto packet = this->packetQueue.front();
	this->logger->logSignal(t, packet->getId(), "sentPacket");

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Sending Packet #%u. queueSize=%d\n",t, this->getFullName().data(),packet->getId(), this->packetQueue.size());
	return Event(packet,0);
}

double packetLinkBandwidth::getServiceTime(std::shared_ptr<NetworkPacket> packet){
	return ((double)packet->getLength_bits())/this->bw_bit_s;
}
