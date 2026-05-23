#ifndef FlowRoutingTable_H_
#define FlowRoutingTable_H_

#include <memory>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include "sinks/ParameterReader.h"

using namespace std;

/**
 * A static global table that relates <nodeName, FlowId> with a <outPort>.
 * There is no inport, thus it can be used only in "uni-directional routers" where the flowId only can determine the outport
 *
 * Definition of flow routes (both for packet-level and fluid-flow).
 * Routes are read from configuration in flow-based style:
 *  - all flowId are defined by the parameter FLOW_IDS_PARAM --> expects a vector<string>
 *  - nodeNames are defined by the parameter <flowId>.ROUTE_NAMES_PARAM --> expects a vector<string>
 *  - outPorts are defined by the parameter <flowId>.ROUTE_OUTPORTS_PARAM --> expects a vector<int>
 */
class FlowRoutingTable {
#define FLOW_IDS_PARAM "FlowIds"
#define ROUTE_NAMES_PARAM ".route"
#define ROUTE_INPORTS_PARAM ".route.inPorts"
#define ROUTE_OUTPORTS_PARAM ".route.outPorts"

	// TODO: check if int can be updated to int32 so save space
	typedef int OutPort ;
	typedef int InPort;
	typedef std::string RouterId;
	typedef std::string FlowId;
	typedef std::unordered_map<InPort, std::unordered_map<FlowId, OutPort>> RouterTable;
	typedef std::unordered_map<RouterId, RouterTable> FlowTable;

	/** Static methods/fields to read route definitions  **/
private:

	static FlowTable RoutingTable; // {NodeName, FlowId} --> outgoingPort

	static void defineRoutes();
public:
	/** returns the outgoing port for a given server+flow (used by routers)**/
	static int getOutgoingPort(const std::string& nodeName, const std::string& flowId, int inputPort);

	/** returns the demultiplexnig outgoing port. Used by demultiplexers to know which port to use per each flow**/
	static int getDemultiplexingPort(const std::string& flowId );

	/** returns the full table for the given router (used by routers)**/
	static RouterTable getRouterTable(const std::string& nodeName);
};



#endif /* FlowRoutingTable_H_ */
