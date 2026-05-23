#include "JsqDispatcherStrategy.h"

JsqDispatcherStrategy::JsqDispatcherStrategy() : IDispatcherStrategy() {
	printLog(LOG_LEVEL_ALWAYS, "Using Strategy: JspDispatcherStrategy\n");
}

void JsqDispatcherStrategy::addProcessor(uint processorId, uint maxCapacity){

}

void JsqDispatcherStrategy::init(){

}

///
void JsqDispatcherStrategy::setIdleProcessor(uint processorId, uint newAvailableCapacity){
	/// Algorithm: 1-Find, 2-Update, 3-Sort
	std::shared_ptr<ProcessorLoad> processor;

	// 1-Find the processor by Id
	auto processorIt = this->findDcm(processorId);
	// if it is not present, we create a new one
	if(processorIt == this->processors.end()){
		processor = std::make_shared<ProcessorLoad>(processorId);
	} else {
		processor = *processorIt;
		this->processors.erase(processorIt);
	}

	// 2-Update procesor status
	for (uint i =0; i < newAvailableCapacity; i++){
		// add busy PU. This breaks sorting
		processor->jobFinished();
		this->totalIdleProcessors++;
		printLog(LOG_LEVEL_FULL_LOGGING, "adding capacity to Processor %i. It has now %i idle capacity  \n", processor->getProcessorId(), processor->getAvailableCapacity());
	}

	// 3- Sort. Find the first one with more load. TODO: it would be better to search from dcmIt-1 to begin. i.e: in reverse order
	auto rfirstSmallerIt = std::find_if(this->processors.begin(), processorIt, [&](std::shared_ptr<ProcessorLoad> const& p) {
			return processor->getUsedCapacity() < p->getUsedCapacity(); // shortest queue
	});
	this->processors.insert(rfirstSmallerIt, processor);

	//this->printLoad(LOG_LEVEL_FULL_LOGGING);
}

int JsqDispatcherStrategy::setNextProcessorBusy() {
	if(this->getIdleProcessors() <= 0){
		return 0; // if there are no idle processors, just choose the first one which will discard the event
	}

	// 1-Find
	std::vector<std::shared_ptr<ProcessorLoad>>::iterator processorIt =  this->processors.begin();
	auto processor = *processorIt;
	this->processors.erase(processorIt);

	printLog(LOG_LEVEL_FULL_LOGGING, "setNextProcessorBusy: found minimal Processor. processorId:%i (availableCapacity:%i)\n", processor->getProcessorId(), processor->getAvailableCapacity());

	// 2-Update add busy PU. This breaks sorting so it must be thread safe (TODO: which its not)
	processor->jobAssigned();
	this->totalIdleProcessors--;
	printLog(LOG_LEVEL_FULL_LOGGING, "setNextPuBusy:Adding busy processor. Processor %i now has %i availableCapacity	,  \n", processor->getProcessorId(), processor->getAvailableCapacity());

	// 3- Sort find position where to move it to maintain sorting: last of all the ones that are now equal to this element. It will be moved back in the list, just before the first item with than is bigger
	auto firstBiggerIt = std::find_if(processorIt, this->processors.end(), [&](std::shared_ptr<ProcessorLoad> const& p) {
		return processor->getUsedCapacity() < p->getUsedCapacity(); // shortest queue
	});
	this->processors.insert(firstBiggerIt, processor);

	//this->printLoad(LOG_LEVEL_FULL_LOGGING);

	return processor->getProcessorId();
}

int JsqDispatcherStrategy::getIdleProcessors(){
	return this->totalIdleProcessors;
}

void JsqDispatcherStrategy::printLoad(int logLevel){
	printLog(logLevel, "System Load: \n");
	printLog(logLevel, "Processor Id (AvailableCapacity) :[");
	for(auto processor : this->processors){
		printLog(logLevel, "%i (%i), ", processor->getProcessorId(), processor->getAvailableCapacity());
	}
	printLog(logLevel, "] \n");
}
