#include "hybridFlowForwarder.h"

void hybridFlowForwarder::init(double t,...) {
    BaseSimulator::init(t);

    va_list parameters;
    va_start(parameters,t);

    //	char *fvar= va_arg(parameters,char*);
    //	this->level= readDefaultParameterValue<double>(fvar );

    //  performed at variable creation (in the .h)
    //	for(int i=0;i<10;i++){u[i]=0;};
    //	for(int i=0;i<10;i++){y[i]=0;};

    this->sigma = std::numeric_limits<double>::infinity();
}


void hybridFlowForwarder::dint(double t) {
    this->sigma = std::numeric_limits<double>::infinity();
    if(this->packetQueue.size() != 0){
        sigma = 0;
    }
}

void hybridFlowForwarder::dext(Event x, double t) {
    auto flow = castEventPointer<hybridFlow>(x); // get the flow from the incoming event

    //sigma is infinite, no need to: this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
//    if(flow->packet.updated){
        this->packetQueue.push( flow); // enqueue packet so that we dont loose them if they arrive in the same t
        this->sigma = 0;
//    }

    return;
}

Event hybridFlowForwarder::lambda(double t) {
    auto hFlow = this->packetQueue.front();
    this->packetQueue.pop();

    return Event(hFlow,0);
}
