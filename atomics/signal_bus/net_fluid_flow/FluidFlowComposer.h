//CPP:signal_bus/net_fluid_flow/FluidFlowComposer.cpp
//CPP:signal_bus/net_fluid_flow/FluidFlow.cpp
//CPP:signal_bus/IEntity.cpp
#if !defined FluidFlowComposer_h
#define FluidFlowComposer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <math.h>       /* isinf */

#include "qss/qsstools.h"

#include "sinks/ParameterReader.h"
#include "general/BaseSimulator.h"
#include "signal_bus/net_fluid_flow/FluidFlow.h"


/**
 * Groups all attributes and forwards the flow when one of the attributes is updated
 */
class FluidFlowComposer: public BaseSimulator {

// state variables: attributes for the flow
double* rate;
double* dropsRate;
double* acumDelay;
std::string id="";

std::shared_ptr<FluidFlow> flow; // flow to forward

public:
	FluidFlowComposer(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
