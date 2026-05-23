#include "FluidFlow2hybridFlow.h"

void FluidFlow2hybridFlow::init(double t,...) {
    BaseSimulator::init(t);
    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters,t);

    //    char* fvar= va_arg(parameters,char*);

    this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting

    return;
}

void FluidFlow2hybridFlow::dint(double t) {
    this->sigma = std::numeric_limits<double>::infinity();
    if(this->fluidQueue.size() != 0){
        sigma = 0;
    }
}

void FluidFlow2hybridFlow::dext(Event x, double t) {
    auto flow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

    this->fluidQueue.push(flow);
    this->sigma = 0;

    return;
}

Event FluidFlow2hybridFlow::lambda(double t) {
    auto fluid_flow = this->fluidQueue.front();
    this->fluidQueue.pop();

    // construct the hybridFlow with only the fluid-flow
    auto hybrid = std::make_shared<hybridFlow>(fluid_flow);
    hybrid->packet.updated = false; // there is no packet for this flow
    hybrid->fluid_flow.updated = true;

    return Event(hybrid, 0);
}
