#include "RandomLoadAwareDispatcherStrategy.h"

RandomLoadAwareDispatcherStrategy::RandomLoadAwareDispatcherStrategy() : IDispatcherStrategy() {
	printLog(LOG_LEVEL_ALWAYS, "Using Strategy: RandomLoadAwareDispatcherStrategy\n");
}

void RandomLoadAwareDispatcherStrategy::addProcessor(uint processorId, uint maxCapacity){
	for (uint i =0; i < maxCapacity; i++){
			this->idleProcessorIds.push_back(processorId);	 // mark idle Processor
	}
}

void RandomLoadAwareDispatcherStrategy::init(){
	// Randomize at each assignment, not necessary now.

}

void RandomLoadAwareDispatcherStrategy::setIdleProcessor(uint processorId, uint newAvailableCapacity){
	for (uint i =0; i < newAvailableCapacity; i++){
		this->idleProcessorIds.push_back(processorId);	 // mark idle Processor
	}
}

int RandomLoadAwareDispatcherStrategy::setNextProcessorBusy() {
	if(this->getIdleProcessors() <= 0){
		return 0; // if there are no idle processors, just choose the first one which will discard the event
	}

	// Randomize
	std::random_shuffle(this->idleProcessorIds.begin(), this->idleProcessorIds.end() );

	// remove it from idle
	int processorId = this->idleProcessorIds.front();
	idleProcessorIds.pop_front(); // remove it from idles

	return processorId;
}

int RandomLoadAwareDispatcherStrategy::getIdleProcessors(){
	return this->idleProcessorIds.size();
}
