#ifndef queueingTheory_DispatcherStrategies_ProcessorLoad_H_
#define queueingTheory_DispatcherStrategies_ProcessorLoad_H_

#include "sinks/ParameterReader.h"
#include "queueing/DispatcherStrategies/IDispatcherStrategy.h"
#include "queueing/Jobs/Job.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <list>

class ProcessorLoad {
		int processorId;

		int currentJobs = 0;
		int maxConcurrent = 0;


	public:
		ProcessorLoad(int myId):
			processorId(myId){
		}

		bool operator==(const ProcessorLoad& other) const { return other.processorId == this->processorId;};
		bool operator!=(const ProcessorLoad &other) const { return !(*this == other); }

		int getProcessorId() { return this->processorId;};

		int getUsedCapacity () const { return this->currentJobs; }; // Complexity constant
		int getAvailableCapacity () const { return this->maxConcurrent - this->getUsedCapacity(); }; // Complexity constant

		void jobAssigned(); // Complexity constant
		void jobFinished(); // Complexity constant
};

#endif /* queueingTheory_DispatcherStrategies_ProcessorLoad_H_ */
