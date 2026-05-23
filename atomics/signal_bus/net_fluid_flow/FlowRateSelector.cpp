#include "FlowRateSelector.h"

void FlowRateSelector::init(double t,...) {
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


void FlowRateSelector::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
}

void FlowRateSelector::dext(Event x, double t) {
	this->flow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowRateSelector::dext:   flowID=%s has arrived  \n", t, this->getFullName().data(), flow->flowId.value.data());

	//sigma is infinite, no need to: this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
	if(this->flow->rate.updated){
		this->rate = this->flow->rate.value;
	    this->sigma = 0;
	    debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowRateSelector::dext: flowID=%s  updated rate=[%f][%f][%f] \n", t, this->getFullName().data(), flow->flowId.value.data(), this->rate[0], this->rate[1], this->rate[2]);
	}

	return;
}

Event FlowRateSelector::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowRateSelector::lambda:   sending rate=[%f][%f][%f] \n", t, this->getFullName().data(), this->rate[0], this->rate[1], this->rate[2]);
	return Event(this->rate,0);
}
