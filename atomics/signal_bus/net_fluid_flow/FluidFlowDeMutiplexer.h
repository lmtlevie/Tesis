#if !defined FluidFlowDeMutiplexer_h
#define FluidFlowDeMutiplexer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <math.h>       /* isinf */
#include "boost/format.hpp"

#include "sinks/ParameterReader.h"
#include "general/BaseSimulator.h"
#include "signal_bus/net_fluid_flow/FluidFlow.h"



/**
 * Demultiplexes multiple fluid flows, each into its own port
 * At the input port  receives multiple flows. Each flowID is sent out using a the port as set in the parameters.
 */
class FluidFlowDeMutiplexer: public BaseSimulator {

static constexpr int MAX_FLOW_IDS = 2;


std::string flowIds[MAX_FLOW_IDS]; // TODO: now the queue accepts maximum 2 flows, but in the future we might need more
std::deque<std::shared_ptr<FluidFlow>> pendingFlows; // pending flows to forward

public:
	FluidFlowDeMutiplexer(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
