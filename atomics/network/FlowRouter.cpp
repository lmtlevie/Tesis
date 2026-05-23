#include "FlowRouter.h"
#include "NetworkPacket.h"

void FlowRouter::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);

	this->sigma = std::numeric_limits<double>::max(); // Immediately asks for a new packet

	// set up routing table
	this->table = new RouterTable(this->father->getFullName()); // TODO: if need to use it in another composition level, receive the name as parameter
	this->table->buildFromConfig();

	// Set up logging
	this->logger->initSignals(std::map<std::string, int>{
		{"incoming_bits", SCILAB_LOG_LEVEL_IMPORTANT}, // to calculate incoming throughput (only works for packet-level simulations)
		{"incoming_packets", SCILAB_LOG_LEVEL_PRIORITY} // (only works for packet-level simulations)
	});
}

void FlowRouter::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->pendingFlows.empty()){
		this->sigma =0; // program transition immediately 
	}
}

void FlowRouter::dext(Event x, double t) {
	auto arrivedFlow = castEventPointer<IFlow>(x); // get the flow from the incoming event
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowRouter::dext: Received incomming port=%i, flowId=%s \n", t, this->getFullName().data(), x.port, arrivedFlow->flowId.value.data());

	auto packet = std::dynamic_pointer_cast<NetworkPacket>(arrivedFlow); // check if it is a packet (might be a flow)
	if(packet){
		this->logger->logSignal(t,packet->getLength_bits(),"incoming_bits");
		this->logger->logSignal(t,1,"incoming_packets");		
	}

	this->pendingFlows.push_back({arrivedFlow, x.port});
	this->sigma = 0; // program transition immediately
}

Event FlowRouter::lambda(double t) {
	auto pendingFlow = this->pendingFlows.front();
	this->pendingFlows.pop_front();

	auto flow = pendingFlow.first;
	auto inPort = pendingFlow.second;
	int outPort = this->table->getOutgoingPort(flow->flowId.value, inPort);

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::FlowRouter::lambda:  incomming port=%i, flowId=%s sent through output port=%i  \n", t, this->getFullName().data(), inPort, flow->flowId.value.data(), outPort);

	return Event(flow, outPort);
}

void FlowRouter::exit(double t){
	BaseSimulator::exit(t);
	if(this->table) delete this->table;
}

