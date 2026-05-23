#if !defined FelixAppProtocol_H
#define FelixAppProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

class FelixAppProtocol : public IProtocol {
	uint size_bits = 0;

public:

	FelixAppProtocol(uint size_bits) : size_bits(size_bits){

	}

	uint getSize_bits() { return size_bits; } ;
	int getLayer() { return LAYER_APPLICATION;};
	uint getId() { return PROTOCOL_ID_FELIX_APP; };
	IProtocol* clone() const { return new FelixAppProtocol(*this); }
};


#endif
