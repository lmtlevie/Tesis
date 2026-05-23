#include "hybridizationLink.h"
void hybridizationLink::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char* fvar= va_arg(parameters,char*);
	this->bandwidth_bits_s = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: capacity: %g (bits/s) \n", t, this->getFullName().data(), this->bandwidth_bits_s);

	fvar = va_arg(parameters,char*);
	if (strcmp(fvar,"WITH_FIRST_BIT")==0) {
		this->discretePacketSendMode = WITH_FIRST_BIT;
	} else if(strcmp(fvar,"WITH_LAST_BIT")==0) {
		this->discretePacketSendMode = WITH_LAST_BIT;
	} else {
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: The discretePacketSendMode is badly configured. Should be either WITH_FIRST_BIT or WITH_LAST_BIT. Received: %s \n", t, this->getFullName().data(), fvar);
		throw std::runtime_error("hybridizationLink::init: The discretePacketSendMode is badly configured. Should be either WITH_FIRST_BIT or WITH_LAST_BIT");
	}
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: discretePacketSendMode: %s  \n", t, this->getFullName().data(), fvar);


	this->logger->initSignals(std::vector<std::string>{"serviceTime", "packetSize_bits", "sentInputRate", "queueSize"});

	// initialize state variables
//	sentInputRate[10] = {0}; // done at variable creation (in the .h)
//	cero[10] = {0}; // done at variable creation (in the .h)
	this->sentInputRate[0] =  this->bandwidth_bits_s; // the sent amplitude is always the capacity
	this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting
	this->sendState = WAITING;

	//* Dynamic could be:
	// * 1) WITH_LAST_BIT: WAITING --> SEND_INPUT_RATE --serviceTime--> SEND_PACKET --0secs--> SEND_INPUT_RATE_RESET --0secs--> WAITING
	// * 2) WITH_FIRST_BIT: WAITING --> SEND_PACKET --0secs--> SEND_INPUT_RATE --serviceTime--> SEND_INPUT_RATE_RESET --0secs--> WAITING
	this->transitionTable = {
		std::make_pair(std::make_pair (WITH_LAST_BIT, WAITING), SEND_INPUT_RATE),
		std::make_pair(std::make_pair (WITH_LAST_BIT, SEND_INPUT_RATE), SEND_PACKET),
		std::make_pair(std::make_pair (WITH_LAST_BIT, SEND_PACKET), SEND_INPUT_RATE_RESET),
		std::make_pair(std::make_pair (WITH_LAST_BIT, SEND_INPUT_RATE_RESET), WAITING),

		std::make_pair(std::make_pair (WITH_FIRST_BIT, WAITING), SEND_PACKET),
		std::make_pair(std::make_pair (WITH_FIRST_BIT, SEND_PACKET), SEND_INPUT_RATE),
		std::make_pair(std::make_pair (WITH_FIRST_BIT, SEND_INPUT_RATE), SEND_INPUT_RATE_RESET),
		std::make_pair(std::make_pair (WITH_FIRST_BIT, SEND_INPUT_RATE_RESET), WAITING),
	  };

	return;
}

//* Dynamic could be:
// * 1) WITH_LAST_BIT: WAITING --> SEND_INPUT_RATE --serviceTime--> SEND_PACKET --0secs--> SEND_INPUT_RATE_RESET --0secs--> WAITING
// * 2) WITH_FIRST_BIT: WAITING --> SEND_PACKET --0secs--> SEND_INPUT_RATE --serviceTime--> SEND_INPUT_RATE_RESET --0secs--> WAITING

void hybridizationLink::dint(double t) {
	auto currentState = std::make_pair(this->discretePacketSendMode, this->sendState);
	auto nextState = this->transitionTable[currentState];
	switch (this->sendState) {
		case SEND_INPUT_RATE:{ // we just sent the rate for a packet
			this->sendState = nextState;
			this->sigma = this->calculateCurrentPacketBandwidthDelay(); // send the packet after the serviceTime
			this->logger->logSignal(t, this->sigma, "serviceTime");
			break;
		} case SEND_PACKET:{ // we just sent the packet
			this->sendState = nextState;
			this->sigma = 0; // send the reset immediately
			break;
		}	case SEND_INPUT_RATE_RESET: { // we just sent the reset, send next packet or wait
			this->packetQueue.pop_front(); // remove the packet as we finished sending it

			// set sigma
			if(this->packetQueue.size() > 0){ // send next packet rate
				this->sigma = 0; // send immediately the rate
				this->sendState = this->transitionTable[std::make_pair(this->discretePacketSendMode, WAITING)];
			} else { // nothing to send yet, just wait
				this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting
				this->sendState = nextState;
			}

			break;
		} default:
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s: hybridizationLink::dint unknown state %i", t, this->getFullName().data(), this->sendState);
			throw std::runtime_error("hybridizationLink::dint unknown state \n");
			break;
	}
}

void hybridizationLink::dext(Event x, double t) {
	if (x.port==0) {   // A new packet enters
		// queue the packet
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
		this->packetQueue.push_back(packet);

		if(this->packetQueue.size() == 1){ // the only packet in the queue
			this->sigma = 0; // send immediately
			this->sendState = this->transitionTable[std::make_pair(this->discretePacketSendMode, WAITING)];
		} else { // we are sending other packets
			this->sigma = this->sigma - e; // continue as before
		}

		this->logger->logSignal(t, packet->getLength_bits(), "packetSize_bits");
		this->logger->logSignal(t, this->packetQueue.size(), "queueSize");
	} else  {
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: hybridizationLink::dext unknown port %i \n ", t, this->getFullName().data(), x.port);
		throw std::runtime_error("hybridizationLink::dext unknown port \n");
	}

	return;
}

Event hybridizationLink::lambda(double t) {
	switch (this->sendState) {
		case SEND_INPUT_RATE:
			this->logger->logSignal(t, this->sentInputRate[0], "sentInputRate");
			return Event(this->sentInputRate, 1);
			break;
		case SEND_PACKET:{
			auto packet = this->packetQueue.front();
			return Event(packet, 0);
			break;
		}
		case SEND_INPUT_RATE_RESET:
			if(this->packetQueue.size() > 1){ // if there are more packets to send, we don't reset (send nothing)
				return Event();
			}

			this->logger->logSignal(t, this->cero[0], "sentInputRate");
			return Event(this->cero, 1);
			break;
		default:
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s: hybridizationLink::lambda unknown state %i \n ", t, this->getFullName().data(), this->sendState);
			throw std::runtime_error("hybridizationLink::lambda unknown state \n");
			break;
	}
}

double hybridizationLink::calculateCurrentPacketBandwidthDelay(){
	auto packet = this->packetQueue.front();
	double delay = packet->getLength_bits() / this->bandwidth_bits_s;

	return delay;
}
