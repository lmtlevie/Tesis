#include "hybridizedDemultiplex.h"

void hybridizedDemultiplex::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	this->sigma = std::numeric_limits<double>::infinity();
}


void hybridizedDemultiplex::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->pendingFlows.empty()){
		this->sigma =0; // program transition immediately
	}
}

void hybridizedDemultiplex::dext(Event x, double t) {
	switch (x.port) {
		case 0: {// Flow arrived
			auto arrivedFlow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

			this->pendingFlows.push_back(arrivedFlow);
			this->sigma = 0; // program transition immediately

			break;
		}
		default:
			throw std::runtime_error("hybridizedDemultiplex::dext: wrong port /n");
			break;
	}
	return;
}

Event hybridizedDemultiplex::lambda(double t) {
	auto flow = this->pendingFlows.front();
	this->pendingFlows.pop_front();

    if (flow->flowId.value.compare(Packet2hybridFlow::HYBRIDIZED_FLOWS_ID) == 0) {
        return Event(flow, 0);
    }

	return Event(flow, 1);
}
