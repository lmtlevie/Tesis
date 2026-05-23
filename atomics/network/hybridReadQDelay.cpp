#include "hybridReadQDelay.h"
void hybridReadQDelay::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

//	char* fvar;
//	fvar=va_arg(parameters,char*);
//	this->delay_s[0] = (double)getScilabVar(fvar);
//	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: Delay: %g sec \n",t, this->getFullName().data(), this->delay_s[0]);

	sigma = std::numeric_limits<double>::infinity();

	this->logger->initSignals(std::vector<std::string>{"delay", "delaySet"});

	return;
}

void hybridReadQDelay::dint(double t) {
	advance_time(this->delay_s, e, -1);

	// de reference the packet we just sent
	this->packetToForward = nullptr;

	// set new sigma
	sigma = std::numeric_limits<double>::infinity();
}

void hybridReadQDelay::dext(Event x, double t) {
	switch (x.port) {
	case 0: { // inport0: packet
		advance_time(this->delay_s, e, -1); // advance time for the delay function to match current time (continuous QSS function)
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event

		// validate
		if(this->packetToForward != nullptr){
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s: A packet arrived when there was another which was not yet sent. Two packets in the same T? . \n", t, this->getFullName().data());
			throw std::runtime_error("hybridReadQDelay::dext: A packet arrived when there was another which was not yet sent. Two packets in the same T? . Aborting \n");
		}

		// set the delay to the packet
		this->packetToForward = packet;
		this->packetToForward->hybridQDelay = this->delay_s[0];

		this->sigma = 0; // program forwarding immediately

		this->logger->logSignal(t, this->packetToForward->hybridQDelay, "delaySet");
		this->logger->logSignal(t, this->delay_s[0], "delay");
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: Packet #%u has a qDelay of  %g sec\n",t, this->getFullName().data(), packet->getId(), this->packetToForward->hybridQDelay);
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
		throw std::runtime_error("hybridReadQDelay::dext: wrong port /n");
		break;
	}
	}

	return ;
}

Event hybridReadQDelay::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Sending Packet #%u with qDelay=%g\n", t, this->getFullName().data(), this->packetToForward->getId(), this->packetToForward->hybridQDelay);

	return Event(this->packetToForward, 0);
}

