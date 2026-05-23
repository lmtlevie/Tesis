#include "ProcessorLoad.h"

void ProcessorLoad::jobAssigned(){
	if (this->getAvailableCapacity() <= 0){
		printLog(LOG_LEVEL_ERROR, "Attempting to assign a job to a full processor");
		throw "Attempting to assign a job to a full processor";
	}

	this->currentJobs++;
};


void ProcessorLoad::jobFinished(){
	if(currentJobs < 0){ // this is a signal that capacity increased
		this->maxConcurrent++;
	} else {
		this->currentJobs--;
	}
};
