#include "BalancedDispatcherStrategy.h"

BalancedDispatcherStrategy::BalancedDispatcherStrategy() : IDispatcherStrategy() {
	printLog(LOG_LEVEL_ALWAYS, "Using Strategy: BalancedDispatcherStrategy\n");
}

void BalancedDispatcherStrategy::addProcessor(uint processorId, uint maxCapacity){

}

void BalancedDispatcherStrategy::init(){

}

void BalancedDispatcherStrategy::setIdleProcessor(uint processorId, uint newAvailableCapacity){
	/// Algorithm: 1-Find, 2-Update
	std::shared_ptr<ProcessorLoad> processor;

	// 1-Find the processor by Id
	auto processorIt = this->findDcm(processorId);
	if(processorIt == this->processors.end()){ // if it is not present, we create a new one
		processor = std::make_shared<ProcessorLoad>(processorId);
		this->processors.push_back(processor);
	} else {
		processor = *processorIt;
	}

	// 2-Update processor status
	for (uint i = 0; i < newAvailableCapacity; i++){
		// add busy PU. This breaks sorting
		processor->jobFinished();
		this->totalIdleProcessors++;
		//printLog(LOG_LEVEL_FULL_LOGGING, "adding capacity to Processor %i. It has now %i idle capacity  \n", processor->getProcessorId(), processor->getAvailableCapacity());
	}

	//this->printLoad(1);
}

int BalancedDispatcherStrategy::setNextProcessorBusy() {
	if(this->getIdleProcessors() <= 0){
		return 0; // if there are no idle processors, just choose the first one which will discard the event
	}

	// 1-Find
	auto processor = this->getWeightedRandomProcessor();

	// 2-Update add busy PU. This breaks sorting so it must be thread safe (which its not)
	processor->jobAssigned();
	this->totalIdleProcessors--;
//	printLog(LOG_LEVEL_FULL_LOGGING, "setNextPuBusy:Adding busy processor. Processor %i now has %i availableCapacity	,  \n", processor->getProcessorId(), processor->getAvailableCapacity());

//	this->printLoad(1);

	return processor->getProcessorId();
}

int BalancedDispatcherStrategy::getIdleProcessors(){
	return this->totalIdleProcessors;
}

std::shared_ptr<ProcessorLoad> BalancedDispatcherStrategy::getWeightedRandomProcessor(){
	// calculate frequencies for the weighted random algorithm, using the available capacity
	// TODO: can we improve performance by maintaining these vectors always already calculated?
	// TODO: can we improve performance by returning directly a pointer to the processor (instead of the ID and then search for it)
	std::vector<double> processorIds;
	std::vector<double> processorWeights;
	for(auto processor : this->processors){
		if(processor->getAvailableCapacity() <= 0) continue; // No Available capacity continue...

		processorIds.push_back(processor->getProcessorId());
		processorWeights.push_back(processor->getAvailableCapacity());
	}

	// get a processor using the weighted random algorithm
	int selectedProcessorId = STDEVS::getConfigurationInstance()->getWeightedElement(processorIds,processorWeights);

	// 1-Find the processor by Id
	auto processorIt = this->findDcm(selectedProcessorId);
	if(processorIt == this->processors.end()){ // if it is not present
		throw new std::runtime_error("BalancedDispatcherStrategy::getWeightedRandomProcessor. Attempted to retrieve processor which does not exist");
	}

	return *processorIt;
}

void BalancedDispatcherStrategy::printLoad(int logLevel){
	printLog(logLevel, "System Load: \n");
	printLog(logLevel, "Processor Id (AvailableCapacity) :[");
	for(auto processor : this->processors){
		printLog(logLevel, "%i (%i), ", processor->getProcessorId(), processor->getAvailableCapacity());
	}
	printLog(logLevel, "] \n");
}
