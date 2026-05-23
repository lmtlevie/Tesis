#include "PacketFlowDefinitions.h"

std::multimap<std::string, std::shared_ptr<Flow>> PacketFlowDefinitions::Flows;


void PacketFlowDefinitions::addFlow(const std::string& flowName, std::vector<double> startStopTimes, int typeOfService, std::shared_ptr<IDistributionParameter> period, std::shared_ptr<IDistributionParameter> size){
	addFlow(std::make_shared<Flow>(flowName, startStopTimes, typeOfService, period, size));
}

//void PacketFlowDefinitions::addFlow(const std::string& flowName, std::vector<double> startStopTimes, int typeOfService, std::shared_ptr<IDistributionParameter> period, std::shared_ptr<IDistributionParameter> size, std::deque<Route::Node> routePath){
//	addFlow(std::make_shared<Flow>(flowName, startStopTimes, typeOfService, period, size, std::move(routePath)));
//}

void PacketFlowDefinitions::addFlow(std::shared_ptr<Flow> flow){
	// TODO: in the Server model, only a single flow can be served (here we allow multiple flows per server).
//	auto firtNode = flow->getRoute()->getNextJump().nodeName;
	Flows.insert({flow->getFlowId(), flow});
}

std::shared_ptr<Flow> PacketFlowDefinitions::getFlow(const std::string& flowName){
	if(Flows.empty()) {// call just once (unless it does not define any flows...)
		defineFlows();
	}

	for (auto key : Flows){ // TODO: use a map based on the name to avoid iteration
		if(key.second->getFlowId() == flowName)
			return key.second;
	}

	printLog(LOG_LEVEL_ERROR, "PacketFlowDefinitions::getFlow: There are is no flow with the name/id: '%s' \n", flowName.data());
	throw std::runtime_error("PacketFlowDefinitions::getFlow: There are is no flow with the name/id: '" + flowName + "'");
}

std::vector<std::shared_ptr<Flow>> PacketFlowDefinitions::getSourceFlows(const std::string& sourceName){
	if(Flows.empty()) {// call just once (unless it does not define any flows...)
		defineFlows();
	}

	std::vector<std::shared_ptr<Flow>> ret; // TODO: is there a way to avoid creating the vector and coping it around?

	auto range =  Flows.equal_range(sourceName);
	for (auto it = range.first; it != range.second; ++it){ // TODO: is there a way to avoid iteration (returning a multimap iterator is not nice)
		ret.push_back(it->second);
	}

	return ret;
}

void PacketFlowDefinitions::defineFlows(){
	// Flow names are defined in the CmdLine configuration (PD<->Scilab can not read Strings
	auto flowNames =  getCmdLineParam<std::vector<std::string>>(PARAMETER_PACKET_FLOW);

	for(auto flowName : flowNames){
		// Create each flow reading from config
		auto flow1PeriodDistribution = readDistributionParameter(flowName + ".period");
		auto flow1PacketSizeDistribution = readDistributionParameter(flowName + ".packetSize");
		auto flow1StartStopTimes = readDefaultParameterValue<std::vector<double>>(flowName + ".startStopTimes");
		int typeOfService = readDefaultParameterValue<int>(flowName + ".typeOfService");
		printLog(LOG_LEVEL_IMPORTANT, "PacketFlowDefinitions - %s : generation period: %s   - packetSize: %s \n", flowName.data(), flow1PeriodDistribution->getFullName().data(), flow1PacketSizeDistribution->getFullName().data());

		// create the route from CmdLine config
//		auto routeNames =  getCmdLineParam<std::vector<std::string>>(flowName + ".routeHosts");
//		auto routePorts =  getCmdLineParam<std::vector<int>>(flowName + ".routeOutPorts");
//		auto route = std::deque<Route::Node>();
//		if(routeNames.size() != routePorts.size()){
//			throw std::runtime_error("ERROR: Parameters '"+flowName + ".routeHosts' and '"+flowName + ".routeOutPorts' must be of the same size.");
//		}
//
//		for(uint i=0; i<routeNames.size(); i++){
//			route.push_back({routePorts[i], routeNames[i]});
//		}

		// create the flow
		PacketFlowDefinitions::addFlow(flowName, flow1StartStopTimes, typeOfService, flow1PeriodDistribution, flow1PacketSizeDistribution);
	}

	// Create each flow reading from config
//	auto flow3PeriodDistribution = readDistributionParameter("DiscreteTcpSender3.TCP_session_1.period");
//	auto flow3PacketSizeDistribution = readDistributionParameter("DiscreteTcpSender3.TCP_session_1.packetSize");
//	auto flow3StartStopTimes = readDefaultParameterValue<std::vector<double>>("DiscreteTcpSender3.TCP_session_1.startStopTimes");
//	printLog(1, "PacketFlowDefinitions - Flow 3: generation period: %s   - packetSize: %s \n", flow1PeriodDistribution->getFullName().data(), flow1PacketSizeDistribution->getFullName().data());
//	PacketFlowDefinitions::addFlow(flow3StartStopTimes, flow3PeriodDistribution,
//			flow3PacketSizeDistribution,
//			{ // Route
//					{0, "DiscreteTcpSender3"},
//					{0, "Router2"},
//					{1, "Router3"},
//			});
}




