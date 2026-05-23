#include "PacketFlowGenerator.h"

using namespace boost;
using namespace std;

void PacketFlowGenerator::init(double t,...) {
    BaseSimulator::init(t);

    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters,t);

    // read parameters
    char* fvar;
    fvar = va_arg(parameters,char*);
    this->flowName =fvar;
    debugMsg(LOG_LEVEL_INIT, "[%g] %s: flowName: %s  \n",t, this->getFullName().data(), this->flowName.data());

    // get the flows for this server
    this->flow = PacketFlowDefinitions::getFlow(this->flowName);

    // Read parameters from flows assigned to this server (by name)
    auto startStopTimes_vector = this->flow->getStartStopTimes();
    this->startStopTimes = std::deque<double>(startStopTimes_vector.begin(), startStopTimes_vector.end());
    this->startStopTimes.push_back(std::numeric_limits<double>::infinity()); // add a last value with inf (last stop/start is at infinity) to facilitate implementation (startStopTimes.front() should never fail)
    debugMsg(LOG_LEVEL_INIT, "[%g] %s: StartTime: %g secs. \n",t, this->getFullName().data(),startStopTimes.front());

    this->generationPeriod = this->flow->getPeriod();
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: generation period: %s \n", this->getFullName().data(), this->generationPeriod->getFullName().data());

    this->packetSize_bits = this->flow->getSize();
    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: PacketSize: %s (bits) \n", this->getFullName().data(), this->packetSize_bits->getFullName().data());

    // Initialize
    this->generatedPackets=0;
    this->sigma=startStopTimes.front();
    if (this->sigma > 0){
        this->generationStopped = true;
    } else {
        this->generationStopped = false;
        startStopTimes.pop_front(); // pop the zero
    }

    debugMsg(LOG_LEVEL_INIT, "[INIT] %s: sigma=%f \n", this->getFullName().data(), this->sigma);
    this->logger->initSignals(std::map<std::string,int>{
        {"intergen", SCILAB_LOG_LEVEL_PRIORITY},
        {"sent_bits", SCILAB_LOG_LEVEL_ALWAYS},
        {"forcedGeneration", SCILAB_LOG_LEVEL_DEBUG},
        {"count", SCILAB_LOG_LEVEL_DEBUG}
    });

}

void PacketFlowGenerator::dint(double t) {
    if(t >= startStopTimes.front()){
        this->generationStopped = !this->generationStopped;
        startStopTimes.pop_front();
        debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s(int): changed start/stop. GenerationStopped: %i \n", t, this->getFullName().data(), this->generationStopped);
    }

    this->sigma = this->generationPeriod->nextValue();
    if(this->generationStopped){ // stop until next start
        this->sigma = startStopTimes.front() - t; // use start time
    }

    if(this->queuedGenerations > 0){
        this->sigma = 0;
    }

    return;
}
void PacketFlowGenerator::dext(Event x, double t) {
    // check if the startStop time already elapsed
    if(t >= startStopTimes.front()){
        this->generationStopped = !this->generationStopped;
        startStopTimes.pop_front();
        debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s(ext): changed start/stop. GenerationStopped: %i \n", t,  this->getFullName().data(), this->generationStopped);
    }

    if (x.port==0) {   // force generation of new packet
        debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s: force generation signal arrived \n",t, this->getFullName().data());

        this->logger->logSignal(t, 1, "forcedGeneration");

        if(this->generationStopped){ // ignore message and continue as before
            this->sigma = startStopTimes.front() - t; // continue as before
        } else {
            this->queuedGenerations++;
            this->sigma = 0; // program transition immediately
            this->logger->logSignal(t, e, "intergen");
        }

    } else  {
        debugMsg(LOG_LEVEL_ERROR, "[%g] %s::dext: unknown port %i \n ", t, this->getFullName().data(), x.port);
        throw std::runtime_error("PacketFlowGenerator::dext unknown port \n");
    }
}


Event PacketFlowGenerator::lambda(double t) {
    auto packet = createPacket(t);

    // log
    this->logger->logSignal(t, this->sigma, "intergen");
    this->logger->logSignal(t, this->generatedPackets, "count");
    this->logger->logSignal(t, packet->getLength_bits(), "sent_bits");

    int level = LOG_LEVEL_IMPORTANT;
    if( (this->generatedPackets % 100000 ) == 0){ // to show some progress every now and then
        level = LOG_LEVEL_ERROR; // always
    }
    debugMsg(level, "[%f] %s_lamda: Sending packet #%u (size=%u bits), generated after %f \n", t, this->getFullName().data(), packet->getId(), packet->getLength_bits(),  this->sigma);

    if(packet->getId() % 100000000 == 0){// every 1G packets
        packet->printInfo(LOG_LEVEL_ALWAYS);
    }
    //	packet->printInfo(level);

    // send the new job
    this->queuedGenerations--;
    this->queuedGenerations = std::max(0, queuedGenerations);
    return Event(packet, 0);
}

std::shared_ptr<NetworkPacket> PacketFlowGenerator::createPacket(double t){
    // Create new packet
    this->generatedPackets++;

    int pSize = this->getNextPacketSize_bits();
    auto packet = std::make_shared<NetworkPacket>(t, pSize, this->flowName);
//    packet->FlowID = this->getFullName();

    // Routing protocol with the route determined by the flow
//    auto routingProt = std::make_shared<RoutingProtocol>(this->flow->getRoute()); // we need a copy of the route, as it will be modified along the way
//    routingProt->getRoute()->popJump(); // before sending the job, remove this node from the route
//    packet->addProtocol(routingProt);

    // create a QoS protocol with the priorities determined by the flow
    std::shared_ptr<IProtocol> qosProtocol = std::make_shared<QualityOfServiceProtocol>(this->flow->getTypeOfService()); // we need a copy of the route, as it will be modified along the way
    packet->addProtocol(qosProtocol);

    return packet;
}

int PacketFlowGenerator::getNextPacketSize_bits(){
    // stochastic job weight
    int packetSize = this->packetSize_bits->nextValue();

    // validate
    if(packetSize <= 0) {
        packetSize = 1; // to allow for exp and norm distributions (cero values are generated)
        debugMsg(LOG_LEVEL_ERROR, "WARNING - %s_getNextPacketSize_bits: Wrong distribution for packetSize generated a 0 size packet (forcing packetSize=1). PacketSize Distribution: %s (bits) \n", this->getFullName().data(), this->packetSize_bits->getFullName().data());
        //throw std::runtime_error("Wrong distribution for packetSize generated a 0 size packet.");
    }

    return packetSize;
}
