#include "packetqueue.h"
void packetqueue::init(double t,...) {
	BaseSimulator::init(t);

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


	for (int i=0;i<10;i++) {
		sizeout[i]=0;}

	this->logger->initSignals(std::map<std::string, int>{
		{"waittime", LOG_LEVEL_IMPORTANT},
		{"qSize_bits", LOG_LEVEL_IMPORTANT},
		{"qSize_elems", LOG_LEVEL_PRIORITY},
		{"discard_bits", LOG_LEVEL_ALWAYS},
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

	mystate=WAITING;
//	serverstate=SERVERBUSY;
	serverstate=SERVERWAITING; // assume server is already waiting (no need for a first request, allows to connect output of server with port1 of the queue). See commit: https://gitlab.cern.ch/tdaq-simulation/powerdevs/commit/e1297e571aacb64bd3eb868c537e1195c821a6c0

	return;
}

void packetqueue::dint(double t) {
	switch (mystate) {
	case WAITING:
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: dinit - mystate=WAITING",t);
		serverstate=SERVERWAITING;
		mystate=t_NOTIFYING;
		sigma=0;

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Int]: WAITING to t_NOTIFYING\n",t, this->getFullName().data());
		return;

	case t_NOTIFYING:  // Notified length, now emit
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Int]: t_NOTIFYING to t_READYTOEMIT\n",t, this->getFullName().data());

		mystate=t_READYTOEMIT;
		sigma=0;
		return ;

	case t_READYTOEMIT:
		//if(Q_Record_log)
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Int]: mystate=t_READYTOEMIT\n",t, this->getFullName().data());
		// Emitted a packet, now notify length again
		if (serverstate==SERVERWAITING && (int)myPacketQueue.size()>0) {
			auto p = myPacketQueue.front();
			myPacketQueue.pop();
			this->qSize_bits -= p->getLength_bits();

			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue:[Int] DEQUEUED SIZE: %u \n",t, (int) myPacketQueue.size());
			serverstate=SERVERBUSY;
			mystate=t_NOTIFYING;
			sigma=0;

			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Int]: t_READYTOEMIT to t_NOTIFYING (pop->size(%u))\n",t, this->getFullName().data(),myPacketQueue.size());
		} else {
			// Nothing else to do, go back waiting
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Int]: (Queue.size=%d)t_READYTOEMIT to WAITING(%g)\n",t, this->getFullName().data(),myPacketQueue.size(), ForcedPeriod);

			mystate=WAITING;
			sigma=ForcedPeriod;
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: Go to wait again \n",t);
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: dinit - mystate=t_READYTOEMIT(else) salgo\n",t);
		}
	}
	return;
}
void packetqueue::dext(Event x, double t) {
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
//			try{
//			    auto protocol = p->getProtocol(PROTOCOL_ID_TCP_IP);
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
//			} catch (const std::runtime_error& error) {
//			    debugMsg(LOG_LEVEL_ERROR, "[%g] %s: Unable to find TCP protocol in packet #%i. It was just for logging, so skipping\n", t, this->getFullName().data());
//            }

			mystate=WAITING;
			sigma=sigma-e;
			return;
		}

		// Otherwise : Accepts
		p->currentReceptionTimestamp = t;
		myPacketQueue.push(p);
		this->qSize_bits += packetSize;

	    mystate=t_NOTIFYING;
		sigma=0;
		debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[ExT]: enqueuing Packet #%i(flow=%s). Queue Size (%u). - t_NOTIFYING immediately.\n",t, this->getFullName().data(), p->getId(),p->flowId.value.data(), (int)myPacketQueue.size());
	}

	if (x.port==1) {		// Request from Server Arrived
		this->logger->logSignal(t, 1, "dequeRequests");
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue... \n",t, this->getFullName().data());
		if ((int)myPacketQueue.size()>0){ // Queue is not empty
		    debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue (to t_READYTOEMIT) - Non Empty Queue... \n",t, this->getFullName().data());

			serverstate=SERVERWAITING;
			mystate=t_READYTOEMIT;
			sigma=0; // Force dequeueing next available element at the head of the queue;
		} else {
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue (to t_READYTOEMIT) - Empty Queue... \n",t, this->getFullName().data());

			serverstate=SERVERWAITING;
			mystate=t_READYTOEMIT;
			sigma=sigma-e; // Continues waiting for a new packet
		}
	}
	return;
}

Event packetqueue::lambda(double t) {
	int qz = (int) myPacketQueue.size();

	switch (mystate) {
	case WAITING:  // Forced Period Elapsed
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: mystate=WAITING \n",t);
		return Event(); // Sends Nothing
		break;
	case t_NOTIFYING:  // Must notify new queue length
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue:[Out] t_NOTIFYING(qz) Notifying QUEUE SIZE: %d \n",t,qz);
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: mystate=t_NOTIFYING \n",t);
		this->logger->logSignal(t,(double)qz,"qSize_elems");
		this->logger->logSignal(t,(double)qSize_bits,"qSize_bits");

		sizeout[0] = (double)qSize_bits; // notify in bits
		//sizeout[0] = (double)qz;  // notify in packets
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: to p1 Notifying sizeout(%g)\n",t, this->getFullName().data(),sizeout[0]);

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: t_NOTIFYING notifying QUEUE SIZE, qz: %d, sizeout: %f \n",t, this->getFullName().data(), qz,*sizeout);
		return Event(&sizeout,1); //original TODO: still sending size in packets not to break (yet) other models

		break;
	case t_READYTOEMIT:
		if (serverstate==SERVERWAITING && (int)myPacketQueue.size()>0) {
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: mystate=t_READYTOEMIT(yes) \n",t);
			// Must emit a packet
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: Trying to DEQUEUE... \n",t);
			auto pout = myPacketQueue.front();
			pout->lastQueueSizeAfterLeaving=(int)myPacketQueue.size()-1; // TODO: Matias: can 'lastQueueSizeAfterLeaving' be removed?
			//debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: Dequeueing Packet ID %u \n",t, pout->getId());
			double waittime = t - pout->currentReceptionTimestamp; // TODO: Matias: can 'currentReceptionTimestamp' be removed?

			this->logger->logSignal(t,waittime,"waittime");
			this->logger->logSignal(t,pout->getId(),"sentId");

			debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[Out]:  dequeue Packet ID(#%u). NOW QUEUE SIZE(%u)\n", t, this->getFullName().data(), pout->getId(), (int) myPacketQueue.size());
			return Event(pout,0);
		} else {
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[Out]: t_READYTOEMIT but myPacketQueue.size = 0 or server not waiting\n", t, this->getFullName().data(), sizeout[0]);
			return Event(); //  Matias fix: original: //return Event(&sizeout,1); //original
		}
		break;
	}

	// should never arrive here
	throw std::runtime_error("packetqueue::lambda - Unknown state. Reach end of lambda without setting output event \n");
	return Event();
}
