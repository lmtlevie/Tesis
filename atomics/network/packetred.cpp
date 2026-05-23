#include "packetred.h"
void packetred::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar;
	fvar = va_arg(parameters,char*);
	minTH = readDefaultParameterValue<int>(fvar);
	fvar = va_arg(parameters,char*);
	PminTH = readDefaultParameterValue<double>(fvar);
	fvar = va_arg(parameters,char*);
	maxTH = readDefaultParameterValue<int>(fvar);
	fvar = va_arg(parameters,char*);
	PmaxTH = readDefaultParameterValue<double>(fvar);

	for (int i=0;i<10;i++) { dropped[i]=0; }

	measuredQueueState=0;
	drop_prob=0;

	this->logger->initSignals({"probdrop","measuredQueue","drop_count"});

	phase=WAITINGFORPACKET;
	sigma=std::numeric_limits<double>::infinity();  // Goes waiting
	return;
}

void packetred::dint(double t) {
	phase = WAITINGFORPACKET ;
	sigma = std::numeric_limits<double>::infinity() ;
}
void packetred::dext(Event x, double t) {
	if (x.port==0) {   // A new packet enters
		p = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent
//		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s(dext):p1 receive a new packet: \n",t, this->getFullName());

		// DECIDE DROP ACTION
		double coinflip=STDEVS::getConfigurationInstance()->uniform();
		//KIKE: prueba si coinflip > drop_prob entonces NO elimina paquetes
		//coinflip=1.1;//Mayor al 100% ;)
		//printLog("[%g] RED: *DECIDE DROP ACTION* if [coinflip: %g] IS [<=] that [drop_prob: %g] \n",t,coinflip,drop_prob);

		// CORRECT:
		if (coinflip<=drop_prob)
		{
			sigma=0;    // DROP: Notify it
			phase=T_NOTIFYINGDROP;
			dropped[0]++;
			debugMsg(LOG_LEVEL_IMPORTANT, "[%g] RED: dropped: %g \n",t,dropped[0]);
			this->logger->logSignal(t,dropped[0],"drop_count");
			return;
		} else {
			sigma=0;    // DO NOT DROP: Emit the packet
			phase=T_EMITTINGPACKET;
			return;
		}
	} else if (x.port==1) {   // A new Queue Length is notified
		double *aux = (double*)x.value;// LOG
		measuredQueueState=(int)(*aux);
		debugMsg(LOG_LEVEL_DEBUG, "[%g] RED[ExT]:p1 receive a new Queue Length is notified: x.value(%g), measuredQueueState(%u) \n",t,aux[0],measuredQueueState);

		if (measuredQueueState<minTH) {
			drop_prob=0; }
		if (measuredQueueState==minTH) {
			drop_prob=PminTH; }
		if (measuredQueueState==maxTH) {
			drop_prob=PmaxTH; }
		if (measuredQueueState>maxTH) {
			drop_prob=1; }
		if (measuredQueueState>minTH && measuredQueueState<maxTH ) {
			drop_prob=PminTH+(PmaxTH-PminTH)/(maxTH-minTH)*(measuredQueueState-minTH);
		}

		debugMsg(LOG_LEVEL_DEBUG, "[%g] RED[ExT]:p1 measuredQueueState: %u new drop_prob: %g \n",t,measuredQueueState,drop_prob);

		this->logger->logSignal(t,drop_prob,"probdrop");
		this->logger->logSignal(t,measuredQueueState,"measuredQueue");

		phase=WAITINGFORPACKET;
		sigma=std::numeric_limits<double>::infinity();
		return;
	};
}
Event packetred::lambda(double t) {
	switch (phase)
	{
	case T_NOTIFYINGDROP:
		debugMsg(LOG_LEVEL_DEBUG,"[%g] RED_%s[Out]: DROP event \n",t, this->getName());
		return Event(p,1);
		break;
	case T_EMITTINGPACKET:
		debugMsg(LOG_LEVEL_DEBUG,"[%g] RED_%s[Out]: Emit PACKET ID(%u) \n",t,this->getName(),p->getId());
		return Event(p,0);
		break;
	default:
		//printLog("[%g] Channel: Error: Do not know what to do... \n",t);
		return Event();
		break;
	};
}

