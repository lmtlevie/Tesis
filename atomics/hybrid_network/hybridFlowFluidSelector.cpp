#include "hybridFlowFluidSelector.h"

void hybridFlowFluidSelector::init(double t,...) {
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


void hybridFlowFluidSelector::dint(double t) {
    this->sigma = std::numeric_limits<double>::infinity();
    if(this->fluidQueue.size() != 0){
        sigma = 0;
    }
}

void hybridFlowFluidSelector::dext(Event x, double t) {
    auto flow = castEventPointer<hybridFlow>(x); // get the flow from the incoming event

    //sigma is infinite, no need to: this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
    if(flow->fluid_flow.updated){
        this->fluidQueue.push(flow->fluid_flow.value);
        this->sigma = 0;
    }

    return;
}

Event hybridFlowFluidSelector::lambda(double t) {
    auto fluid_flow = this->fluidQueue.front();
    this->fluidQueue.pop();

    return Event(fluid_flow,0);
}
