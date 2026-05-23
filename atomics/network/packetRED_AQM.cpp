#include "packetRED_AQM.h"

void packetRED_AQM::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char *fvar;
	fvar = va_arg(parameters,char*);
	this->minTH = readDefaultParameterValue<unsigned long>(fvar);
	fvar = va_arg(parameters,char*);
	this->pMinTH = readDefaultParameterValue<double>(fvar);
	fvar = va_arg(parameters,char*);
	this->maxTH = readDefaultParameterValue<unsigned long>(fvar);
	fvar = va_arg(parameters,char*);
	this->pMaxTH = readDefaultParameterValue<double>(fvar);
	fvar = va_arg(parameters,char*);
	this->alpha = readDefaultParameterValue<double>(fvar);

	// validate
	if(this->alpha<0 || this->alpha > 1){
		debugMsg(LOG_LEVEL_EXCEPTION, "[INIT] %s: wrong value for parameter alpha=%f . Value should be in the range [0,1]. \n", this->getFullName().data(), this->alpha);
	}
	if(this->minTH > this->maxTH){
		debugMsg(LOG_LEVEL_EXCEPTION, "[INIT] %s: wrong value for parameters minTH=%lu and maxTH=%lu . Should be minTH<maxTH. \n", this->getFullName().data(), this->minTH, this->maxTH);
	}
	if(this->pMinTH > this->pMaxTH){
		debugMsg(LOG_LEVEL_EXCEPTION, "[INIT] %s: wrong value for parameters pMinTH=%f and pMaxTH=%f . Should be pMinTH<pMaxTH. \n", this->getFullName().data(), this->pMinTH, this->pMaxTH);
	}

	this->queueSize = 0;
	this->dropped = 0;
	this->drop_prob = 0;
	this->sampledQueueSize = 0;
	this->phase = WAITING_FOR_PACKET;
	this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting

	this->logger->initSignals(std::map<std::string, int>{
		{"queueAvg", LOG_LEVEL_IMPORTANT},
		{"drop_prob", LOG_LEVEL_IMPORTANT},
		{"drop_bits", LOG_LEVEL_IMPORTANT},
		{"drop_count", LOG_LEVEL_DEBUG}
	});

	return;
}

void packetRED_AQM::dint(double t) {
	if(this->outputPackets.size() == 0){ // nothing to send
		phase = WAITING_FOR_PACKET ;
		sigma = std::numeric_limits<double>::infinity();//sigma - e; // Matias fix: keep as before. Setting sigma=inf might cause a packet to no be forwarded if update arrives in the same t
	} else { // there is a packet to send
		sigma=0;    // Emit the packet
		phase=T_EMITTING_PACKET;
	}
}

void packetRED_AQM::dext(Event x, double t) {
	if (x.port==0) {   // A new packet enters
		if(this->outputPackets.size() != 0){ // validate
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s::dext: New packet received but there are still %d packets not yet forwarded. 2 packets in the very same instant might be a signal that a queue/link is missing \n",t, this->getFullName().data(), this->outputPackets.size());
		}

		auto p = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

		this->updateMeasuredQueueSize(t, this->queueSize);
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Ext]: New packet received. Updated sampledQueueSize=%g \n",t, this->getFullName().data(), this->sampledQueueSize);

		// DECIDE DROP ACTION
		double coinflip=STDEVS::getConfigurationInstance()->uniform();

		//debugMsg(LOG_LEVEL_DEBUG, "[%g] RED: *DECIDE DROP ACTION* if [coinflip: %g] IS [<=] that [drop_prob: %g] \n",t,coinflip,drop_prob);

		// CORRECT:
		if (coinflip<=drop_prob)
		{
			sigma=0;    // DROP: Notify it
			phase=T_NOTIFYING_DROP;
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: NOTIFY to DROP (drop_prob(%g)>=coinflip(%g))! \n", t, this->getFullName().data(),drop_prob,coinflip);
			//			p->printInfo(LOG_LEVEL_DEBUG);
			dropped++;
			this->logger->logSignal(t, dropped, "drop_count");
			this->logger->logSignal(t, p->getLength_bits(), "drop_bits");
		} else {
			sigma=0;    // DO NOT DROP: Emit the packet
			this->outputPackets.push(p);
			phase=T_EMITTING_PACKET;
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: ADMITTED (drop_prob(%g)<coinflip(%g)) - EMITTING PACKET!\n", t, this->getFullName().data(),drop_prob,coinflip);
		}
	} else if (x.port==1) {   // A new Queue Length is notified
		this->queueSize = ((double*)x.value)[0]; // read&store value

		if(this->queueSize == 0){
			this->q_time = t; // record the time when the queue became empty
		}

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: received a new queue length: %g. Current sampledQueueSize=%g \n",t, this->getFullName().data(), this->queueSize, this->sampledQueueSize);
		if(this->outputPackets.size() == 0){ // nothing to send
			phase = WAITING_FOR_PACKET ;
			sigma = std::numeric_limits<double>::infinity();//sigma - e; // Matias fix: keep as before. Setting sigma=inf might cause a packet to no be forwarded if update arrives in the same t
		} else { // there is a packet to send
			sigma=0;    // Emit the packet
			phase=T_EMITTING_PACKET;
		}
	}
}

Event packetRED_AQM::lambda(double t) {
	switch (phase){
	case T_NOTIFYING_DROP:{
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::lambda: Notifying packet drop (outport=1) \n", t, this->getFullName().data());
		return Event(&this->queueSize, 1); // notify drop
		break;
	} case T_EMITTING_PACKET:{
		// unqueue packet & send
		auto p = this->outputPackets.front();
		this->outputPackets.pop();
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::lambda: Forwarding packet #%u \n", t, this->getFullName().data(),p->getId());
		return Event(p,0);
		break;
	} default:
		debugMsg(LOG_LEVEL_EXCEPTION, "[%g] %s::lambda: Error: Do not know what to do... \n", t, this->getFullName().data());
		return Event();
		break;
	}
}

/* this is implements the weighted sampled average */
void packetRED_AQM::updateMeasuredQueueSize(double t, double newQueueSizeSample){
	// update based on the RED w-avg algorithm: http://www.icir.org/floyd/papers/early.twocolumn.pdf
	if(newQueueSizeSample > 0){
		this->sampledQueueSize = (1 - alpha)*sampledQueueSize + alpha*newQueueSizeSample;
	} else	{
		double m = (t - this->q_time) * 1000; // in ms
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::updateMeasuredQueueSize:: with newSize=0;  m=%g  ; sampledQueueSize=%g (will be updated to %g) \n",t, this->getFullName().data(), m, sampledQueueSize, pow(1 - alpha, m) * sampledQueueSize);
		this->sampledQueueSize = pow(1 - alpha, m) * sampledQueueSize; // if queue is empty decrease avg as time goes by
	}

	this->updateDropProbability();

	this->logger->logSignal(t, this->sampledQueueSize, "queueAvg");
	this->logger->logSignal(t, this->drop_prob, "drop_prob");
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::updateMeasuredQueueSize:: logged drop_prob=%g  \n",t, this->getFullName().data(), this->drop_prob);
}

/* this is implements the AQM dropProb function  */
void packetRED_AQM::updateDropProbability(){
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] RED: minTH %f maxTH %f pMinTH %g pMaxTH %g \n",t,minTH,maxTH,pMinTH,pMaxTH);

	if (this->sampledQueueSize < this->minTH) {
		this->drop_prob = 0;
	}
	if (this->sampledQueueSize == this->minTH) {
		this->drop_prob = pMinTH;
	}
	if (this->sampledQueueSize == this->maxTH) {
		this->drop_prob = pMaxTH;
	}
	if (this->sampledQueueSize > this->maxTH) {
		this->drop_prob = 1;
	}
	if (this->sampledQueueSize>minTH && sampledQueueSize<maxTH ) {
		drop_prob = pMinTH+(pMaxTH-pMinTH)/(maxTH-minTH)*(sampledQueueSize-minTH);
	}
}
