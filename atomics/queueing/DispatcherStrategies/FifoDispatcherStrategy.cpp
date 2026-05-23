#include "FifoDispatcherStrategy.h"

FifoDispatcherStrategy::FifoDispatcherStrategy() : IDispatcherStrategy() {
	printLog(LOG_LEVEL_ALWAYS, "Using Strategy: FifoDispatcherStrategy\n");
}

void FifoDispatcherStrategy::addProcessor(uint processorId, uint maxCapacity){
	for (uint i =0; i < maxCapacity; i++){
			this->idleProcessorIds.push_back(processorId);	 // mark idle Processor
	}
}

void FifoDispatcherStrategy::init(){
	// Randomize
	//std::random_shuffle(this->idleProcessorIds.begin(), this->idleProcessorIds.end() );
}

void FifoDispatcherStrategy::setIdleProcessor(uint processorId, uint newAvailableCapacity){
	for (uint i =0; i < newAvailableCapacity; i++){
		this->idleProcessorIds.push_back(processorId);	 // mark idle Processor
	}
}

int FifoDispatcherStrategy::setNextProcessorBusy() {
	if(this->getIdleProcessors() <= 0){
		return 0; // if there are no idle processors, just choose the first one which will discard the event
	}

	int processorId = this->idleProcessorIds.front();
	idleProcessorIds.pop_front(); // remove it from idles

	return processorId;
}

int FifoDispatcherStrategy::getIdleProcessors(){
	return this->idleProcessorIds.size();
}
