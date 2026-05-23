#include "FluidFlowComposer.h"

void FluidFlowComposer::init(double t,...) {
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


void FluidFlowComposer::dint(double t) {
	this->flow.reset();
	this->sigma = std::numeric_limits<double>::infinity();
}

void FluidFlowComposer::dext(Event x, double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:  event arrived in port=%i  \n", t, this->getFullName().data(), x.port);

	switch (x.port) {
		case 0: {// Flow arrived
			// record all the values, but do not forward them
			auto arrivedFlow = castEventPointer<FluidFlow>(x); // get the flow from the incoming event
			this->sigma = std::max(0.0, this->sigma - this->e); // continue as before

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   flowID=%s arrived (through output port 0)  \n", t, this->getFullName().data(), arrivedFlow->flowId.value.data());

			// if the flow ID is updated, it is propagated through all the path immediately
			if(arrivedFlow->flowId.updated){
				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   flowID=%s has updated its ID  \n", t, this->getFullName().data(), arrivedFlow->flowId.value.data());
				this->id = arrivedFlow->flowId.value;

				// set/create flow with id updated
				if(!this->flow){
					this->flow = std::make_shared<FluidFlow>(this->id, this->rate, this->dropsRate, this->acumDelay);
				} else {
					this->flow->flowId.value = this->id;
				}
				this->flow->flowId.updated = true;

				debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   flowID=%s was recoded in model \n", t, this->getFullName().data(), flow->flowId.value.data());
				this->sigma = 0; // forward immediately
			}

			break;
		} case 1: {// Departure(t)
			// update departure and forward the full flow structure
			this->rate = (double*)(x.value);

			// create flow with departure updated
			if(!this->flow){
				this->flow = std::make_shared<FluidFlow>(this->id, this->rate, this->dropsRate, this->acumDelay);
			} else {
				this->flow->rate.value = this->rate;
			}
			flow->rate.updated = true;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   updated  rate=[%g][%g][%g]\n", t, this->getFullName().data(), this->rate[0],  this->rate[1],  this->rate[2]);

			// program transition now
			this->sigma = 0;

			break;
		} case 2: {// Drops(t)
			// update departure and forward the full flow structure
			this->dropsRate = (double*)(x.value);

			// create flow with departure updated
			if(!this->flow){
				this->flow = std::make_shared<FluidFlow>(this->id, this->rate, this->dropsRate, this->acumDelay);
			} else {
				this->flow->acumDrops.value = this->dropsRate;
			}
			flow->acumDrops.updated = true;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   updated  acumDrops=[%g][%g][%g]\n", t, this->getFullName().data(), this->dropsRate[0],  this->dropsRate[1],  this->dropsRate[2]);

			// program transition now
			this->sigma = 0;

			break;
		}case 3: {// Delay(t)
			// update departure and forward the full flow structure
			this->acumDelay = (double*)(x.value);

			// create flow with departure updated
			if(!this->flow){
				this->flow = std::make_shared<FluidFlow>(this->id, this->rate, this->dropsRate, this->acumDelay);
			} else {
				this->flow->acumDelay.value = this->acumDelay;
			}
			flow->acumDelay.updated = true;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::dext:   updated  acumDelay=[%g][%g][%g]\n", t, this->getFullName().data(), this->acumDelay[0],  this->acumDelay[1],  this->acumDelay[2]);

			// program transition now
			this->sigma = 0;

			break;
		}default:
			throw std::runtime_error("FluidFlowComposer::dext: wrong port /n");
			break;
	}
	return;
}

Event FluidFlowComposer::lambda(double t) {
	if(this->flow->flowId.value.empty()){ // if the flow id was not yet set does not make sense to forward it (and it breaks routing)
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FluidFlowComposer::lambda:  flowId is empty, not forwarding.  \n", t, this->getFullName().data());
		return Event();
	}

	debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s::FluidFlowComposer::lambda:   flowID=%s sent (through output port 0)  \n", t, this->getFullName().data(), flow->flowId.value.data());

	return Event(this->flow,0);
}
