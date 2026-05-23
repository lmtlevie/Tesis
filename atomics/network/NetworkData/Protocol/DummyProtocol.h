#if !defined DummyProtocol_H
#define DummyProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

/**
 * Dummy protocol with just a size
 */
class DummyProtocol : public IProtocol {
	uint size_bits = 0;
public:

	DummyProtocol(int size_bits) :
		size_bits(size_bits){
	}

	uint getSize_bits() const { return size_bits; } ;
	int getLayer() const { return LAYER_APPLICATION;};
	uint getId() const { return PROTOCOL_DUMMY; };
	IProtocol* clone() const { return new DummyProtocol(*this); }

	void setSize_bits(uint size) { this->size_bits = size;};

};


#endif

