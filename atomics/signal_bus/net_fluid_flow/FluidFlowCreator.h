//CPP:signal_bus/net_fluid_flow/FluidFlowCreator.cpp
//CPP:signal_bus/net_fluid_flow/FluidFlow.cpp
//CPP:signal_bus/IEntity.cpp
#if !defined FluidFlowCreator_h
#define FluidFlowCreator_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <math.h>       /* isinf */

#include "qss/qsstools.h"

#include "sinks/ParameterReader.h"
#include "general/BaseSimulator.h"
#include "signal_bus/net_fluid_flow/FluidFlow.h"


/**
 * Builds the initial flow
 */
class FluidFlowCreator: public BaseSimulator {

	// parameters
	std::string flowId;

	// state variables
	double departureRate[10]={0};
	double dropsRate[10]={0};
	double delay[10]={0};

	std::shared_ptr<FluidFlow> flow; // flow to be send

public:
	FluidFlowCreator(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
