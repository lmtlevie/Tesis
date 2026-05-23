#if !defined JobAssignment_h
#define JobAssignment_h

#include "Job.h"

#include <sys/types.h> // uint
#include <memory>

class JobAssignment {

private:
	// JobAssignment ID counter
	std::shared_ptr<Job> job;
	uint processorId;

public:
	JobAssignment(std::shared_ptr<Job> job, uint processorId) :
		job(job),
		processorId(processorId) {

	}

	std::shared_ptr<Job> getJob() { return this->job; }
	uint getProcessorId() { return this->processorId; }
};
#endif
