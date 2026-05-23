#include "FluidFlowDynamicDeMutiplexer.h"

void FluidFlowDynamicDeMutiplexer::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char* fvar= va_arg(parameters,char*);
	this->maxFlows=readDefaultParameterValue<int>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: maxFlows = %i  \n", this->getFullName().data(), this->maxFlows);


	this->sigma = std::numeric_limits<double>::infinity();
}


void FluidFlowDynamicDeMutiplexer::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->pendingFlows.empty()){
		this->sigma =0; // program transition immediately
	}
}

void FluidFlowDynamicDeMutiplexer::dext(Event x, double t) {
	switch (x.port) {
		case 0: {// Flow arrived
			auto arrivedFlow = castEventPointer<IFlow>(x); // get the flow from the incoming event

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowDynamicDeMutiplexer::dext:   flowID=%s arrived through input port=%i  \n", t, this->getFullName().data(), arrivedFlow->flowId.value.data(), x.port);

			this->pendingFlows.push_back(arrivedFlow);
			this->sigma = 0; // program transition immediately

			break;
		}
		default:
			throw std::runtime_error("FluidFlowDynamicDeMutiplexer::dext: wrong port /n");
			break;
	}
	return;
}

Event FluidFlowDynamicDeMutiplexer::lambda(double t) {
	// pop next flow to output
	auto flow = this->pendingFlows.front();
	this->pendingFlows.pop_front();

	// determine port
	int port = -1;
	auto it = this->flowId2port.find(flow->flowId.value);
	if(it != this->flowId2port.end())	{ // flowId found;
		port = it->second;
	} else { // flowId not seen before
		if(this->flowId2port.size() < this->maxFlows){
			port = this->flowId2port.size();
			this->flowId2port[flow->flowId.value] = port ; // new flowId<-> port mapping
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowDynamicDeMutiplexer::lambda: new mapping  flowID=%s will always be sent through output port=%i  \n", t, this->getFullName().data(), flow->flowId.value.data(), port);
		} else{
			throw std::runtime_error(this->getFullName() + "::FluidFlowDynamicDeMutiplexer::lambda: a new flowID arrived but maximum output flow ports exceeded: " + std::to_string(this->maxFlows) + " \n");
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s::FluidFlowDynamicDeMutiplexer::lambda:  a new flowID=%s arrived but maximum output flow ports=%i exceeded.  \n", t, this->getFullName().data(), flow->flowId.value.data(), this->maxFlows);
		}
	}

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowDynamicDeMutiplexer::lambda:   flowID=%s sent through output port=%i  \n", t, this->getFullName().data(), flow->flowId.value.data(), port);

	return Event(flow, port);
}
