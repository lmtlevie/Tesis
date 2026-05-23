#include "FlowDropsSelector.h"

void FlowDropsSelector::init(double t,...) {
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


void FlowDropsSelector::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
}

void FlowDropsSelector::dext(Event x, double t) {
	this->flow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowDropsSelector::dext:   flowID=%s has arrived  \n", t, this->getFullName().data(), flow->flowId.value.data());

	//sigma is infinite, no need to: this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
	if(this->flow->acumDrops.updated){
		this->drops = this->flow->acumDrops.value;
	    this->sigma = 0;
	    debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowDropsSelector::dext: flowID=%s  updated drops=[%f][%f][%f] \n", t, this->getFullName().data(), flow->flowId.value.data(), this->drops[0], this->drops[1], this->drops[2]);
	}

	return;
}

Event FlowDropsSelector::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowDropsSelector::lambda:   sending drops=[%f][%f][%f] \n", t, this->getFullName().data(), this->drops[0], this->drops[1], this->drops[2]);
	return Event(this->drops,0);
}
