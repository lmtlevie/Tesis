#ifndef FLOW_H_
#define FLOW_H_

#include <stdlib.h>
#include <string>

//#include "Route.h"

#include "sinks/ParameterReader.h"
#include "hybrid/DistributionParameters/IDistributionParameter.h"


/**
 *  Flows (or classes in the the MVA parlance) define the communication between different nodes.
 *
 */
class Flow {
private:
	std::string flowId; // flowName 
	std::vector<double> startStopTimes;
	int typeOfService; // to use in QoS and priority queues (see PacketPrioritySelect.h, QualityOfServiceProtocol.h)
	std::shared_ptr<IDistributionParameter> period;
	std::shared_ptr<IDistributionParameter> size;
//	std::shared_ptr<Route> route;
	// TODO: implement --> finishTime

public:
	Flow(const std::string& flowName, std::vector<double> startStopTimes, int typeOfService, std::shared_ptr<IDistributionParameter> period, std::shared_ptr<IDistributionParameter> size) :
		flowId(flowName), startStopTimes(startStopTimes), typeOfService(typeOfService), period(period), size(size){
	}

//	Flow(const std::string& flowName, std::vector<double> startStopTimes, int typeOfService, std::shared_ptr<IDistributionParameter> period, std::shared_ptr<IDistributionParameter> size, std::deque<Route::Node> routePath) :
//		Flow(flowName, startStopTimes, typeOfService, period, size, std::make_shared<Route>(routePath)) {
//	}

	~Flow() {	}

	std::string getFlowId(){ return this->flowId; }
	std::vector<double> getStartStopTimes(){ return this->startStopTimes; }
	std::shared_ptr<IDistributionParameter> getPeriod(){ return this->period; }
	std::shared_ptr<IDistributionParameter> getSize(){ return this->size; }
	int getTypeOfService(){ return this->typeOfService; }/** Returns the type of service for the flow. Used in QoS and priority queues (see PacketPrioritySelect.h) **/

	/** Returns a copy of the route **/
//	std::shared_ptr<Route> getRoute(){ return std::make_shared<Route>(*this->route); }
};


#endif
