#if !defined QualityOfServiceProtocol_H
#define QualityOfServiceProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

class QualityOfServiceProtocol : public IProtocol {
	int typeOfService;

public:

	QualityOfServiceProtocol(int typeOfService) : typeOfService(typeOfService) {

	}

	uint getSize_bits() const { return 0; } ;
	int getLayer() const { return LAYER_ROUTING;};
	uint getId() const { return PROTOCOL_ID_QUALITY_OF_SERVICE; };
	IProtocol* clone() const { return new QualityOfServiceProtocol(*this); }



	int getTypeOfService() { return this->typeOfService; };
};


#endif

