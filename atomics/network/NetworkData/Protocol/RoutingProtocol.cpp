#include "RoutingProtocol.h"

IProtocol* RoutingProtocol::clone() const {
	RoutingProtocol* other = new RoutingProtocol(*this);

	// deep copy the route
	other->route = std::shared_ptr<Route>(new Route(*this->route));

	return other;
}
