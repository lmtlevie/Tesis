#include "FluidFlowCreator.h"

void FluidFlowCreator::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	this->flowId = std::string(va_arg(parameters,char*));
	if(this->flowId.empty()){ // if empty get it from the parents name
		this->flowId = this->father->getFullName();
	}
	//	this->level= readDefaultParameterValue<double>(fvar );

	//  performed at variable creation (in the .h)
	//	for(int i=0;i<10;i++){u[i]=0;};
	//	for(int i=0;i<10;i++){y[i]=0;};

	// send an initial event to propagate flowID
	this->flow = std::make_shared<FluidFlow>(this->flowId, departureRate, dropsRate, delay);
	this->flow->flowId.updated = true;

	this->sigma = 0;
}


void FluidFlowCreator::dint(double t) {
	this->flow.reset();
	this->sigma = std::numeric_limits<double>::infinity();
}

void FluidFlowCreator::dext(Event x, double t) {
	double *input = (double*)(x.value);

	for (int i=0;i<10;i++) { // copy polinomio (QSS values)
		this->departureRate[i]=input[i];
	}

	// create flow with departure updated
	if(!this->flow){
		this->flow = std::make_shared<FluidFlow>(this->flowId, this->departureRate, this->dropsRate, this->delay);
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowCreator::dext:  created new flowID=%s with departureRate==[%f][%f][%f]  \n", t, this->getFullName().data(), flow->flowId.value.data(), this->flow->rate.value[0], this->flow->rate.value[1], this->flow->rate.value[2]);
	} else { // at the beggining the flow is already created with the id updated. Afterwards, the id.updated = false
		this->flow->rate.value = this->departureRate;
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowCreator::dext:  flowID=%s updated departureRate==[%f][%f][%f]  \n", t, this->getFullName().data(), flow->flowId.value.data(), this->flow->rate.value[0], this->flow->rate.value[1], this->flow->rate.value[2]);
	}
	flow->rate.updated = true;


	this->sigma = 0;
	return;
}

Event FluidFlowCreator::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowCreator::lambda:  sending flowID=%s with departureRate==[%f][%f][%f]  \n", t, this->getFullName().data(), flow->flowId.value.data(), this->flow->rate.value[0], this->flow->rate.value[1], this->flow->rate.value[2]);
	return Event(this->flow,0);
}
