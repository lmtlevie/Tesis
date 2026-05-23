#include "FlowAcumDelaySelector.h"

void FlowAcumDelaySelector::init(double t,...) {
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


void FlowAcumDelaySelector::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
}

void FlowAcumDelaySelector::dext(Event x, double t) {
	this->flow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowAcumDelaySelector::dext:   flowID=%s has arrived  \n", t, this->getFullName().data(), flow->flowId.value.data());

	//sigma is infinite, no need to: this->sigma = std::max(0.0, this->sigma - this->e); // continue as before
	if(this->flow->acumDelay.updated){
		this->acumDelay = this->flow->acumDelay.value;
	    this->sigma = 0;
	    debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowAcumDelaySelector::dext: updated flowID=%s   acum delay=[%g][%g][%g]   \n", t, this->getFullName().data(), flow->flowId.value.data(), this->acumDelay[0], this->acumDelay[1], this->acumDelay[2]);
	}

	return;
}

Event FlowAcumDelaySelector::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowAcumDelaySelector::lambda:   sending acum delay=[%g][%g][%g] \n", t, this->getFullName().data(), this->acumDelay[0], this->acumDelay[1], this->acumDelay[2]);
	return Event(this->acumDelay,0);
}
