#include "Packet2hybridFlow.h"

void Packet2hybridFlow::init(double t,...) {
    BaseSimulator::init(t);
    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters,t);

    char* fvar= va_arg(parameters,char*);
    this->inv_bandwidth_bits_s = readDefaultParameterValue<double>(fvar );
    debugMsg(LOG_LEVEL_INIT, "[%g] %s: capacity: %g (bits/s) \n", t, this->getFullName().data(), this->inv_bandwidth_bits_s);
    this->departureRate[0] =  this->inv_bandwidth_bits_s; // the sent amplitude is always the capacity

    this->inv_bandwidth_bits_s = 1/this->inv_bandwidth_bits_s; // calculate the division once so that it is a multiplication the rest of the times

    this->logger->initSignals(std::vector<std::string>{"serviceTime", "packetSize_bits", "departureRate", "queueSize"});

    // initialize state variables
    //	departureRate[10] = {0}; // done at variable creation (in the .h)
    //	cero[10] = {0}; // done at variable creation (in the .h)

    this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting
    this->sendState = WAITING;

    return;
}

void Packet2hybridFlow::dint(double t) {
    switch (this->sendState) {
    case SEND_INPUT_RATE:{ // we just sent the rate for a packet
        this->sendState = SEND_INPUT_RATE_RESET;
        this->sigma = this->calculateCurrentPacketBandwidthDelay(); // send the packet after the serviceTime
        this->logger->logSignal(t, this->sigma, "serviceTime");
        break;
    }
    case SEND_INPUT_RATE_RESET: { // we just sent the reset, send next packet or wait
        this->packetQueue.pop_front(); // remove the packet as we finished sending it

        // set sigma
        if(this->packetQueue.size() > 0){ // send next packet & rate
            this->sigma = 0; // send immediately the rate without reset
            this->sendState = SEND_INPUT_RATE;
        } else { // nothing to send yet, just wait
            this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting
            this->sendState = WAITING;
        }

        break;
    }
    default:
        debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2HybridFlow::dint unknown state %i", t, this->getFullName().data(), this->sendState);
        throw std::runtime_error("Packet2HybridFlow::dint unknown state \n");
        break;
    }
}

void Packet2hybridFlow::dext(Event x, double t) {
    if (x.port==0) {   // A new packet enters
        // queue the packet
        auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
        this->packetQueue.push_back(packet);

        if(this->packetQueue.size() == 1){ // the only packet in the queue
            this->sigma = 0; // send immediately
            this->sendState = SEND_INPUT_RATE;
        } else { // we are sending other packets
            this->sigma = std::max(0.0, this->sigma - e); // continue as before
        }

        this->logger->logSignal(t, packet->getLength_bits(), "packetSize_bits");
        this->logger->logSignal(t, this->packetQueue.size(), "queueSize");
    } else  {
        debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2HybridFlow::dext unknown port %i \n ", t, this->getFullName().data(), x.port);
        throw std::runtime_error("Packet2HybridFlow::dext unknown port \n");
    }

    return;
}

Event Packet2hybridFlow::lambda(double t) {
    switch (this->sendState) {
    case SEND_INPUT_RATE:{
        // construct the hybridFlow with the packet and fluid-flow
        auto hybrid = this->createHybridFlow(this->departureRate);
        hybrid->packet.updated = true; // packet is forwarded, mark it as updated
        // WARNING: this is only QSS1 because departureRate is now always constant. Be careful if updated to other techniques with higher order QSS
        if(!this->lastSentFlow || this->lastSentFlow->fluid_flow.value->rate.value[0] != this->departureRate[0]){ // update QSS signal only if needed
            hybrid->fluid_flow.updated = true;
            hybrid->fluid_flow.value->rate.updated = true; // fluid rate is the attribute that was updated
        }
//        hybrid->fluid_flow.updated = true; // TODO: tmp to be removed

        if(!this->lastSentFlow ){ // send an initial event to propagate flowID
            hybrid->flowId.updated= true;
            hybrid->fluid_flow.value->flowId.updated = true;
            hybrid->packet.value->flowId.updated = true;
        }

        this->logger->logSignal(t, hybrid->fluid_flow.value->rate.value[0], "departureRate");

        this->lastSentFlow = hybrid;
        return Event(hybrid, 0);
        break;
    }
    case SEND_INPUT_RATE_RESET:{
        if(this->packetQueue.size() > 1){ // if there are more packets to send, we don't reset (send nothing)
            return Event();
        }

        // construct the hybridFlow with the packet and fluid-flow
        auto hybrid = this->createHybridFlow(this->cero);
        hybrid->fluid_flow.updated = true; // if we reach here is because we must update the QSS signal
        hybrid->fluid_flow.value->rate.updated = true; // fluid rate is the attribute that was updated
        hybrid->packet.updated = false; // packet was already forwarded in the SEND_INPUT_RATE, mark it as NOT updated

        this->logger->logSignal(t, hybrid->fluid_flow.value->rate.value[0], "departureRate");

        this->lastSentFlow = hybrid;
        return Event(hybrid, 0);
        break;
    }
    default:
        debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Packet2HybridFlow::lambda unknown state %i \n ", t, this->getFullName().data(), this->sendState);
        throw std::runtime_error("Packet2HybridFlow::lambda unknown state \n");
        break;
    }
}

double Packet2hybridFlow::calculateCurrentPacketBandwidthDelay(){
    auto packet = this->packetQueue.front();
    double delay = packet->getLength_bits() * this->inv_bandwidth_bits_s;

    return delay;
}

/*
 * Constructs the hybridFlow based on  the current packet
 */
std::shared_ptr<hybridFlow> Packet2hybridFlow::createHybridFlow(double* fluidRate){
    // construct the hybridFlow with the packet and fluid-flow
    auto packet = this->packetQueue.front();

    auto fluidId = HYBRIDIZED_FLOWS_ID; // use a specific ID to create this type of fluid-flow. It need to match with the demultiplexer inside hybrid queues. TODO: this will not work for multiple packet flows with different IDs
//    auto fluidId = packet->flowId.value;
    auto fluid = std::make_shared<FluidFlow>(fluidId, fluidRate, this->dropsRate, this->delay);

   return std::make_shared<hybridFlow>(packet, fluid);

}
