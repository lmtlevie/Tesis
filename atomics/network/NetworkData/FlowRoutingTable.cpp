#include "FlowRoutingTable.h"

FlowRoutingTable::FlowTable FlowRoutingTable::RoutingTable;
//std::unordered_map<std::string, int> FlowRoutingTable::DemuxPorts;


int FlowRoutingTable::getOutgoingPort(const std::string& nodeName, const std::string& flowId , int inputPort){
	if(RoutingTable.empty()) {// call just once (unless it does not define any flows...)
		defineRoutes();
	}

	auto searchNode = RoutingTable.find(nodeName);
	if ( searchNode == RoutingTable.end() ) { // not found
		throw std::runtime_error("FlowRoutingTable::getOutgoingPort - '" + nodeName + "' is not defined in the routing table. Check the CmdLine configuration (routes can not be defined in scilab)"); // Flow names must be defined in the CmdLine configuration (PD<->Scilab can not read strings)
	}

	auto searchInput= searchNode->second.find(inputPort);
	if ( searchInput == searchNode->second.end() ) { // not found
		throw std::runtime_error("'" + std::to_string(inputPort) + "' input port is not defined in the routing table for node '"+ nodeName +"'. Check the CmdLine configuration (routes can not be defined in scilab)"); // Flow names must be defined in the CmdLine configuration (PD<->Scilab can not read strings)
	}

	auto searchFlow= searchInput->second.find(flowId);
	if ( searchFlow == searchInput->second.end() ) { // not found
		throw std::runtime_error("'" + flowId + "' flow is not defined in the routing table for node '"+ nodeName +"', input port '" + std::to_string(inputPort) + ". Check the CmdLine configuration (routes can not be defined in scilab)"); // Flow names must be defined in the CmdLine configuration (PD<->Scilab can not read strings)
	}

	return searchFlow->second;
}

//int FlowRoutingTable::getDemultiplexingPort(const std::string& flowId){
//	if(DemuxPorts.empty()) {// call just once (unless it does not define any flows...)
//		defineRoutes();
//	}
//
//	auto search = DemuxPorts.find({flowId});
//	if ( search == DemuxPorts.end() ) { // not found
//		throw std::runtime_error("'" + flowId + "' has no demux outPort defined. Check the CmdLine configuration (routes can not be defined in scilab)"); // Flow names must be defined in the CmdLine configuration (PD<->Scilab can not read strings)
//	}
//
//	return search->second;
//}

void FlowRoutingTable::defineRoutes(){
	// Flow names must be defined in the CmdLine configuration (PD<->Scilab can not read strings)
	std::vector<std::string> flowNames;
	if(!getCmdLineParam<std::vector<std::string>>(FLOW_IDS_PARAM, flowNames)){
		return; // no flows declared
	}

	std::string flowName;
	for(uint flowIndex=0; flowIndex<flowNames.size(); flowIndex++){
		flowName = flowNames[flowIndex];

		// create the route from CmdLine config
		auto routeNames = getCmdLineParam<std::vector<std::string>>(flowName + ROUTE_NAMES_PARAM);
		auto inPorts = getCmdLineParam<std::vector<int>>(flowName + ROUTE_INPORTS_PARAM);
		auto outPorts = getCmdLineParam<std::vector<int>>(flowName + ROUTE_OUTPORTS_PARAM);
		if(routeNames.size() != inPorts.size()){
			throw std::runtime_error("ERROR: Parameters '"+flowName + ROUTE_NAMES_PARAM +"' and '"+flowName + ROUTE_OUTPORTS_PARAM + "' must be of the same size.");
		}
		if(outPorts.size() != inPorts.size()){
			throw std::runtime_error("ERROR: Parameters '"+flowName + ROUTE_OUTPORTS_PARAM +"' and '"+flowName + ROUTE_OUTPORTS_PARAM + "' must be of the same size.");
		}

		// add entries to the routing table
		for(uint i=0; i<routeNames.size(); i++){
			RoutingTable[routeNames[i]][inPorts[i]][flowName] = outPorts[i];
		}
	}
}

FlowRoutingTable::RouterTable FlowRoutingTable::getRouterTable(const std::string& nodeName){
	if(RoutingTable.empty()) {// call just once (unless it does not define any flows...)
		defineRoutes();
	}

	auto searchNode = RoutingTable.find(nodeName);
	if ( searchNode == RoutingTable.end() ) { // not found
		return RouterTable();
	}

	return searchNode->second;
}


