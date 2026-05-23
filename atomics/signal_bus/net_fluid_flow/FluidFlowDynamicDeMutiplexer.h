#if !defined FluidFlowDynamicDeMutiplexer_h
#define FluidFlowDynamicDeMutiplexer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <math.h>       /* isinf */
#include "boost/format.hpp"

#include "sinks/ParameterReader.h"
#include "general/BaseSimulator.h"
#include "signal_bus/net_fluid_flow/IFlow.h"


/**
 * Demultiplexes multiple flows (fluid or discrete), each into its own port.
 * At the input port, receives multiple flows. Each flowID is sent out using a different output port.
 * First ID to arrives will always go through port0, 2nd ID to arrive will always go through port1, etc
 *
 * TODO: rename FluidFlowDynamicDeMutiplexer->FlowDynamicDeMutiplexer
 */
class FluidFlowDynamicDeMutiplexer: public BaseSimulator {

// Parameters
int maxFlows;

// state variables
std::map<std::string, int> flowId2port; // mapping of flow IDs to ports
std::deque<std::shared_ptr<IFlow>> pendingFlows; // pending flows to forward

public:
	FluidFlowDynamicDeMutiplexer(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
