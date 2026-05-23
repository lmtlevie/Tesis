//CPP:signal_bus/net_fluid_flow/FluidFlow.cpp
#if !defined FluidFlow_H
#define FluidFlow_H

#include <stdlib.h>
#include <string>

#include "signal_bus/net_fluid_flow/IFlow.h"


class FluidFlow :  public IFlow {
public:
	// QSS signals
	Attribute<double*> rate;
	Attribute<double*> acumDrops;

	// Other attributes
	Attribute<double*> acumDelay;

    // all attributes values are set, but start as not updated. If any attribute is updated has to be marked separately
	FluidFlow(const std::string& flowId, double* rate, double* drops, double* accumDelay) : IFlow(flowId),
	        rate(rate),
	        acumDrops(drops),
	        acumDelay(accumDelay){
	}

	void printInfo(int level){
		IEntity::printInfo(level);

		printLog(level, "\t rate[0]=%g ;rate[1]=%g ;rate[2]=%g ;rate[3]=%g; (updated:%d) ;  \n", rate.value[0], rate.value[1], rate.value[2], rate.value[3], rate.updated);
		printLog(level, "\t drops[0]=%g ;drops[1]=%g ;drops[2]=%g ;drops[3]=%g ;  (updated:%d)\n", acumDrops.value[0], acumDrops.value[1], acumDrops.value[2], acumDrops.value[3], acumDrops.updated);
		printLog(level, "\t acumDelay=%g (updated:%d) \n", acumDelay.value, acumDelay.updated);
	}

};


#endif
