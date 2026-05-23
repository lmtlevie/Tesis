#include "TcpDstPort2Vector.h"

void TcpDstPort2Vector::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	this->sigma = std::numeric_limits<double>::infinity();

}

void TcpDstPort2Vector::dint(double t) {
	sigma = std::numeric_limits<double>::infinity();
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void TcpDstPort2Vector::dext(Event x, double t) {
	// Event arrived, get the packet from vectorial or clasic event
	auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event

//	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// find out DST address to which this packet is directed to (in the IP protocol)
	auto tcpProtocol = packet->getProtocol(PROTOCOL_ID_TCP_IP);
	if(!tcpProtocol){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without TCP IP protocol. \n ", t, this->getFullName().data(), packet->getId());
		throw std::runtime_error("TcpDstPort2Vector: received packet #%u without TCP IP protocol." );
		return;
	}
	int portDst = std::static_pointer_cast<TcpIpProtocol>(tcpProtocol)->getPortDst();

	// Change the index of the event to go to the corresponding destinationId
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = packet;
	vec->index = portDst;

//	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u Prot%i[Head%i]=%i . Sending it with index=%i \n", t, this->getName(), arrivedPacket->ID, this->getProtocolId(), this->getProtocolHeaderId(), sourceId, vec->index);

	outputVectorialEvents.push(vec);
	sigma = 0;
}

Event TcpDstPort2Vector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	//printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}
