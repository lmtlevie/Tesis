#if !defined TcpPacketizationProtocol_H
#define TcpPacketizationProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"
#include "network/NetworkData/NetworkPacket.h"

// Use (void) to silence unused warnings
#define assertm(exp, msg) assert(((void)msg, exp))

/**
 * Protocol to implement TCP packetization
 * This protocol allows to set an arbitrary size and encapsulate the original packet without accounting for its size 
 */
class TcpPacketizationProtocol : public IProtocol {
	uint size_bits = 0;
	uint currentFragment = 0;
	uint totalFragments = 0;	
	std::shared_ptr<NetworkPacket> originalPacket; 
	
public:

	static int TCP_IP_OVERHEAD_BITS;

	TcpPacketizationProtocol(uint size_bits, uint fragmentNumber, uint totalFragments, std::shared_ptr<NetworkPacket> originalPacket) :
		size_bits(size_bits),
		currentFragment(fragmentNumber),
		totalFragments(totalFragments),
		originalPacket(originalPacket){
			assertm(currentFragment < totalFragments, "Fragment numbers should be 0-based");// 
	}

	uint getSize_bits() const { return size_bits; } ;	
	int getLayer() const { return LAYER_ROUTING;};
	uint getId() const { return PROTOCOL_ID_TCP_PACKETIZATION; };
	IProtocol* clone() const { return new TcpPacketizationProtocol(*this); }
	
	uint getTotalFragments() const { return totalFragments; } ;
	uint getCurrentFragment() const { return currentFragment; } ;
	uint isLastFragment() const { return currentFragment == totalFragments - 1;  } ; // 0-based
	std::shared_ptr<NetworkPacket> getOriginalPacket() { return originalPacket;};


	void printInfo(int level){
		IProtocol::printInfo(level);
		printLog(level, "Protocol wrapping fragmented packet (due to TCP packetization)\n");
		printLog(level, "\t totalFragments = %u \n", this->getTotalFragments());
		printLog(level, "\t currentFragment = %u \n", this->getCurrentFragment());
		printLog(level, " Original packet :\n");
		this->originalPacket->printInfo(level);
	}

};


#endif

