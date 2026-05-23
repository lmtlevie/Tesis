#if !defined IFlow_H
#define IFlow_H

#include <stdlib.h>
#include <string>

#include "signal_bus/IEntity.h"

/*
 * Common class to use between packet-level packets and fluid-flow flows. Allows reusing the router for example
 */
class IFlow : public IEntity {
public:
	// attributes
    Attribute<std::string> flowId; // can be potentially different than entity ID (flowID distiguishes the flows)
	Attribute<int> typeOfService;

    IFlow(const std::string& flowId) : IFlow(flowId, -1) {   }

	IFlow(const std::string& flowId, int typeOfService) : IEntity(flowId), flowId(flowId), typeOfService(typeOfService) {
	}

	void printInfo(int level){
		IEntity::printInfo(level);

		printLog(level, "\t flowID=%s; (updated:%d) ;  \n", this->flowId.value.data(), this->flowId.updated);
		printLog(level, "\t typeOfService=%i ;  (updated:%d)\n", this->typeOfService.value, this->typeOfService.updated);
	}

};


#endif
