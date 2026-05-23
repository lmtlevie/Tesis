#include "hybridApplyQDelay.h"
void hybridApplyQDelay::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

//	char* fvar;
//	fvar=va_arg(parameters,char*);
//	this->delay_s[0] = (double)getScilabVar(fvar);
//	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: Delay: %g sec \n",t, this->getFullName().data(), this->delay_s[0]);

	sigma = std::numeric_limits<double>::infinity();

	this->logger->initSignals({"arrived", "departed", "queuesize", "appliedDelay", "sigma", "wrongDelayFixed"});

	return;
}

void hybridApplyQDelay::dint(double t) {
	// remove the packet we just sent
	this->dequeuePacket();

	this->logger->logSignal(t, this->packetOutputTimes.size(), "queuesize");

	// set new sigma
	this->sigma = std::numeric_limits<double>::infinity();
	if(this->packetOutputTimes.size() > 0){
		this->sigma = this->getNextOutputTime() - t;

		// t+delay should never yield a negative value.  In practice, for some unknown reason it does.
		if(this->sigma < 0){
			this->sigma = 0; // TODO: this will inmediatley send next packet without proccessing. Most surely this is not right as will generate next link to queue the packets (maybe fixing the problem but generating extra delay)
			//this->sigma = this->getNextOutputPacket()->hybridQDelay; // TODO: this sends next packet after an additional qDelay. This is not feasible as it will yield much more delay stopping all forwarding.
			this->logger->logSignal(t, this->wrongDelayFixed, "wrongDelayFixed"); // so we track the anomalies
		}
	}

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint]: next transition programmed in %g sec\n", t, this->getFullName().data(), sigma);
}

void hybridApplyQDelay::dext(Event x, double t) {
	switch (x.port) {
	case 0: { // inport0: packet
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
		double outTime = t + packet->hybridQDelay; // set the outtime of the packet using the delay

		// TODO: output times are kept SORTED, which should not be necessary
		this->enqueuePacket(packet, outTime);

		this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
		if(this->packetOutputTimes.size() == 1){ // if there were  no previous packets
			sigma = packet->hybridQDelay ; // should be the same as this->packetOutputTimes.front().first - t

			this->logger->logSignal(t, this->sigma, "sigma");
		}

		this->logger->logSignal(t, this->packetOutputTimes.size(), "queuesize");
		this->logger->logSignal(t, packet->getId(), "arrived");
		this->logger->logSignal(t, packet->hybridQDelay, "appliedDelay");

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Packet #%u programmed to be send at  %g sec (delay=%g)\n",t, this->getFullName().data(), packet->getId(), outTime, packet->hybridQDelay);

		break;
	}
	default: {
		throw std::runtime_error("hybridApplyQDelay::dext: wrong port /n");
		break;
	}
	}

	return ;
}

Event hybridApplyQDelay::lambda(double t) {
	auto packet = this->getNextOutputPacket();
	packet->hybridQDelay = -1; // now that we already used it, set it back to 0 (TODO: find a better place to put this hybridQDelay (not in the packet))

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Sending Packet #%u. queueSize=%d\n",t, this->getFullName().data(),packet->getId(), this->packetOutputTimes.size());
	this->logger->logSignal(t, packet->getId(), "departed");
	return Event(packet,0);
}

void hybridApplyQDelay::enqueuePacket(std::shared_ptr<NetworkPacket> packet , double outTime){
	// in some corner case experiments this sets ta<0 (see dint).
	// i.e.: With this implementation packets are kept in order, but in some cases the delay is not respected
	this->packetOutputTimes.push_back({outTime, packet});

	// TODO: Another options would be to respect the delay, but disorder packets:
	// output times are kept SORTED, which should not be necessary
	// (i.e. they arrive in order and the qDelay is calculated based on their arrival and size which should not disorder them)
	// Nevertheless, in practice, for some unknown reason they do get disordered.
	// (i.e. with this implementation the queue might in some occasions output packets in a different order than they arrived, but qDelay is respected)

	// Instead we use a vector and keep it sorted (instead of just a dequeue) :-(
	// insert sorted, find the first one less outTime (in reverse order because it will probably yield better performance)
//	std::pair<double, Packet*> pair = {outTime, p};
//	auto firstSmallerIt = this->packetOutputTimes.rbegin();
//	for (; firstSmallerIt != this->packetOutputTimes.rend(); ++firstSmallerIt ) {
//		if (pair.first > (*firstSmallerIt).first) break;
//	}
//
//	if(firstSmallerIt != this->packetOutputTimes.rbegin()){
//		this->wrongDelayFixed++;
//		this->logger->logSignal(outTime - p->hybridQDelay, this->wrongDelayFixed, "wrongDelayFixed");
//	}
//	this->packetOutputTimes.insert(firstSmallerIt.base(), pair);
}

void hybridApplyQDelay::dequeuePacket( ){
	this->packetOutputTimes.pop_front();
//	this->packetOutputTimes.erase(this->packetOutputTimes.begin());
}

double hybridApplyQDelay::getNextOutputTime(){
	return this->packetOutputTimes.front().first;
//	return (*this->packetOutputTimes.begin()).first;
}

std::shared_ptr<NetworkPacket> hybridApplyQDelay::getNextOutputPacket(){
	return this->packetOutputTimes.front().second;
//	return (*this->packetOutputTimes.begin()).second;
}




