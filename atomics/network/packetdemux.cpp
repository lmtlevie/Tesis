#include "packetdemux.h"

void packetdemux::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	// TODO: using 4 is too rigid. Make the first param to be n and then iterate n times reading args
	for (int i=0;i<4;i++) {
		Labels[i] = va_arg(parameters,char*);
		debugMsg(LOG_LEVEL_INIT, "[INIT] %s: Label %u = %s \n", this->getName(), i, Labels[i].data());
	};

	char* fvar = va_arg(parameters,char*);
	n = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: Number of Labels: %u \n", this->getName(), n);

	sigma = std::numeric_limits<double>::infinity(); //wait until next packet arrives
}

void packetdemux::dint(double t) {
	sigma=INF;
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}

void packetdemux::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition

	auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
	packetBuffer.push(arrivedPacket);

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_ext: Packet arrived with flowId=%s \n",t, this->getName(), arrivedPacket->flowId.value.data());

	sigma = 0;
}

Event packetdemux::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	for (int i=0;i<n;i++) {
		if (Labels[i].compare("*") == 0 || Labels[i].compare(packetToSend->flowId.value.data()) == 0) {
			debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: forwarding packet %u(%s) by outport %u (matched label: %s) \n", t, this->getName(), packetToSend->getId(), packetToSend->flowId.value.data(), i, Labels[i].data());
			return Event(packetToSend,i);
		}
	}

	return Event(NULL,0);
}

