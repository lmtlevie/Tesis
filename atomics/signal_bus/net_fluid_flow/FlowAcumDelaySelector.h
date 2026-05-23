//CPP:signal_bus/net_fluid_flow/FlowAcumDelaySelector.cpp
//CPP:signal_bus/net_fluid_flow/FluidFlow.cpp
//CPP:signal_bus/IEntity.cpp
#if !defined FlowAcumDelaySelector_h
#define FlowAcumDelaySelector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <math.h>       /* isinf */

#include "qss/qsstools.h"

#include "sinks/ParameterReader.h"
#include "general/BaseSimulator.h"
#include "signal_bus/net_fluid_flow/FluidFlow.h"


/**
 * Outputs only the rate(double[10]) from the flow
 */
class FlowAcumDelaySelector: public BaseSimulator {

// state variables
double* acumDelay;
std::shared_ptr<FluidFlow> flow; // keep a reference to the last flow


public:
	FlowAcumDelaySelector(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
