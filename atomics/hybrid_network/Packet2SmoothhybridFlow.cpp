#include "Packet2SmoothhybridFlow.h"

void Packet2SmoothhybridFlow::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char* fvar= va_arg(parameters,char*);

	// Parameter: Flow ID
	this->flowId = std::string(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: flowId: %s \n", t, this->getFullName().data(), this->flowId.data());

	// Parameter: Bandwidth
	fvar= va_arg(parameters,char*);
	this->inv_bandwidth_bits_s = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: capacity: %g (bits/s) \n", t, this->getFullName().data(), this->inv_bandwidth_bits_s);
	this->departureRate[0] =  this->inv_bandwidth_bits_s; // the sent amplitude is always the capacity
	this->inv_bandwidth_bits_s = 1/this->inv_bandwidth_bits_s; // calculate the division once so that it is a multiplication the rest of the times

	// Parameter: smoothingWindow
	fvar= va_arg(parameters,char*);
	this->smoothingWindow = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: smoothingWindow: %g (s) \n", t, this->getFullName().data(), this->smoothingWindow);
	if(this->smoothingWindow <= 0) {
	    throw std::runtime_error("Packet2SmoothhybridFlow::init smoothingWindow can not be <=0. If you want no smoothing use the Packet2hybridFlow.h model. \n");
	}

	// Parameter: rateDeltaMin
	fvar= va_arg(parameters,char*);
	this->rateDeltaMin = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[%g] %s: rateDeltaMin: %g (bits/s) \n", t, this->getFullName().data(), this->rateDeltaMin);

	this->logger->initSignals(std::vector<std::string>{"serviceTime", "packetSize_bits", "departureRate", "queueSize"});

	// initialize state variables
	//	departureRate[10] = {0}; // done at variable creation (in the .h)
	//	cero[10] = {0}; // done at variable creation (in the .h)

	this->lastSentRate =  std::numeric_limits<double>::infinity();  // set to infinite so that the first calculation get a big delta difference
	this->windowSum_bits=0;
	this->sigmaForNextRate = this->smoothingWindow;
	this->sigmaForNextPkt = std::numeric_limits<double>::infinity();  // Goes waiting
	this->sigma = 0;  // Send initial rate 0
	this->sendState = SEND_RATE;

	return;
}

void Packet2SmoothhybridFlow::dint(double t) {
	this->updateSigmas(t);
	switch (this->sendState) {
	case SEND_PACKET:{ // we just sent a packet
	    auto packet = this->packetQueue.front();
	    this->packetQueue.pop_front(); // remove the packet as we finished sending it

	    this->sigmaForNextPkt = this->calculateCurrentPacketBandwidthDelay(); // send the packet after the serviceTime
		this->logger->logSignal(t, this->sigmaForNextPkt, "serviceTime");
		break;
	}
	case SEND_RATE: { // we just sent the a rate
		this->sigmaForNextRate = this->smoothingWindow;
		this->windowSum_bits = 0;

		break;
	}
	default:
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2SmoothhybridFlow::dint unknown state %i", t, this->getFullName().data(), this->sendState);
		throw std::runtime_error("Packet2SmoothhybridFlow::dint unknown state \n");
		break;
	}

	this->setSigma();
}

void Packet2SmoothhybridFlow::dext(Event x, double t) {
	this->updateSigmas(t);
	if (x.port==0) {   // A new packet enters
		// queue the packet
		auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
		this->packetQueue.push_back(packet);

		// add packets to the avg when already sent only, not when they arrive
		//		this->windowSum_bits += packet->getLength_bits();


		if(this->packetQueue.size() == 1 ){ // the only packet in the queue and not waiting
		    //this->sigmaForNextPkt = 0;                                                                            // send immediately bandwidth delay (i.e WITH_FIRST_BIT)
			this->sigmaForNextPkt = this->calculateCurrentPacketBandwidthDelay(); // send AFTER bandwidth delay (i.e WITH_LAST_BIT)
			this->logger->logSignal(t, this->sigmaForNextPkt, "serviceTime");
		}

		this->logger->logSignal(t, packet->getLength_bits(), "packetSize_bits");
		this->logger->logSignal(t, this->packetQueue.size(), "queueSize");
	} else  {
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2SmoothhybridFlow::dext unknown port %i \n ", t, this->getFullName().data(), x.port);
		throw std::runtime_error("Packet2SmoothhybridFlow::dext unknown port \n");
	}

	this->setSigma();
	return;
}

Event Packet2SmoothhybridFlow::lambda(double t) {
	switch (this->sendState) {
	case SEND_PACKET:{
		// construct an hybridFlow with only the packet
		auto hybrid = this->createPacketHybridFlow();
		hybrid->packet.updated = true; // packet is forwarded, mark it as updated
		hybrid->fluid_flow.updated = false; // do not send fluid rate

		// add the packet we are sending to the avg
		this->windowSum_bits += hybrid->packet.value->getLength_bits();
		// TODO: there might be a chance here to detect if we already passes the deltaMin and send the rate before the window elapses. This would help accuracy (in case of bursts), but might be worst performance

//		this->logger->logSignal(t, hybrid->fluid_flow.value->rate.value[0], "departureRate");

		return Event(hybrid, 0);
		break;
	}
	case SEND_RATE:{
	    double avgRate = this->calculateWindowAverageRate();
	    if(std::abs(avgRate - this->lastSentRate) <= this->rateDeltaMin){ // if the averaged rate is not different enough, do not send any update
	        return Event();
	    }

		// construct an hybridFlow with only the averaged rate
		auto hybrid = this->createFluidHybridFlow(avgRate);

		if(t==0){ // send an initial event to propagate flowID
            hybrid->flowId.updated= true;
            hybrid->fluid_flow.value->flowId.updated = true;
        }
		hybrid->fluid_flow.updated = true;
		hybrid->fluid_flow.value->rate.updated = true; // fluid rate is the attribute that was updated
		hybrid->packet.updated = false; // / do not send a packet

		this->lastSentRate = hybrid->fluid_flow.value->rate.value[0];
		this->logger->logSignal(t, this->lastSentRate, "departureRate");

		return Event(hybrid, 0);
		break;
	}
	default:
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2SmoothhybridFlow::lambda unknown state %i \n ", t, this->getFullName().data(), this->sendState);
		throw std::runtime_error("Packet2SmoothhybridFlow::lambda unknown state \n");
		break;
	}
}

double Packet2SmoothhybridFlow::calculateWindowAverageRate(){
    return this->windowSum_bits / this->smoothingWindow; // bit per second
}

double Packet2SmoothhybridFlow::calculateCurrentPacketBandwidthDelay(){
	if(this->packetQueue.empty()){
		return std::numeric_limits<double>::infinity();
	}

	auto packet = this->packetQueue.front();
	return packet->getLength_bits() * this->inv_bandwidth_bits_s;
}

/*
 * Constructs the hybridFlow based on  the current packet
 */
std::shared_ptr<hybridFlow> Packet2SmoothhybridFlow::createFluidHybridFlow(double avgRate){
	auto fluidId = HYBRIDIZED_FLOWS_ID; // use a specific ID to create this type of fluid-flow. It need to match with the demultiplexer inside hybrid queues. TODO: this will not work for multiple packet flows with different IDs
	//    auto fluidId = packet->flowId.value;

	this->departureRate[0]=avgRate; // TODO: this is only QSS1. In the future more coefficients could be calculated
	auto fluid = std::make_shared<FluidFlow>(fluidId, this->departureRate, this->dropsRate, this->delay);

	auto hybrid = std::make_shared<hybridFlow>(fluid);
	hybrid->flowId.value = this->flowId; // set the hybrid flow according to the packet-level ids. The hybridized fluid flow has a different special ID to distiguish it

	return hybrid ;
}

/*
 * Constructs the hybridFlow based on  the current packet
 */
std::shared_ptr<hybridFlow> Packet2SmoothhybridFlow::createPacketHybridFlow(){
	// construct the hybridFlow with the packet and fluid-flow
	auto packet = this->packetQueue.front();

	// validate FlowId
	if(this->flowId != packet->flowId.value){
	    debugMsg(LOG_LEVEL_ERROR, "[??] %s: Packet2SmoothhybridFlow::createPacketHybridFlow::lambda model configured with flowId='%s' but a packet arrived with flowId='%s' \n ", this->getFullName().data(), this->flowId.data(), packet->flowId.value.data());
	    throw std::runtime_error("Packet2SmoothhybridFlow::createPacketHybridFlow::lambda model configured with flowId='"+this->flowId+"' but a packet arrived with flowId='"+ packet->flowId.value+ "' \n. See log for details \n");
	}

	return std::make_shared<hybridFlow>(packet);
}

/**
 *  update the internal sigmas after every transition
 */
void Packet2SmoothhybridFlow::updateSigmas(double t){

	this->sigmaForNextPkt -= this->e;
	this->sigmaForNextRate -= this->e;
}

/**
 *  Sets sigma according to the internal sigmas
 */
void Packet2SmoothhybridFlow::setSigma(){
	if(this->sigmaForNextPkt < this->sigmaForNextRate){
		this->sigma =  std::max(0.0, this->sigmaForNextPkt);
		this->sendState = SEND_PACKET;
	} else {
		this->sigma =  std::max(0.0, this->sigmaForNextRate);
		this->sendState = SEND_RATE;
	}
}
