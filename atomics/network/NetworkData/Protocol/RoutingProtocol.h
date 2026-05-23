#if !defined RoutingProtocol_H
#define RoutingProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"
#include "network/NetworkData/Route.h"

class RoutingProtocol : public IProtocol {
	std::shared_ptr<Route> route;

public:

	RoutingProtocol(std::shared_ptr<Route> route) : route(route){
	}

	RoutingProtocol(std::deque<Route::Node> route) : route(std::make_shared<Route>(std::move(route))){
	}

	uint getSize_bits() const { return 0; }
	int getLayer() const { return LAYER_ROUTING;}
	uint getId() const{ return PROTOCOL_ID_ABSTRACT_ROUTING; }
	IProtocol* clone() const ;

	std::shared_ptr<Route> getRoute() { return this->route; }

	void printInfo(int level){
		IProtocol::printInfo(level);

		this->route->printInfo(level);
	}

};


#endif
