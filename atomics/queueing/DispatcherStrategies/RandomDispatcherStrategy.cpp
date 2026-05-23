#include "RandomDispatcherStrategy.h"

RandomDispatcherStrategy::RandomDispatcherStrategy() : IDispatcherStrategy() {
	printLog(LOG_LEVEL_ALWAYS, "Using Strategy: RandomDispatcherStrategy\n");
}

void RandomDispatcherStrategy::addProcessor(uint processorId, uint maxCapacity){

}

void RandomDispatcherStrategy::init(){
	// Randomize at each assignment, not necessary now.

}

void RandomDispatcherStrategy::setIdleProcessor(uint processorId, uint newAvailableCapacity){
	// If not in the list, add it
	if(std::find(processorIds.begin(), processorIds.end(), processorId) == processorIds.end()){
		this->processorIds.push_back(processorId);	 // add the id
	}
}

int RandomDispatcherStrategy::setNextProcessorBusy() {
	// get a random element
	auto randIt = this->processorIds.begin();
	std::advance(randIt, std::rand() % processorIds.size()); //TODO: std::rand()%N can be biased: http://stackoverflow.com/questions/6942273/get-random-element-from-container-c-stl

	return *randIt;
}

int RandomDispatcherStrategy::getIdleProcessors(){
	return this->processorIds.size(); // this policy is not aware of the capacity and load in each processor. This is just a mock value
}
