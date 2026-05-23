#if !defined FinishedJob_h
#define FinishedJob_h

#include "Job.h"
#include <sys/types.h> // uint
#include <vector>
#include <memory>

class FinishedJob {

private:
	std::shared_ptr<std::vector<std::shared_ptr<Job>>> originalJobs;
	uint processorId;
	double totalProcessingTime;

public:
	FinishedJob(std::shared_ptr<std::vector<std::shared_ptr<Job>>> jobs, uint processorId, double totalProcessingTime) :
		originalJobs(jobs),
		processorId(processorId),
		totalProcessingTime(totalProcessingTime){

	}

	std::shared_ptr<std::vector<std::shared_ptr<Job>>> getOriginalJobs() { return this->originalJobs; }

	uint getProcessorId() { return this->processorId; }

	double getTotalProcessingTime() { return this->totalProcessingTime; }

};
#endif
