#if !defined IProtocol_H
#define IProtocol_H

#include <stdlib.h>
#include <string>

#include "sinks/ParameterReader.h"

// Known implemented Protocol IDs (classes that inherit from Protocol)
#define PROTOCOL_ID_UNDEFINED -1
#define PROTOCOL_DUMMY 2
#define PROTOCOL_ID_FELIX_APP 700
#define PROTOCOL_ID_ABSTRACT_ROUTING 300
#define PROTOCOL_ID_TCP_IP 350
#define PROTOCOL_ID_TCP_PACKETIZATION 351
#define PROTOCOL_ID_QUALITY_OF_SERVICE 400 		// QualityOfServiceProtocol.h

// Layer IDS
#define LAYER_APPLICATION 7
#define LAYER_ROUTING 3

class IProtocol {
public:
 virtual ~IProtocol() {}

 virtual uint getSize_bits() const=0;
 virtual int getLayer() const=0;
 virtual uint getId() const=0;

 /* creates a new object same as the other.
 This is to support deep copy in NetworkPacket
* */
 virtual IProtocol* clone() const=0;

 virtual void printInfo(int level){
 	printLog(level, "\t Protocol ID= %u \n", this->getId());
 	printLog(level, "\t Protocol layer= %u \n", this->getLayer());
 	printLog(level, "\t Protocol size= %u (bits) \n", this->getSize_bits());
 }


};


#endif
