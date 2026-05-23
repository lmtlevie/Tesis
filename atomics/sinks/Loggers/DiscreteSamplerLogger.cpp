#include "DiscreteSamplerLogger.h"

DiscreteSamplerLogger::DiscreteSamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger) : DiscreteSamplerLogger(model, baseLogger, -1){
	printLog(LOG_LEVEL_DEBUG, "Created DiscreteSamplerLogger \n");
	// the sample period will be read when the variable is initialized (so that we know the variable name)
}

DiscreteSamplerLogger::DiscreteSamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger, double samplePeriod) :  samplePeriod(samplePeriod), nextSampleT(samplePeriod), model(model), baseLogger(baseLogger){
	printLog(LOG_LEVEL_DEBUG, "Created DiscreteSamplerLogger for model %s \n", this->model->getFullName().data());
	// reset
	resetCounters();
}

void DiscreteSamplerLogger::initSignals(const std::map<std::string,int>& variableNamesAndPriorities){
	printLog(LOG_LEVEL_ERROR, "Method 'DiscreteSamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'DiscreteSamplerLogger::initSignals(std::vector<std::string> variableNames)'");
	throw std::runtime_error("Method 'DiscreteSamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'DiscreteSamplerLogger::initSignals(std::vector<std::string> variableNames)'");

}

void DiscreteSamplerLogger::initSignals(const std::vector<std::string>& variableNames) {
	if(variableNames.size() != 1){
		printLog(LOG_LEVEL_ERROR, "DiscreteSamplerLogger::initSignals: Trying to initialize sampler for model %s with %u variables. Each sampler can only sample 1 variable at a time. \n", this->model->getFullName().data(), variableNames.size());
		throw std::runtime_error("DiscreteSamplerLogger::initSignals: Trying to initialize sampler with variables != 1. Only one variable can be samples. See log for details.");
	}

	this->variableName = variableNames[0];

	if(this->samplePeriod <= 0 ){ // Read samplePeriod from config
		if(!readScopedVariable<double>(this->model, this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME, this->samplePeriod) || this->samplePeriod <= 0){
			printLog(LOG_LEVEL_ERROR, "DiscreteSamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. Variable '%s' was not correctly set in configuration for model '%s'. \n", (this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME).data(), this->model->getFullName().data());
			throw std::runtime_error("DiscreteSamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. See log for details.");
		}
	}
	this->nextSampleT = this->samplePeriod;

	printLog(LOG_LEVEL_FULL_LOGGING, "DiscreteSamplerLogger::initSignals: model.variable: %s.%s ;  samplePeriod: %f \n", this->model->getFullName().data(), this->variableName.data(), samplePeriod);

	// init variables in base logger
	std::vector<std::string> vars =   {
			this->variableName + ".max",
			this->variableName + ".min",
			this->variableName + ".sum",
			this->variableName + ".avg",
			this->variableName + ".first",   // first value in the sampling period
			this->variableName + ".sample",  // a single sample over all of the events arrived
			this->variableName + ".evCount",
			this->variableName + ".expMovingAvg",
			this->variableName + ".movingAvg",
			this->variableName + ".Hz",    // rate per second
			this->variableName + ".sample_period" // will be logged only once, as it might be needed and not logged if loggingBackend!=readingBackend
	};
	this->baseLogger->initSignals(vars);

	// record the sampling period that will be used (TODO: it is logged multiple times now :-( )
	this->baseLogger->logSignal(0, this->samplePeriod, this->variableName + ".sample_period"); // TODO: implement logValue(value, variableName) in IPowerDEVSLogger
}

void DiscreteSamplerLogger::logSignal(double t, double signalValue, const std::string& signalName) { // Logger
	if(signalName != this->variableName) return; // just in case

	double previousT = this->lastT; // save it as it is needed by the expMovingAvg and updated in logSampledValues

	// When a values arrive, they are not recorded immediately. They are recorded when a value out of the sampling period arrives
	// record samples if period elapsed
	if(t >= this->nextSampleT){
		this->logSampledValues(t);
	}

	// values for the sampling period
	this->eventCounter++;
	addValueToMovingAvg(signalValue); // moving average
	addValueToExpMovingAvg(signalValue, t, previousT);
	this->sumCounter += signalValue;
	this->maxCounter = std::isnan(maxCounter)?signalValue : std::max(this->maxCounter, signalValue);
	this->minCounter = std::isnan(minCounter)?signalValue : std::min(this->minCounter, signalValue);
	this->first = std::isnan(first)?signalValue : this->first;
	if(this->nextSampleT - t >= this->samplePeriod/2){ // try to get a value around the middle of the sampling period
		this->sample = signalValue;
	}
	this->lastT = t;
}

void DiscreteSamplerLogger::logSampledValues(const double t){
	do {
		double currentTime = this->nextSampleT;
		if(t < this->nextSampleT){ // this happens only when flushing
			currentTime = t; // we use the current time when flushing, so be more accurate with the timeAvg sampled
		}

		// record samples
		this->baseLogger->logSignal(currentTime, this->eventCounter, this->variableName + ".evCount");
		this->baseLogger->logSignal(currentTime, this->maxCounter, this->variableName + ".max");
		this->baseLogger->logSignal(currentTime, this->minCounter, this->variableName + ".min");
		this->baseLogger->logSignal(currentTime, this->sumCounter, this->variableName + ".sum");
		this->baseLogger->logSignal(currentTime, this->first, this->variableName + ".first");
		this->baseLogger->logSignal(currentTime, this->sample, this->variableName + ".sample");
		this->baseLogger->logSignal(currentTime, (this->sumCounter==0)?0:this->sumCounter / this->eventCounter, this->variableName + ".avg");
		this->baseLogger->logSignal(currentTime, (this->sumCounter==0)?0:this->sumCounter / this->samplePeriod, this->variableName + ".Hz");
		this->baseLogger->logSignal(currentTime, this->expMovingAvg, this->variableName + ".expMovingAvg");
		this->baseLogger->logSignal(currentTime, this->getMovingAvg(), this->variableName + ".movingAvg");

		// reset
		resetCounters();

		// set next sample time
		this->nextSampleT += this->samplePeriod;

		// record samples again (if more that one period had elapsed)
		this->lastT = currentTime;
	} while(t >= this->nextSampleT);
}

void DiscreteSamplerLogger::flush(double t) { // Flush
	this->logSampledValues(t); // log last values

	this->baseLogger->flush(t);
}

void  DiscreteSamplerLogger::resetCounters(){
	this->eventCounter = 0;
	this->sumCounter = 0;
	this->maxCounter = NAN;
	this->minCounter = NAN;
	this->first = NAN;
	this->sample = NAN;
}

void DiscreteSamplerLogger::addValueToMovingAvg(double value){
 if(this->valuesInWindow.size() >= this->movingAvgWindowSize){
	 this->valuesInWindow.erase(this->valuesInWindow.begin()); // remove first (pop_front)
 }

 this->valuesInWindow.push_back(value); // add to back

}

void DiscreteSamplerLogger::addValueToExpMovingAvg(double value, double currentT, double previousT){
	if(previousT == -1) { // expMovingAvg base case
		this->expMovingAvg = value;
	} else	{ // expMovingAvg
		double alpha = (currentT - previousT) / this->samplePeriod; // alpha = (Tn - Tn-1) / W ; TODO: now getting the moving averange window=sampling period
		this->expMovingAvg = value + exp(-alpha) * (this->expMovingAvg - value); // MA_today = new_value + e^(-alpha) * ( MA_yesterdy - new_value)
	}
}

double DiscreteSamplerLogger::getMovingAvg(){
	// TODO: this could be implemented much more efficient doing the calculation every time we add a value.
	double sum = 0;
	for (auto val : this->valuesInWindow){
		sum += val;
	}

	return sum / this->valuesInWindow.size(); // sum/N
}
