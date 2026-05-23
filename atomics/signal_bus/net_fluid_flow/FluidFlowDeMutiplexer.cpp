#include "FluidFlowDeMutiplexer.h"

void FluidFlowDeMutiplexer::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);


	for (int i=0;i<MAX_FLOW_IDS;i++) {
		flowIds[i] = va_arg(parameters,char*);
		debugMsg(LOG_LEVEL_INIT, "[INIT] %s: Label %u = %s \n", this->getName(), i, flowIds[i].data());
	}

	this->sigma = std::numeric_limits<double>::infinity();
}


void FluidFlowDeMutiplexer::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->pendingFlows.empty()){
		this->sigma =0; // program transition immediately
	}
}

void FluidFlowDeMutiplexer::dext(Event x, double t) {
	switch (x.port) {
		case 0: {// Flow arrived
			auto arrivedFlow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

			this->pendingFlows.push_back(arrivedFlow);
			this->sigma = 0; // program transition immediately

			break;
		}
		default:
			throw std::runtime_error("FluidFlowDeMutiplexer::dext: wrong port /n");
			break;
	}
	return;
}

Event FluidFlowDeMutiplexer::lambda(double t) {
	auto flow = this->pendingFlows.front();
	this->pendingFlows.pop_front();

	for (int i=0;i< MAX_FLOW_IDS;i++) {
		if (flowIds[i].compare("*") == 0 || flowIds[i].compare(flow->flowId.value) == 0) {
//			debugMsg(LOG_LEVEL_DEBUG, boost::format("[%f] %s_lamb: forwarding flow.ID='%s' by outport %u (matched label: %s) \n") % t % this->getFullName() % flow->id.value % i % flowIds[i], "");
			return Event(flow,i);
		}
	}

//	debugMsg(LOG_LEVEL_EXCEPTION, boost::format("[%f] %s_lamb: packet flow.ID='%s'  did not match any label.  \n") % t % this->getFullName() % flow->id.value, "");
	throw std::runtime_error("FluidFlowDeMutiplexer::dext: flow '" + flow->id.value + "' did not match any label /n");
	return Event();
}
