#ifndef RouterTable_H_
#define RouterTable_H_

#include <memory>
#include <stdlib.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

#include "FlowRoutingTable.h"
#include "sinks/ParameterReader.h"

using namespace std;

/**
 * Represents the routing table at each router. Defines of flow routes both for packet-level and fluid-flow
 * It gets built (at constructor) by reading the configuration (getCmdLineParam) using the following params:
 *  - <routerName>.flows --> expects a vector<string>
 *  - <routerName>.<flowName>.inPorts --> expects a vector<int> of same size as outports
 *  - <routerName>.<flowName>.outPorts --> expects a vector<int> of same size as inports
 *
 * The table that relates <inputPort, FlowId> with a <outPort>
 *
 */
class RouterTable {
#define FLOWS_PARAM ".flows"
#define INPORTS_PARAM ".inPorts"
#define OUTPORTS_PARAM ".outPorts"
#define	WILD_CARD_INPORT -1 // use to specify any incoming port
#define	WILD_CARD_FLOW "*" // use to specify any incoming flow ID

	// TODO: check if int can be updated to int32 so save space
	typedef int OutPort ;
	typedef int InPort;
	typedef std::string RouterId;
	typedef std::string FlowId;
	typedef std::unordered_map<InPort, std::unordered_map<FlowId, OutPort>> Table;
	//	typedef std::unordered_map<Key, OutPort> Table;

private:
	Table table;
	RouterId routerId;

public:
	RouterTable (const RouterId& routerId) : routerId(routerId){}

	void buildFromConfig();

	/** returns the outgoing port for a given server+flow (used by routers)**/
	int getOutgoingPort(const FlowId& flowId, InPort inputPort);
};
#endif /* RouterTable_H_ */
