#if !defined TcpIpProtocol_H
#define TcpIpProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

/**
 * Protocol representing TCP IP
 * The fields and implementation are taken from packetool to keep it compatible
 */
class TcpIpProtocol : public IProtocol {
	uint size_bits = 0;

	bool markCongestion = false; // used to support ECN (red, tcpSnd, tcpRcv, etc)

	// these are taken from PacketTool
	std::string payload;
	uint ACK;
	std::string ipSrc;
	std::string ipDst;
	int portSrc;
	int portDst;
public:

	static int TCP_IP_OVERHEAD_BITS;

	TcpIpProtocol(uint payload_bits, const std::string& ipSrc, const std::string& ipDst, int portSrc, int portDst) :
		size_bits(payload_bits + TCP_IP_OVERHEAD_BITS),
		ACK(-1),  // warning ACK=-1=4294967295 (unsigned)
		ipSrc(ipSrc),
		ipDst(ipDst),
		portSrc(portSrc),
		portDst(portDst){

	}

	uint getSize_bits() const { return size_bits; } ;
	int getLayer() const { return LAYER_ROUTING;};
	uint getId() const { return PROTOCOL_ID_TCP_IP; };
	IProtocol* clone() const { return new TcpIpProtocol(*this); }

	std::string getPayload() { return this->payload;};
	uint getACK() { return this->ACK;};
	std::string getIpSrc() { return this->ipSrc ;};
	std::string getIpDst() { return this->ipDst;};
	int getPortSrc() { return this->portSrc;};
	int getPortDst() { return this->portDst;};
	bool getMarkCongestion() { return this->markCongestion;};

	void setPayload(const std::string& payload) { this->payload = payload;};
	void setACK(uint ack) { this->ACK=ack;};
	void setMarkCongestion(bool mark) { this->markCongestion=mark;};

	void printInfo(int level){
		IProtocol::printInfo(level);

		printLog(level, "\t ACK = %u \n", this->getACK());
		printLog(level, "\t IP_SRC:port = %s:%d \n", this->getIpSrc().data(), this->getPortSrc());
		printLog(level, "\t IP_DST:port = %s:%d \n", this->getIpDst().data(), this->getPortDst());
	}

};


#endif

