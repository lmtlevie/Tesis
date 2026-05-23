#include "JobGenerator.h"

void JobGenerator::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	  %Type% is the parameter type

	// Read parameters
	char* fvar;
	fvar = va_arg(parameters,char*);
	this->startTime = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: StartTime: %f secs. \n",t, this->getName(), this->startTime);
	
	fvar = va_arg(parameters,char*);
	this->generationPeriod = readDistributionParameter(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: generation period: %s \n", this->getName(), this->generationPeriod->getFullName().data());

	fvar = va_arg(parameters,char*);
	this->jobSize = readDistributionParameter(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: JobSizeAverage: %s \n", this->getName(), this->jobSize->getFullName().data());

    fvar = va_arg(parameters,char*);
	this->eventsThreshold = readDefaultParameterValue<int>(fvar);
	if(this->eventsThreshold < 0) this->eventsThreshold = std::numeric_limits<int>::max();
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: eventsThreadhold: %i \n", this->getName(), this->eventsThreshold);

	// Initialize
	this->sigma = startTime + this->generationPeriod->nextValue();

	counter=0;

	this->logger->initSignals(std::vector<std::string>{"count", "intergen", "jobWeight"});

	return;
}

void JobGenerator::dint(double t) {
//	if(this->counter >= this->eventsThreshold){
//		this->sigma = INF; // stop sending events if threshold is reached
//		debugMsg(LOG_LEVEL_ALWAYS, "[%f] %s: reach events threshold=%i. Stopping event generation \n",t, this->getName(), this->eventsThreshold);
////		StoppableSimulator::Stop(); // Stop other models (TODO: maybe not always a desired behavior in other models)
//	} else {
		this->sigma = this->generationPeriod->nextValue();
//	}

	return;
}
void JobGenerator::dext(Event x, double t) {
	return; // no input ports
}


Event JobGenerator::lambda(double t) {
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: Sending an event, generated after %f \n", t, this->getName(), this->sigma);

	// stochastic job weight
	double jobSize = this->jobSize->nextValue();

	// validate
	if(jobSize <= 0) {
		debugMsg(LOG_LEVEL_ERROR, "Wrong distribution for jobWeight generated a 0 weight job.", t);
		throw std::runtime_error("Wrong distribution for jobWeight generated a 0 weight job.");
	}

	// Create new job
	this->counter++;
	auto job = std::make_shared<Job>(jobSize);

	// log
	this->logger->logSignal(t, this->sigma, "intergen");
	this->logger->logSignal(t, this->counter, "count");
	this->logger->logSignal(t, job->getWeight(), "jobWeight");

	if(this->debugLevel > LOG_LEVEL_FULL_LOGGING || (this->counter % 100000 ) == 0){
		debugMsg(LOG_LEVEL_ALWAYS, "[%f] %s_lambda: Sending event %i (weight=%f) \n", t, this->getName(), job->getId(), job->getWeight());
	}

	// send the new job
    return Event(job, 0);
}
