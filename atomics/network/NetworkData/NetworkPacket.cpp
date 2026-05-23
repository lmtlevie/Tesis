#include "NetworkPacket.h"

uint NetworkPacket::nextPacketId = 1;

NetworkPacket* NetworkPacket::clone() const {
	NetworkPacket* other = new NetworkPacket(*this);

	// deep copy protocols (the copy constructor makes a copy of the shared_ptr, we need a full new object)
	other->protocols.clear();
	for(auto elem : this->protocols){
		other->protocols[elem.first] = std::shared_ptr<IProtocol>(elem.second->clone());
	}

	return other;
}

void NetworkPacket::removeProtocol(int protocolId) {
	auto protoIt = this->protocols.find(protocolId);
	if(protoIt != this->protocols.end()){
		this->length_bits -= protoIt->second->getSize_bits(); // substract old size
		this->protocols.erase(protocolId);
	}
}

void NetworkPacket::addProtocol(const std::shared_ptr<IProtocol> protocol) {
	this->length_bits += protocol->getSize_bits(); // add new size
	auto protoIt = this->protocols.find(protocol->getId());
	if(protoIt == this->protocols.end()){
		this->protocols[protocol->getId()] = protocol;
	} else {
		this->length_bits -= protoIt->second->getSize_bits(); // substract old size
		protoIt->second = protocol;
	}
}

bool NetworkPacket::hasProtocol(int protocolId) {
    auto it = this->protocols.find(protocolId);
    return (it != this->protocols.end());
}

std::shared_ptr<IProtocol> NetworkPacket::getProtocol(int protocolId) {
	auto it = this->protocols.find(protocolId);
	if (it == this->protocols.end() )
	{
		printLog(LOG_LEVEL_ERROR, "NetworkPacket::getProtocol: Unable to find protocol: %d \n ", protocolId);
		this->printInfo(LOG_LEVEL_ERROR);
		throw std::runtime_error("NetworkPacket::getProtocol: Unable to find protocol:"+ std::to_string(protocolId));
	}

	return it->second;
}

std::vector<std::shared_ptr<IProtocol>> NetworkPacket::getProtocols() {
	std::vector<std::shared_ptr<IProtocol>> ret;
	for (auto protocolIt : this->protocols){
		ret.push_back(protocolIt.second);	
	}

	return ret;
}


void NetworkPacket::printInfo(int level) {
    if (LOG_LEVEL >= level) { // TODO: update it for readDebugLevel(this) > level
        printLog(level, "-------    Information for Packet #%u: ----------------\n", this->packet_id);
        IFlow::printInfo(level);
        printLog(level, "ID= #%u \n", this->packet_id);
        printLog(level, "birthTime= %.16f \n", this->birthTime);
        printLog(level, "getLength()= %u bits (%u Bytes)\n", this->getLength_bits(), this->getLength_bytes());
        printLog(level, "---- Additional Fields ----- \n", this->auxiliar_SEQ);
        printLog(level, "auxiliar_SEQ = %u \n", this->auxiliar_SEQ);
        printLog(level, "lastModifTime= %f \n", this->lastModifTime);
        printLog(level, "currentReceptionTimestamp= %f \n", this->currentReceptionTimestamp);
        printLog(level, "lastQueueSizeAfterLeaving= %f \n", this->lastQueueSizeAfterLeaving);
        printLog(level, "ComplexityFactor= %f \n", this->ComplexityFactor);
        printLog(level, "hybridQDelay= %f \n", this->hybridQDelay);

        printLog(level, "Protocols (%u): \n", (int) this->protocols.size());
        for (auto protocolIt : this->protocols) {
            protocolIt.second->printInfo(level);

            printLog(level, "\t-------------------  \n");
        }

        printLog(level, "-------    END packet Information  ----------------\n", this->packet_id);
    }
}

