#include "ProcesorSharingServer.h"

uint ProcesorSharingServer::ProcessorId = 0;

void ProcesorSharingServer::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	this->id = ProcesorSharingServer::ProcessorId++;

	debugMsg(LOG_LEVEL_INIT, "[%f] %s: Starting processor %d \n",t, this->getFullName(), this->id);

	// Read parameters
	char* fvar;

	// Log parameters only once
	int logLevel = LOG_LEVEL_INIT;
	if(this->id == 0) logLevel = 1;

	fvar = va_arg(parameters,char*);
	this->serviceTime = readDefaultParameterValue<double>(fvar);
	debugMsg(logLevel, "[%f] %s: serviceTime: %f  \n",t, this->getFullName(), this->serviceTime);

	fvar = va_arg(parameters,char*);
	this->maxCapacity = readDefaultParameterValue<double>(fvar);
	debugMsg(logLevel, "[%f] %s: maxCapacity: %f parallel jobs . \n",t, this->getFullName(), this->maxCapacity);

	// Initialize
	this->sigma = 0; // start sending current availability

	this->myState = INITIALIZING;

	this->logger->initSignals(std::vector<std::string>{"finished", "arrival", "queueSize", "job1", "job2"});
}

void ProcesorSharingServer::dint(double t) {
	// update jobs pending time
	this->updateJobsPendingServiceTime();

	// remove sent job from queue
	if(this->myState == INITIALIZING){
		this->myState = PROCESSING; // initialize only once
	} else if (this->myState == PROCESSING){
		this->queuedJobs.pop_front();
	} else if (this->myState == DISCARDING){
		this->jobToDiscard.reset(); // release pointer
		this->myState = PROCESSING; // back to processing
	}

	// next sigma (assuming the queue is sorted)
	this->sigma = this->getNextJobFinishTime();

	this->printLoad(LOG_LEVEL_PRIORITY);
}

void ProcesorSharingServer::dext(Event x, double t) {
	auto newJob = castEventPointer<Job>(x);

	debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: New Job #%i received for processing. Job Weight=%f, total jobs=%i \n",t, this->getFullName(), newJob->getId(), newJob->getWeight(), this->queuedJobs.size());

	// update current jobs
	this->updateJobsPendingServiceTime();

	if(this->queuedJobs.size() < this->maxCapacity){ // there is space, add new job
		// add the new job into the queue
		this->addNewJob(t, newJob);

		// next sigma
		this->sigma = this->getNextJobFinishTime();
	} else { // program discard
		this->sigma = 0;
		this->myState = DISCARDING;
		this->jobToDiscard = newJob;
	}

	// logging
	this->logSignals(t);
	this->printLoad(LOG_LEVEL_PRIORITY);
}

Event ProcesorSharingServer::lambda(double t) {
	if(this->myState == INITIALIZING){
		// create a fake finished job to let the dispatcher know initial capacity
		auto initJob = this->getInitializationJob();
		debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: Sending %u finished jobs from processor %u \n",t, this->getFullName(), initJob->getOriginalJobs()->size(), this->id);
		return Event(initJob, 0);
	} else if(this->myState == DISCARDING){
		debugMsg(LOG_LEVEL_ALWAYS,  "[%f] %s: Discarding job #%u from processor %u \n",t, this->getFullName(), this->jobToDiscard->getId(), this->id);
		return Event(this->jobToDiscard, 1); // send discard
	} else if(this->myState == PROCESSING){
		this->finished++;

		// get the first one and remove it from the queue
		auto job = this->queuedJobs.front();

		job->setPendingServiceTime(0);
		this->logSignals(t);

		auto jobs = std::make_shared<std::vector<std::shared_ptr<Job>>>();
		jobs->push_back(job->getOriginalJob());
		auto finishedJob = std::make_shared<FinishedJob>(jobs, this->id, job->getTotalProcessingTime(t));

		debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: Sending finished job #%u from processor %u \n",t, this->getFullName(), finishedJob->getOriginalJobs()->front()->getId(), this->id);

		return Event(finishedJob, 0);
	}

	debugMsg(LOG_LEVEL_ERROR,  "[%f] %s: Unexpected model state \n",t, this->getFullName());
	throw new std::runtime_error("Unexpected model state \n");
}

void ProcesorSharingServer::logSignals(double t){
	this->logger->logSignal(t, this->arrival, "arrival");
	this->logger->logSignal(t, this->finished, "finished");
	this->logger->logSignal(t, this->queuedJobs.size(), "queueSize");

	if(this->queuedJobs.size() > 0){
		this->logger->logSignal(t, this->queuedJobs.front()->getPendingService(), "job1");
	}

	if(this->queuedJobs.size() > 1){
		auto it = this->queuedJobs.begin();
		it++; // second job

		this->logger->logSignal(t, (*it)->getPendingService(), "job2");
	}
}

std::shared_ptr<FinishedJob> ProcesorSharingServer::getInitializationJob(){
	// create a finishedJob with as many fake jobs as my max capacity
	auto jobs = std::make_shared<std::vector<std::shared_ptr<Job>>>();
	for(int i = 0; i < this->maxCapacity; i++){
		jobs->push_back(NULL); // add a null job
	}

	double totalProcessingTime = -1;
	return std::make_shared<FinishedJob>(jobs, this->id, totalProcessingTime);
}

void ProcesorSharingServer::updateJobsPendingServiceTime(){
	if(this->queuedJobs.size() == 0){
		return; // nothing to do
	}

	// update the time for each job
	double servicePerJob = this->e * this->getServiceTimePerJob(); // service distributed equally amount all jobs
	debugMsg(LOG_LEVEL_FULL_LOGGING,  "%s: Time to reduce per job=%f  \n", this->getFullName(), servicePerJob);

	for(auto job : this->queuedJobs){
		job->setPendingServiceTime(job->getPendingService() - servicePerJob);
	}
	//NOTE: The queue was sorted, so subtracting the same amount to all elements maintains sorting.
}

void ProcesorSharingServer::addNewJob(double currentTime, std::shared_ptr<Job> newJob){
	auto newJobToProcess = std::make_shared<JobBeingProcessed>(newJob, currentTime);
	this->arrival++;

	// validate capacity
	if(this->queuedJobs.size() >= this->maxCapacity){
		debugMsg(LOG_LEVEL_FULL_LOGGING,  "[%f] %s: ERROR!! Max capacity exceeded at Processor %i. Max Capacity=%i  \n", currentTime, this->getFullName(), this->id, this->maxCapacity);
		throw new std::runtime_error("Max capacity exceeded at Processor " + this->id);
	}

	// If there are no previous jobs, just insert the new one and finish
	if(this->queuedJobs.size() == 0){
		this->queuedJobs.push_back(newJobToProcess);
		return;
	}

	// insert in corresponding index to keep the queue sorted
	bool added=false;
	for(auto it = this->queuedJobs.begin(); it < this->queuedJobs.end(); it++){
		// check if we found the index where to insert the new one
		if((*it)->getPendingService() > newJobToProcess->getPendingService() ){
			this->queuedJobs.insert(it, newJobToProcess);
			added = true;
			break;
		}
	}

	// If position was not found, insert it at the very end
	if(!added){
		this->queuedJobs.push_back(newJobToProcess);
	}
}

double ProcesorSharingServer::getServiceTimePerJob(){
	return this->serviceTime / this->queuedJobs.size();
}

double ProcesorSharingServer::getNextJobFinishTime(){
	if(this->queuedJobs.size() > 0) {
		return this->queuedJobs.front()->getPendingService() / this->getServiceTimePerJob();
	}

	return INF;
}

void ProcesorSharingServer::printLoad(int logLevel){
	if (LOG_LEVEL >= logLevel) {
		// LOG
		debugMsg(logLevel,  "[%f] %s: Pending jobs in queue: %u \n", this->sigma, this->getFullName(), this->queuedJobs.size());
		for(auto it = this->queuedJobs.begin(); it < this->queuedJobs.end(); it++){
			debugMsg(logLevel,  "#%u(%f), ", (*it)->getOriginalJob()->getId(), (*it)->getPendingService());
		}
		debugMsg(logLevel,  "\n", 1);
	}
}
