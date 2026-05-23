#include "RouterTable.h"

void RouterTable::buildFromConfig(){
	if(!this->table.empty()) {
		throw std::runtime_error(this->routerId + "::RouterTable::buildFromConfig - Table has been already built! Call build only once at init");//defineRoutes();
	}
	// retrieve the flow-based table
	this->table = FlowRoutingTable::getRouterTable(this->routerId);

	std::vector<std::string> flows;
	if(!getCmdLineParam<std::vector<std::string>>(routerId + FLOWS_PARAM, flows)){
		if(this->table.empty()){
			printLog(LOG_LEVEL_ALWAYS, "WARNING: router '%s' does not have any flows configured (no parameter '%s' was found) and non of the global flows (parameter '%s') pass through the router (parameter '<flow_id>%s') \n", this->routerId.data(), (routerId + FLOWS_PARAM).data(), FLOW_IDS_PARAM, ROUTE_NAMES_PARAM );
		}
		return; // no flows specified for this router
	}

	for (auto flowId : flows){
		auto inPorts =  getCmdLineParam<std::vector<int>>(routerId + "." + flowId + INPORTS_PARAM);
		auto outPorts =  getCmdLineParam<std::vector<int>>(routerId + "." + flowId + OUTPORTS_PARAM);

		if(inPorts.size() != outPorts.size()){
			std::stringstream stream;
			stream << "ERROR: Parameters '" << routerId << INPORTS_PARAM << " and '" <<
					routerId << OUTPORTS_PARAM << "' must be of the same size.";
			throw std::runtime_error(stream.str());
		}

		// add entries to the routing table
		for(uint i=0; i < inPorts.size(); i++){
			this->table[inPorts[i]][flowId] = outPorts[i];
		}
	}
}

int RouterTable::getOutgoingPort(const FlowId& flowId, InPort inputPort){
	if(this->table.empty()) {// call just once (unless it does not define any flows...)
		throw std::runtime_error(this->routerId + "::RouterTable::getOutgoingPort - Attempting to retrieve from an uninitialized routing table. Forgot to call build at init?");//defineRoutes();
	}

	auto searchPort = this->table.find(inputPort);
	if ( searchPort == this->table.end() ) { // not found
		searchPort = this->table.find(WILD_CARD_INPORT); // search wildcard (any) incoming port
		if ( searchPort == this->table.end() ) { // not found
			std::stringstream stream;
			stream << this->routerId <<  "::RouterTable::getOutgoingPort - " <<
					" The input port '" << inputPort <<
					"' is not defined in the routing table of router '" << this->routerId <<
					"'. Check the CmdLine configuration (routes can not be defined in scilab)";
			throw std::runtime_error(stream.str());
		}
	}

	auto searchFlow = searchPort->second.find(flowId);
	if ( searchFlow == searchPort->second.end() ) { // not found
		searchFlow = searchPort->second.find(WILD_CARD_FLOW);
		if ( searchFlow == searchPort->second.end() ) { // not found
			std::stringstream stream;
			stream << this->routerId <<  "::RouterTable::getOutgoingPort - " <<
					" The flowId '" << flowId <<
					"' incoming port '" << inputPort <<
					"' is not defined in the routing table of router '" << this->routerId <<
					"'. Check the CmdLine configuration (routes can not be defined in scilab)";
			throw std::runtime_error(stream.str());
		}
	}

	return searchFlow->second;
}
