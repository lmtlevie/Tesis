#include "packetqueue_simple.h"
void packetqueue_simple::init(double t,...) {
    BaseSimulator::init(t);
    //    std::cout << "Simple queue" << std::endl;

    va_list parameters;
    va_start(parameters,t);

    char *fvar;
    fvar = va_arg(parameters,char*);

    // There are parameters for the queue, it is not a default infinite capacity queue
    maxCapacity_bits = readDefaultParameterValue<long>(fvar);
    maxCapacity_bits = maxCapacity_bits<0? std::numeric_limits<long>::max(): maxCapacity_bits;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: MaxCapacity: %i bits \n", this->getFullName().data(), maxCapacity_bits);

    fvar = va_arg(parameters,char*);
    ForcedPeriod = readDefaultParameterValue<double>(fvar);
    ForcedPeriod = ForcedPeriod<0? std::numeric_limits<double>::infinity(): ForcedPeriod;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: ForcedPeriod: %g \n", this->getFullName().data(), ForcedPeriod);


    sigma=ForcedPeriod;

    this->logger->initSignals(std::map<std::string, int>{
        {"waittime", LOG_LEVEL_IMPORTANT},
        {"qSize_bits", LOG_LEVEL_IMPORTANT},
        {"qSize_elems", LOG_LEVEL_PRIORITY},
        {"discard_bits", LOG_LEVEL_IMPORTANT},
        {"discard_elems", LOG_LEVEL_PRIORITY},
        {"discard_bits1", LOG_LEVEL_PRIORITY}, // easy workaround to log FLOW 1 discards
        {"discard_bits2", LOG_LEVEL_PRIORITY}, // easy workaround to log FLOW 2 discards
        {"discard_bits3", LOG_LEVEL_PRIORITY}, // easy workaround to log FLOW 2 discards
        {"elemSize", LOG_LEVEL_DEBUG},
        {"dequeRequests", LOG_LEVEL_DEBUG},
        {"arrivedId", LOG_LEVEL_DEBUG},
        {"discardedId", LOG_LEVEL_DEBUG},
        {"sentId", LOG_LEVEL_DEBUG},
    });

    //	serverstate=SERVERBUSY;
    serverstate=SERVERWAITING; // assume server is already waiting (no need for a first request, allows to connect output of server with port1 of the queue). See commit: https://gitlab.cern.ch/tdaq-simulation/powerdevs/commit/e1297e571aacb64bd3eb868c537e1195c821a6c0

    return;
}

void packetqueue_simple::dint(double t) {
    return; // Everything done in lambda
}
void packetqueue_simple::dext(Event x, double t) {
    //debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: dext - x.port=%g\n",t,x.port);
    if (x.port==0) {	    // Packet Arrived
        auto p = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
        int packetSize = p->getLength_bits();
        //		myprotoStack = p->ProtocolList; TODO: Matias removed it to migrate to NetworkPacket, was it needed?

        this->logger->logSignal(t, packetSize, "elemSize");
        this->logger->logSignal(t, p->getId(), "arrivedId");

        if (this->qSize_bits + packetSize > maxCapacity_bits) { // Queue Full: Rejects
            debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s: Incoming Packet #%i(flow=%s) Refused qSize=%ld bits (MaxCapacity %ld bits reached) \n", t, this->getFullName().data(), p->getId(), p->flowId.value.data(), this->qSize_bits, maxCapacity_bits);
            this->logger->logSignal(t, packetSize, "discard_bits");
            this->logger->logSignal(t, 1, "discard_elems");
            this->logger->logSignal(t, packetSize, "discardedId");

            //TODO: hardcoded flow IDs for logging purposes directly in the atomic
            // to do it properly: send the discarded packets by a new outport and do proper connections from the coupled model
//            try{
//                auto protocol = p->getProtocol(PROTOCOL_ID_TCP_IP);
//                std::string ip = "";
//                if(protocol){
//                    ip = std::static_pointer_cast<TcpIpProtocol>(protocol)->getIpSrc();
//                }
//
//                if(strcmp("FLOWID_1", p->flowId.value.data()) == 0 || strcmp("TCP_FLOWID_1", p->flowId.value.data()) == 0 ||  strcmp("DiscreteTcpSender1", ip.data()) == 0 ){
//                    this->logger->logSignal(t, packetSize, "discard_bits1");
//                }
//                if(strcmp("FLOWID_2", p->flowId.value.data()) == 0 || strcmp("TCP_FLOWID_2", p->flowId.value.data()) == 0 ||  strcmp("DiscreteTcpSender2", ip.data()) == 0 ){
//                    this->logger->logSignal(t, packetSize, "discard_bits2");
//                }
//                if(strcmp("FLOWID_3", p->flowId.value.data()) == 0 || strcmp("TCP_FLOWID_3", p->flowId.value.data()) == 0 ||  strcmp("DiscreteTcpSender3", ip.data()) == 0 ){
//                    this->logger->logSignal(t, packetSize, "discard_bits3");
//                }
//            } catch (const std::runtime_error& error) {
//                debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Unable to find TCP protocol in packet #%i. It was just for logging, so skipping\n", t, this->getFullName().data());
//            }

            sigma=sigma-e;
            return;
        }

        // Otherwise : Accepts
        p->currentReceptionTimestamp = t;
        mypacketqueue.push(p);
        this->qSize_bits += packetSize;
        this->logger->logSignal(t, (double) mypacketqueue.size(), "qSize_elems");
        this->logger->logSignal(t, (double) qSize_bits,"qSize_bits");

        sigma=0;
        debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[ExT]: enqueuing Packet #%i(flow=%s). Queue Size (%u). - t_NOTIFYING immediately.\n",t, this->getFullName().data(), p->getId(),p->flowId.value.data(), (int)mypacketqueue.size());
    }

    if (x.port==1) {		// Request from Server Arrived
        this->logger->logSignal(t, 1, "dequeRequests");
        //debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue... \n",t, this->getFullName().data());
        if ((int)mypacketqueue.size()>0){ // Queue is not empty
            debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue (to t_READYTOEMIT) - Non Empty Queue... \n",t, this->getFullName().data());

            serverstate=SERVERWAITING;
            sigma=0; // Force dequeueing next available element at the head of the queue;
        } else {
            debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue (to t_READYTOEMIT) - Empty Queue... \n",t, this->getFullName().data());

            serverstate=SERVERWAITING;
            sigma=sigma-e; // Continues waiting for a new packet
        }
    }
    return;
}

Event packetqueue_simple::lambda(double t) {
    int qz = (int) mypacketqueue.size();

    if (serverstate==SERVERWAITING && qz >0) {
        //debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: mystate=t_READYTOEMIT(yes) \n",t);
        // Must emit a packet
        //debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: Trying to DEQUEUE... \n",t);
        auto pout = mypacketqueue.front();
        mypacketqueue.pop();
        this->qSize_bits -= pout->getLength_bits();

        pout->lastQueueSizeAfterLeaving=qz; // TODO: Matias: can 'lastQueueSizeAfterLeaving' be removed?
        //debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: Dequeueing Packet ID %u \n",t, pout->getId());
        double waittime = t - pout->currentReceptionTimestamp; // TODO: Matias: can 'currentReceptionTimestamp' be removed?

        this->logger->logSignal(t,waittime,"waittime");
        this->logger->logSignal(t,pout->getId(),"sentId");

        // set next state
         serverstate=SERVERBUSY;
         sigma=ForcedPeriod;

        debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[Out]:  dequeue Packet ID(#%u). NOW QUEUE SIZE(%u)\n", t, this->getFullName().data(), pout->getId(), (int) mypacketqueue.size());
        return Event(pout,0);
    } else {
    	// set next state
     	sigma=ForcedPeriod;
        debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: t_READYTOEMIT but mypacketqueue.size = 0 (=%u) or server not waiting\n", t, this->getFullName().data(), mypacketqueue.size());
        return Event(); //  Matias fix: original: //return Event(&sizeout,1); //original
    }


    // should never arrive here
    throw std::runtime_error("packetqueue_simple::lambda - Unknown state. Reach end of lambda without setting output event \n");
    return Event();
}
