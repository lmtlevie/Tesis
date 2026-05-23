#include "hybridMergeFluidIntoPacket.h"
void hybridMergeFluidIntoPacket::init(double t,...) {
    BaseSimulator::init(t);
    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters,t);
    //To get a parameter: %Name% = va_arg(parameters,%Type%)
    //where:
    //  %Name% is the parameter name
    //	%Type% is the parameter type

    this->logger->initSignals(std::map<std::string, int>{
        {"discardProb", LOG_LEVEL_IMPORTANT},
        {"discard_bits", LOG_LEVEL_IMPORTANT},
        {"drops", LOG_LEVEL_PRIORITY},
        {"rate", LOG_LEVEL_PRIORITY},
        {"sent_bits", LOG_LEVEL_DEBUG},
        {"packetdrop", LOG_LEVEL_DEBUG},
        {"rndValue", LOG_LEVEL_DEBUG},
        {"appliedDelay", LOG_LEVEL_DEBUG}
    });


    this->sigma = std::numeric_limits<double>::max();  // wait for first packet to arrive

    return;
}


void hybridMergeFluidIntoPacket::dint(double t) {
    // advance time to match current time (continuous QSS function)
    advance_time(this->rate, e, -1);
    advance_time(this->drops, e, -1);
    advance_time(this->delay, e, -1);

    // a packet was just sent, pop it
    this->pendingPackets.pop();

    this->setSigma(t);
}

void hybridMergeFluidIntoPacket::dext(Event x, double t) {
    if (x.port==0) {   // A new packet arrived
        auto p = castEventPointer<NetworkPacket>(x); // get the pacekt from the incoming event

        // advance time to match current time (continuous QSS function)
        advance_time(this->rate, e, -1);
        advance_time(this->drops, e, -1);
        advance_time(this->delay, e, -1);

        // calculate discard probability based on the current rate and discards
        double discardProb = 0;
        if(this->rate[0] != 0){
            discardProb = this->drops[0] / this->rate[0];
        }
        this->logger->logSignal(t, discardProb,"discardProb");

        // get a random uniform number
        double randomValue = STDEVS::getConfigurationInstance()->uniform(); // [0, 1]
        this->logger->logSignal(t, randomValue, "rndValue");
        if(randomValue >= discardProb){ // packet accepted
            // schedule departe according to delay
            this->logger->logSignal(t, delay[0], "appliedDelay");
            this->pendingPackets.push({p, t + delay[0]});
            debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s: Packet ID #%u was accepted.(discardProb=%f, randomValue=%f). Will be forwarded at t=%f (delay=%f) \n", t, this->getFullName().data(), p->getId(), discardProb, randomValue, t + delay[0], delay[0]);
        } else { // packet rejected
            this->logger->logSignal(t, p->getLength_bits(),"discard_bits");
            debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s: Packet ID #%u was discarded.(discardProb=%f, randomValue=%f) \n", t, this->getFullName().data(), p->getId(), discardProb, randomValue);
        }
    }
    else if (x.port==1) { // fluid flow updated
        auto fluidFlow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

        // update the local signals based on the new flow update
        this->updateSignals(t, fluidFlow);
    }

    // set sigma
    setSigma(t);

    return;
}

Event hybridMergeFluidIntoPacket::lambda(double t) {
    // send next packet
    auto pair = this->pendingPackets.front();

    this->logger->logSignal(t, pair.first->getLength_bits(), "sent_bits");
    return Event(pair.first, 0);
}


void hybridMergeFluidIntoPacket::setSigma(double t) {
    if(this->pendingPackets.empty()) {
        this->sigma = std::numeric_limits<double>::max();  // wait for next packet to arrive
    } else { // if not empty schedule next packet at corresponding time
        auto pair = this->pendingPackets.front();
        this->sigma= std::max(0.0, pair.second - t);
    }
}

/*
 * Copies the updated signal and advances the other ones
 */
void hybridMergeFluidIntoPacket::updateSignals(double t, std::shared_ptr<FluidFlow> fluidFlow) {
    debugMsg(LOG_LEVEL_PRIORITY, "%s: fluid flow updated: \n",  this->getFullName().data());
    fluidFlow->printInfo(LOG_LEVEL_PRIORITY);

    // rate
    if (fluidFlow->rate.updated) {
        for (int i = 0; i < 10; i++) {
            this->rate[i] = fluidFlow->rate.value[i];
        }
        this->logger->logSignal(t, rate[0], "rate");
    } else {
        advance_time(this->rate, e, -1); // advance time match current time (continuous QSS function)
    }

    //drops
    if (fluidFlow->acumDrops.updated) {
        for (int i = 0; i < 10; i++) {
            this->drops[i] = fluidFlow->acumDrops.value[i];
        }
        this->logger->logSignal(t, drops[0], "drops");
    } else {
        advance_time(this->drops, e, -1); // advance time match current time (continuous QSS function)
    }

    //delay
    if (fluidFlow->acumDelay.updated) {
        for (int i = 0; i < 10; i++) {
            this->delay[i] = fluidFlow->acumDelay.value[i];
        }
        this->logger->logSignal(t, delay[0], "delay");
    } else {
        advance_time(this->delay, e, -1); // advance time match current time (continuous QSS function)
    }
}
