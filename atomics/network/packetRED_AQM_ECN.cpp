#include "packetRED_AQM_ECN.h"

void packetRED_AQM_ECN::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char *fvar;
	fvar = va_arg(parameters,char*);
	this->minTH = readDefaultParameterValue<int>(fvar);
	fvar = va_arg(parameters,char*);
	this->pMinTH = readDefaultParameterValue<double>(fvar);
	fvar = va_arg(parameters,char*);
	this->maxTH = readDefaultParameterValue<int>(fvar);
	fvar = va_arg(parameters,char*);
	this->pMaxTH = readDefaultParameterValue<double>(fvar);
	fvar = va_arg(parameters,char*);
	this->alpha = readDefaultParameterValue<double>(fvar);

	p = NULL;
	this->queueSize = 0;
	this->marked = 0;
	this->drop_prob = 0;
	this->sampledQueueSize = 0;
	this->phase = WAITING_FOR_PACKET;
	this->sigma = std::numeric_limits<double>::infinity();  // Goes waiting

	this->logger->initSignals(std::map<std::string, int>{
		{"queueAvg", SCILAB_LOG_LEVEL_ALWAYS},
		{"drop_prob", SCILAB_LOG_LEVEL_ALWAYS},
		{"mark_bits", SCILAB_LOG_LEVEL_ALWAYS},
		{"mark_count", SCILAB_LOG_LEVEL_DEBUG}
	});

	return;
}

void packetRED_AQM_ECN::dint(double t) {
	phase = WAITING_FOR_PACKET ;
	p = NULL;
	sigma = std::numeric_limits<double>::infinity();
}

void packetRED_AQM_ECN::dext(Event x, double t) {
	if (x.port==0) {   // A new packet enters
		if(p != NULL){ // validate
			debugMsg(LOG_LEVEL_ERROR, "[%g] %s::dext: New packet received but previous packet was yet not forwarded. Are you missing a queue before this model? \n",t, this->getFullName().data());
			throw std::runtime_error("packetRED_AQM_ECN::dext: New packet received but previous packet was yet not forwarded. Are you missing a queue before this model? \n");
		}

		p = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

		this->updateMeasuredQueueSize(t, this->queueSize);
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Ext]: New packet received. Updated sampledQueueSize=%g \n",t, this->getFullName().data(), this->sampledQueueSize);

		// DECIDE DROP ACTION
		double coinflip=STDEVS::getConfigurationInstance()->uniform();

		// CORRECT:
		if (coinflip<=drop_prob)
		{
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: Packet #%u marked  \n", t, this->getFullName().data() );

			// Get the route (from the protocol)
			auto tcpProtocol = p->getProtocol(PROTOCOL_ID_TCP_IP);
			if(!tcpProtocol){
				debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without TCP IP protocol. Not possible to mark for congestion. \n ", t, this->getFullName().data(), p->getId());
				return;
			}
			std::static_pointer_cast<TcpIpProtocol>(tcpProtocol)->setMarkCongestion(true);

			marked++;
			this->logger->logSignal(t, marked, "mark_count");
			this->logger->logSignal(t, p->getLength_bits(), "mark_bits");
		}

		sigma=0;    // DO NOT DROP: Emit the packet
		phase=T_EMITTING_PACKET;
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ext]: programmed to send Packet #%u marked (drop_prob(%g)<coinflip(%g)) \n", t, this->getFullName().data(), p->getId(), drop_prob,coinflip);

	} else if (x.port==1) {   // A new Queue Length is notified
		this->queueSize = ((double*)x.value)[0]; // read&store value

		if(this->queueSize == 0){
			this->q_time = t; // record the time when the queue became empty
		}

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: received a new queue length: %g. Current sampledQueueSize=%g \n",t, this->getFullName().data(), this->queueSize, this->sampledQueueSize);
		if(p == NULL){ // nothing to send
			phase = WAITING_FOR_PACKET ;
			sigma = std::numeric_limits<double>::infinity();//sigma - e; // Matias fix: keep as before. Setting sigma=inf might cause a packet to no be forwarded if update arrives in the same t
		} else { // there is a packet to send
			sigma=0;    // DO NOT DROP: Emit the packet
			phase=T_EMITTING_PACKET;
		}
	}
}

Event packetRED_AQM_ECN::lambda(double t) {
	switch (phase){
	case T_EMITTING_PACKET:
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: Emit packet #%u \n", t, this->getFullName().data(),p->getId());
		return Event(p,0);
		break;
	default:
		debugMsg(LOG_LEVEL_ERROR, "[%g] %s::lambda: Error: Do not know what to do... \n", t, this->getFullName().data());
		throw std::runtime_error("packetRED_AQM_ECN::lambda - Error: Do not know what to do... \n");
		return Event();
		break;
	}
}

/* this is implements the weighted sampled average */
void packetRED_AQM_ECN::updateMeasuredQueueSize(double t, double newQueueSizeSample){
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
void packetRED_AQM_ECN::updateDropProbability(){
	//debugMsg(LOG_LEVEL_DEBUG, "[%g] RED: minTH %i maxTH %i pMinTH %g pMaxTH %g \n",t,minTH,maxTH,pMinTH,pMaxTH);

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
