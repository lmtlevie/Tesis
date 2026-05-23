#include "packetSharedQueue.h"

void packetSharedQueue::init(double t,...) {
    BaseSimulator::init(t);
    //    std::cout << "Simple queue" << std::endl;

    va_list parameters;
    va_start(parameters,t);

    char *fvar;
    fvar = va_arg(parameters,char*);

    // There are parameters for the queue, it is not a default infinite capacity queue
    per_port_capacity_bits = readDefaultParameterValue<long>(fvar);
    per_port_capacity_bits = per_port_capacity_bits<0? std::numeric_limits<long>::max(): per_port_capacity_bits;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: per_port_capacity_bits: %ld bits \n", this->getFullName().data(), per_port_capacity_bits);

    fvar = va_arg(parameters,char*);
    shared_capacity_bits = readDefaultParameterValue<long>(fvar);
    shared_capacity_bits = shared_capacity_bits<0? std::numeric_limits<long>::max(): shared_capacity_bits;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: shared_capacity_bits: %ld bits \n", this->getFullName().data(), shared_capacity_bits);

    fvar = va_arg(parameters,char*);
    buffer_cell_size_bits = readDefaultParameterValue<long>(fvar);
    buffer_cell_size_bits = buffer_cell_size_bits<0? 1: buffer_cell_size_bits;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: buffer_cell_size_bits: %ld bits\n", this->getFullName().data(), buffer_cell_size_bits);

    fvar = va_arg(parameters,char*);
    port_max_shared_buffer_use_bits = readDefaultParameterValue<long>(fvar);
    port_max_shared_buffer_use_bits = port_max_shared_buffer_use_bits<0? std::numeric_limits<long>::max(): port_max_shared_buffer_use_bits;
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: port_max_shared_buffer_use_bits: %ld bits\n", this->getFullName().data(), port_max_shared_buffer_use_bits);


    sigma=std::numeric_limits<double>::infinity(); // wait indefinitely

    this->logger->initSignals(std::map<std::string, int>{
        {"shared_buffer_size_bits", LOG_LEVEL_IMPORTANT},
        {"max_port_shared_buffer_size_bits", LOG_LEVEL_IMPORTANT}, // maximum single port usage of shared buffer (compared to port_max_shared_buffer_use_bits)
		{"port0_buffer_size_bits", LOG_LEVEL_IMPORTANT}, // NOTE: queue0 does not necessarily match with port0 (queues are created as needed)
        {"discard_bits", LOG_LEVEL_IMPORTANT},
        {"discard_elems", LOG_LEVEL_PRIORITY},
        {"elemSize", LOG_LEVEL_DEBUG},
		// (TODO: these are only selected hardcoded ports. Its not possible to know at init the number of ports that will be used, should come as parameter)
		{"port3_buffer_size_bits", LOG_LEVEL_DEBUG},
		{"port5_buffer_size_bits", LOG_LEVEL_DEBUG},
		{"port9_buffer_size_bits", LOG_LEVEL_DEBUG},
    });

    return;
}

void packetSharedQueue::dint(double t) {
	if(pending_sends.size() > 0){
		sigma = 0; // there are sends still pending
	} else {
		sigma = std::numeric_limits<double>::infinity(); // wait indefinitely
	}

    return;
}

bool packetSharedQueue::hasBuffer(const OutputQueue& portQueue, long buffering_size) {
    bool hasPortBuffer = portQueue.port_size_bits + buffering_size <= per_port_capacity_bits;
    bool hasSharedBuffer = shared_buffer_size_bits + buffering_size <= shared_capacity_bits;
    bool hasSharedQuota = portQueue.shared_size_bits + buffering_size <= port_max_shared_buffer_use_bits;

    return hasPortBuffer || (hasSharedBuffer && hasSharedQuota);
}


void packetSharedQueue::dext(Event x, double t) {
    //debugMsg(LOG_LEVEL_DEBUG, "[%g] Queue: dext - x.port=%g\n",t,x.port);
    if (x.port % 2 == 0) {	    // Packet Arrived
    	int output_port = x.port / 2;
        auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming event
        int packet_size_bits = packet->getLength_bits();
        this->logger->logSignal(t, packet_size_bits, "elemSize");

        // we are adding a new queue, set it with a pending request
        if(this->queues.count(output_port) == 0){
        	pending_requests.insert(output_port); // assume server is already waiting (no need for a first request, allows to connect output of server with port1 of the queue)
        }
        OutputQueue& queue = this->queues[output_port]; // Adds a new OutputQueue if necessary and retrieve it

        // account for effective buffer required in terms of cells
        auto buffering_size = getBufferingSize(packet_size_bits);

        // reject?
        if (!hasBuffer(queue, buffering_size)) { 
            debugMsg(/*LOG_LEVEL_IMPORTANT*/ LOG_LEVEL_ALWAYS, "[%g] %s: Incoming Packet #%i(flow=%s) Refused queue[%i]=%ld bits, shared_buffer_bits=%ld  (per_port_capacity_bits %ld bits, shared_capacity_bits=%ld bits reached)\n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(),
            						output_port, queue.port_size_bits, shared_buffer_size_bits, per_port_capacity_bits, shared_capacity_bits);
            packet->printInfo(LOG_LEVEL_DEBUG);

            this->logger->logSignal(t, packet_size_bits, "discard_bits");
            this->logger->logSignal(t, 1, "discard_elems");

            sigma=sigma-e; // continue as before
            return;
        }

        // Otherwise : Accepts
        packet->currentReceptionTimestamp = t;
        if(queue.port_size_bits + buffering_size <= per_port_capacity_bits){ // fixed queue)
        	queue.port_size_bits += buffering_size;
        	queue.queue.push(std::make_shared<QueuedPacket>(packet, /*in_shared_buffer=*/false));
        	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: Incoming Packet #%i(flow=%s, size=%ld (buffer_size=%ld)) queued in queue[%i]=%ld bits (shared_buffer_size_bits=%ld) \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), packet_size_bits, buffering_size, output_port, queue.port_size_bits, shared_buffer_size_bits);

        	// log size of fixed buffers (TODO: these are only selected hardcoded ports)
        	this->logger->logSignal(t, queue.port_size_bits, "port" + std::to_string(output_port) + "_buffer_size_bits");

        } else { // shared buffer
        	shared_buffer_size_bits += buffering_size;
            queue.shared_size_bits += buffering_size;
        	queue.queue.push(std::make_shared<QueuedPacket>(packet, /*in_shared_buffer=*/true));
        	this->logger->logSignal(t, shared_buffer_size_bits, "shared_buffer_size_bits");
        	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: Incoming Packet #%i(flow=%s, size=%ld (buffer_size=%ld)) for queue[%i]=%ld bits queued in shared buffer size =%ld bits \n", t, this->getFullName().data(), packet->getId(), packet->flowId.value.data(), packet_size_bits, buffering_size, output_port, queue.port_size_bits,  shared_buffer_size_bits);

            if (max_single_port_shared_buffer_use_bits < queue.shared_size_bits){
                max_single_port_shared_buffer_use_bits = queue.shared_size_bits;
                this->logger->logSignal(t, max_single_port_shared_buffer_use_bits, "max_port_shared_buffer_size_bits");
            }             
        }

        auto it = pending_requests.find(output_port);
        if(it != pending_requests.end()){
        	pending_sends.push(output_port); // add send
        	pending_requests.erase(it); // remove pending request
        	sigma=0; // send immediately, server already requested next packet
        } else{
        	sigma=sigma-e; // continue as before, waiting for a request from server to arrive
        }
    }

    if (x.port % 2 == 1) {		// Request from server arrived
    	int output_port = (x.port-1) / 2;
    	OutputQueue& queue = this->queues[output_port]; // Adds a new OutputQueue if necessary and retrieve it

//    	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested by Server for dequeue... \n",t, this->getFullName().data());
    	if (queue.queue.size()>0){ // Queue is not empty
            debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested dequeue for non-empty outport %i (with fixed size =%ld), shared_buffer_size_bits=%ld) \n",t, this->getFullName().data(), output_port, queue.port_size_bits, shared_buffer_size_bits);
            pending_sends.push(output_port); // add send
            sigma=0; // send packet
        } else {
        	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[ExT]: Requested dequeue for EMPTY outport %i (with fixed size =%ld), shared_buffer_size_bits=%ld) \n",t, this->getFullName().data(), output_port, queue.port_size_bits, shared_buffer_size_bits);
        	pending_requests.insert(output_port); // remember pending request
        	sigma=sigma-e; // Continues waiting for a new packet
        }
    }
    return;
}

Event packetSharedQueue::lambda(double t) {
	// dequeue request and packet
    // FIXME(cgiudice): check that the deque is not empty before using its elements
	int output_port = pending_sends.front(); pending_sends.pop(); // pop send request
	OutputQueue& queue = this->queues[output_port];
	auto queued_packet = queue.queue.front(); queue.queue.pop(); // pop packet
	auto packet_size_bits = queued_packet->packet->getLength_bits();
    auto buffering_size = getBufferingSize(packet_size_bits);     // account for effective buffer required in terms of cells

    if(queued_packet->in_shared_buffer){
    	shared_buffer_size_bits -= buffering_size;
        queue.shared_size_bits -= buffering_size;
    	this->logger->logSignal(t, shared_buffer_size_bits, "shared_buffer_size_bits");
    } else { // in fixed port buffer
    	queue.port_size_bits -= buffering_size;
    	// log sized of fixed buffers (TODO: these are only selected hardcoded ports)
    	this->logger->logSignal(t, queue.port_size_bits, "port" + std::to_string(output_port) + "_buffer_size_bits");
    }

    debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s[Out]:  dequeue Packet #%i(flow=%s, size=%ld (buffer_size=%ld)) from queue %i (fixed size=%ld bits), shared_buffer_size_bits=%ld bits\n", t, this->getFullName().data(), queued_packet->packet->getId(), queued_packet->packet->flowId.value.data(), packet_size_bits, buffering_size,
    		output_port, queue.port_size_bits, shared_buffer_size_bits);

    return Event(queued_packet->packet, output_port);
}

/**
 * @brief Account for the effective size the packet will use in the buffer.
 * In some switches (e.g Juniper ToR QFX5100) the buffer is divided into cells, so a packet effectively uses a round number of cells instead of the actual size of the packet
 * 
 * @param packetSize 
 * @return long 
 */
long packetSharedQueue::getBufferingSize(long packetSize_bits) const{
    long ret = ceill((double)packetSize_bits / buffer_cell_size_bits) * buffer_cell_size_bits; // cells to use mutiplied by cell size
    return ret;
}
