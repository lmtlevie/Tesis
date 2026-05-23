#if !defined Job_h
#define Job_h

#include <sys/types.h> // uint

class Job {

private:
	// Job ID counter
	static uint NextJobID;

	double weight;
	uint id;

public:
	Job(double jobWeight) :
		weight(jobWeight),
		id(Job::NextJobID++) {

	}

	double getWeight() { return this->weight; }
	// No setJobWeight as jobs are born with a specified weight which does not change. See JobProcessing for jobs being processed.

	uint getId() { return this->id; }

};
#endif
