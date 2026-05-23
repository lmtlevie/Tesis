#include "PacketRouter.h"

void PacketRouter::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

//	char* fvar;
//	fvar = va_arg(parameters,char*);
//	this->capacity_bit_s_r = readDefaultParameterValue<double>(fvar);
//	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: capacity_bit_s: %f bits/s. \n", this->getFullName().data(), this->capacity_bit_s_r);
//	this->capacity_bit_s_r = 1 / this->capacity_bit_s_r; // revert it to avoid continuous divisions later
//

	this->sigma = std::numeric_limits<double>::max(); // Immediately asks for a new packet

//	this->logger->initSignals({
//		{"sent_bits", SCILAB_LOG_LEVEL_ALWAYS},
//		{"sent_packets", SCILAB_LOG_LEVEL_PRIORITY},
//		{"processingTime", SCILAB_LOG_LEVEL_DEBUG},
//	});
}

void PacketRouter::dint(double t) {
	this->sigma=INF;
	if(this->outputPackets.size() != 0){
		this->sigma = 0;
	}
}

void PacketRouter::dext(Event x, double t) {
	// Packet arrived
	auto incomingPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

	// Get the route (from the protocol)
	auto protocol = incomingPacket->getProtocol(PROTOCOL_ID_ABSTRACT_ROUTING);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without routing protocol. discarding. \n ", t, this->getFullName().data(), incomingPacket->getId());
		return;
	}
	auto route = std::static_pointer_cast<RoutingProtocol>(protocol)->getRoute();

	// Check this is the appropriate node in the route
	if(route->getNextJump().nodeName != this->getName() && route->getNextJump().nodeName != this->father->getName()){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u which was not destined for this node (next jump: %s). Discarding.  \n ", t, this->getFullName().data(), incomingPacket->getId(), route->getNextJump().nodeName.data());
		incomingPacket->printInfo(LOG_LEVEL_ERROR);
		return;
	}

	// Decide which out port to use for forwarding based on the route
	int outPort = route->getNextJump().outgoingPort;
	this->outputPackets.push({ outPort, incomingPacket});
	this->sigma = 0;

	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received packet #%u from inPort %u. Forwarding through port %u. \n ", t, this->getFullName().data(), incomingPacket->getId(), x.port, outPort);

	// remove node from route
	route->popJump();
}

Event PacketRouter::lambda(double t) {
	auto outEvent = this->outputPackets.front();
	this->outputPackets.pop();

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding packet #u through port %u \n", t, this->getFullName().data(), outEvent.packet->getId(), outEvent.outgoingPort);
	return Event(outEvent.packet,outEvent.outgoingPort);
}

