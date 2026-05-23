#include "packetDelay.h"
void packetDelay::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char* fvar;

	fvar=va_arg(parameters,char*);
	this->delay_s[0] = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: Delay: %g sec \n", this->getFullName().data(), this->delay_s[0]);

	sigma = std::numeric_limits<double>::infinity();

	this->logger->initSignals(std::map<std::string,int>{
		{"arrived", LOG_LEVEL_IMPORTANT},
		{"departed", LOG_LEVEL_IMPORTANT},
		{"delay", LOG_LEVEL_IMPORTANT},
		{"queuesize", LOG_LEVEL_IMPORTANT},
		{"appliedDelay", LOG_LEVEL_IMPORTANT},
		{"sigma", LOG_LEVEL_DEBUG},
		{"sentPacket", LOG_LEVEL_DEBUG},
	});

	return;
}

void packetDelay::dint(double t) {
	advance_time(this->delay_s, e, -1);

	// remove the packet we just sent
	this->packetOutputTimes.pop_front();

	// set new sigma
	sigma = std::numeric_limits<double>::infinity();
	if(this->packetOutputTimes.size() > 0){
		sigma = this->packetOutputTimes.front().first - t;
		this->logger->logSignal(t, this->sigma, "sigma");
	}

	this->logger->logSignal(t, this->packetOutputTimes.size(), "queuesize");

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dint]: next transition programmed in %g sec\n", t, this->getFullName().data(), sigma);
}

void packetDelay::dext(Event x, double t) {
	switch (x.port) {
	case 0: { // inport0: packet
		advance_time(this->delay_s, e, -1); // advance time for the delay function to match current time (continuous QSS function)

		auto p = castEventPointer<NetworkPacket>(x); // get the job from the incoming event
		this->packetOutputTimes.push_back({t+this->delay_s[0], p});
		// TODO: these output times must be kept SORTED. Is there a chance that they change order? -> no
		arrived++;

		this->sigma = this->sigma - e; // continue as before
		if(this->packetOutputTimes.size() == 1){ // if there were  no previous packets
			sigma = this->packetOutputTimes.front().first - t; // should be the same as this->delay_s

			this->logger->logSignal(t, this->sigma, "sigma");
		}

		this->logger->logSignal(t, this->packetOutputTimes.size(), "queuesize");
		this->logger->logSignal(t, this->arrived, "arrived");
		this->logger->logSignal(t, this->delay_s[0], "delay");
		this->logger->logSignal(t, this->delay_s[0], "appliedDelay");

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Packet #%u programmed to be sent at t=%g. %d packets in queue, next packet will be sent at t=%g\n",t, this->getFullName().data(), p->getId(), t+this->delay_s[0], this->packetOutputTimes.size(), t+sigma);

		break;
	}
	case 1: { // inport1: new delay
		double* newValue = (double*)x.value;
		for (int i=0; i<10; i++) { // copy delay function (QSS values)
			this->delay_s[i] = newValue[i];
		}

		this->sigma = this->sigma - e; // continue as before

		this->logger->logSignal(t, this->delay_s[0], "delay");
		debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s: New delay received. [0]=%f ; \n", t, this->getFullName().data(), this->delay_s[0] );
		break;
	}
	default: {
		throw std::runtime_error("packetDelay::dext: wrong port /n");
		break;
	}
	}

	return ;
}

Event packetDelay::lambda(double t) {
	auto p = this->packetOutputTimes.front().second;
	departed++;

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Sending Packet #%u. queueSize=%d\n",t, this->getFullName().data(),p->getId(), this->packetOutputTimes.size());
	p->printInfo(LOG_LEVEL_DEBUG );

	this->logger->logSignal(t, this->departed, "departed");
	this->logger->logSignal(t, p->getId(), "sentPacket");

	return Event(p,0);
}

