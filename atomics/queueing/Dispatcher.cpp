#include "Dispatcher.h"

void Dispatcher::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	debugMsg(LOG_LEVEL_INIT, "[%f] %s: INIT \n", t, this->getName());

	// Read parameters
	char* fvar;

	fvar = va_arg(parameters, char*);
	double strategyParameter = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: strategyOption: %f \n", t, this->getName(), strategyParameter);

	// Check which assignment algorithm was configured
	DISPATCHER_STRATEGY strategyOption = static_cast<DISPATCHER_STRATEGY>(strategyParameter);

	// create the strategy according to the configured algorithm
	this->strategy = this->createStrategy(strategyOption);

	// Initialize
	this->sigma = INF; // wait for next event

	this->logger->initSignals(std::vector<std::string>{"sent", "totalProcessingTime", "idleProcessors"});
}


void Dispatcher::dint(double t) {
	this->logger->logSignal(t, this->strategy->getIdleProcessors(),"idleProcessors");
	this->sigma = INF; // wait for next event
}

void Dispatcher::dext(Event x, double t) {
	if (x.port == 0) { // new job arrived
		if(this->sent == 0){
			this->strategy->init(); // Initialize strategy after all processors published their availability.
		}

		auto newJob = castEventPointer<Job>(x);
		this->processNewJob(t, newJob);
	} else if (x.port == 1) { // a processor is now idle
		auto finishedJob = castEventPointer<FinishedJob>(x);
		this->processFinishedJob(t, finishedJob);
	}

	this->logger->logSignal(t, this->strategy->getIdleProcessors(),"idleProcessors");
}

Event Dispatcher::lambda(double t) {
	debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: Sending job (#%u) to processor #%u. \n", t, this->getName(), this->nextOutputJob->getId(), this->nextProcessorToAssign);
	this->logger->logSignal(t,++this->sent,"sent");

	// Send assignment
	auto assignment = std::make_shared<JobAssignment>(this->nextOutputJob, this->nextProcessorToAssign);
	return Event(assignment, 0 /*assignment port*/);
}

void Dispatcher::processNewJob(double t, std::shared_ptr<Job> job){
	debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: New job (#%u) arrived. Total Idle=%i  \n", t, this->getName() , job->getId(), this->strategy->getIdleProcessors());

	// get the next processor from the strategy
	this->nextProcessorToAssign = this->strategy->setNextProcessorBusy();
	this->nextOutputJob = job;

	this->sigma = 0; // program transition now
}

void Dispatcher::processFinishedJob(double t, std::shared_ptr<FinishedJob> jobs){
	// set the processor idle using the current strategy
	this->strategy->setIdleProcessor(jobs->getProcessorId(), jobs->getOriginalJobs()->size());

	this->sigma = this->sigma - this->e; // keep waiting

	// logging
	++this->finished;
	this->logger->logSignal(t,this->finished,"finished");
	this->logger->logSignal(t,jobs->getTotalProcessingTime(),"totalProcessingTime");

	debugMsg(LOG_LEVEL_PRIORITY,  "[%f] %s: Processor #%u has finished %u jobs. Total Idle=%i \n", t, this->getName() , jobs->getProcessorId(), jobs->getOriginalJobs()->size(), this->strategy->getIdleProcessors());
}

std::shared_ptr<IDispatcherStrategy>  Dispatcher::createStrategy(DISPATCHER_STRATEGY assignmentAlgorithm){
	switch (assignmentAlgorithm) {
			case FIFO: {
				return std::make_shared<FifoDispatcherStrategy>();
				break;
			}
			case RANDOM_LOAD_AWARE: {
				return std::make_shared<RandomLoadAwareDispatcherStrategy>();
				break;
			}
			case JOIN_SHORTEST_QUEUE: {
				return std::make_shared<JsqDispatcherStrategy>();
				break;
			}
			case BALANCED: {
				return std::make_shared<BalancedDispatcherStrategy>();
				break;
			}
			case RANDOM: {
				return std::make_shared<RandomDispatcherStrategy>();
				break;
			}
			default: {
				debugMsg(LOG_LEVEL_ERROR, "[Dispatcher] createStrategy: unknown dispatcher strategy: %i  \n", assignmentAlgorithm);
				throw std::runtime_error("createStrategy: unknown dispatcher strategy");
				break;
			}
		}
}
