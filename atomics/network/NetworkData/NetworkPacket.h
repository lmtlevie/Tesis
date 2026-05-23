#if !defined NETWORK_PACKET
#define NETWORK_PACKET

#include <stdlib.h>
#include <string>
#include <list>
#include <map>

#include "Protocol/IProtocol.h"
#include "sinks/ParameterReader.h"

#include "signal_bus/net_fluid_flow/IFlow.h"

class NetworkPacket :  public IFlow {
private:
	// to create unique packet IDs
	static uint nextPacketId;
	static uint getNextPacketID() { return nextPacketId++; }

	uint packet_id = 0;
	double birthTime = -1;
	uint length_bits = 0; // internal counter calculated based on the protocols metadata OVERHEAD and PAYLOADSIZE_BITS
	std::map<int, std::shared_ptr<IProtocol> > protocols;

public:
	std::shared_ptr<void> payload; // can be set to a variable of any type (needs to be casted afterwards).
	int payload_size_bits = 0; // size of the payload (added to the total  length_bits of the packet)

	// TODO: temporary fields added for compatibility with Packet from packettool.h. Need to review if they can be removed or moved somewhere else more elegant (like TCP protocol)
	double hybridQDelay = -1; // This is set by the hybridReadQDelay and used (and reset to -1) by the hybridApplyQDelay. TODO: find a more elegant way to pass this information, not in the packet
	uint auxiliar_SEQ = 0; // First valid SEQ is 1. TODO: this is used in tcpsnd, move to TCP protocol.
	double lastModifTime=-1; // TODO: this is used in tcpsnd
	double currentReceptionTimestamp =-1, lastQueueSizeAfterLeaving = -1; // used by packetqueue and tcpsnd
	double ComplexityFactor=-1; // TODO: this is used in tcprcv


	NetworkPacket(double t, const std::string& flowId) : NetworkPacket(t, 0, flowId) {	}

	NetworkPacket(double t, int payload_size_bits, const std::string& flowId) : IFlow(flowId){
			this->packet_id = NetworkPacket::getNextPacketID();
			this->birthTime = t;
			this->length_bits = payload_size_bits;
			this->payload_size_bits = payload_size_bits;
	}

	~NetworkPacket() {
		//printLog(10, "Packet Destroyed (pointer=%i) \n", this);
	}

	/* creates a new object same as the other.
	 * Needed for clients to send copies of packets and retain a copy to be resent if needed (can not keep pointer to packet, as the packet might change along the way)
	 * NOTE: New packet object will have same ID as the other packet too.
	 * */
	NetworkPacket* clone() const;

	uint getId(){ return this->packet_id; }
	double getBirthTime(){ return this->birthTime;	}

	std::shared_ptr<IProtocol> getProtocol(int protocolId);
	std::vector<std::shared_ptr<IProtocol>> getProtocols(); 
	void removeProtocol(int protocolId);
	void addProtocol(const std::shared_ptr<IProtocol> protocol);
	bool hasProtocol(int protocolId);

	uint getLength_bits() { return this->length_bits; } // in bits
	uint getLength_bytes() {
		uint bytes = this->length_bits >> 3;
		return (this->length_bits % 8 == 0)? bytes: bytes+1 ;
	} // in bytes

	void printInfo(int level);
};


#endif
