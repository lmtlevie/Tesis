#include "SamplerLogger.h"

SamplerLogger::SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger) : SamplerLogger(model, baseLogger, -1){
	printLog(LOG_LEVEL_DEBUG, "Created SamplerLogger \n");
	// the sample period will be read when the variable is initialized (so that we know the variable name)
}

SamplerLogger::SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger, double samplePeriod) :  samplePeriod(samplePeriod), nextSampleT(samplePeriod), model(model), baseLogger(baseLogger){
	printLog(LOG_LEVEL_DEBUG, "Created SamplerLogger for model %s \n", this->model->getFullName().data());
	// reset
	resetCounters();
	this->lastValue = NAN;
	this->maxCounter  = std::numeric_limits<double>::min();
	this->minCounter  = std::numeric_limits<double>::max();
}

void SamplerLogger::initSignals(const std::map<std::string,int>& variableNamesAndPriorities){
	printLog(LOG_LEVEL_ERROR, "Method 'SamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'SamplerLogger::initSignals(std::vector<std::string> variableNames)'");
	throw std::runtime_error("Method 'SamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'SamplerLogger::initSignals(std::vector<std::string> variableNames)'");

}

void SamplerLogger::initSignals(const std::vector<std::string>& variableNames) {
	if(variableNames.size() != 1){
		printLog(LOG_LEVEL_ERROR, "SamplerLogger::initSignals: Trying to initialize sampler for model %s with %u variables. Each sampler can only sample 1 variable at a time. \n", this->model->getFullName().data(), variableNames.size());
		throw std::runtime_error("SamplerLogger::initSignals: Trying to initialize sampler with variables != 1. Only one variable can be samples. See log for details.");
	}

	this->variableName = variableNames[0];

	if(this->samplePeriod <= 0 ){ // Read samplePeriod from config
		if(!readScopedVariable<double>(this->model, this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME, this->samplePeriod) || this->samplePeriod <= 0){
			printLog(LOG_LEVEL_ERROR, "SamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. Variable '%s' was not correctly set in configuration for model '%s'. \n", (this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME).data(), this->model->getFullName().data());
			throw std::runtime_error("SamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. See log for details.");
		}
	}
	this->nextSampleT = this->samplePeriod;

	printLog(LOG_LEVEL_FULL_LOGGING, "SamplerLogger::initSignals: model.variable: %s.%s ;  samplePeriod: %f \n", this->model->getFullName().data(), this->variableName.data(), samplePeriod);

	// init variables in base logger
	std::vector<std::string> vars =   {
			this->variableName + ".sample_period",
			this->variableName + ".max",
			this->variableName + ".timeAvg",
			this->variableName + ".min",
			this->variableName + ".sum",
			this->variableName + ".avg",
			this->variableName + ".evCount",
			this->variableName + ".expMovingAvg",
			this->variableName + ".movingAvg"};
	this->baseLogger->initSignals(vars);

	// record the sampling period that will be used
	this->baseLogger->logSignal(0, this->samplePeriod, this->variableName + ".sample_period");
}

void SamplerLogger::logSignal(double t, double signalValue, const std::string& signalName) { // Logger
	if(signalName != this->variableName) return; // just in case

	double previousT = this->lastT; // save it as it is needed by the expMovingAvg and updated in logSampledValues

	// When a values arrive, they are not recorded immediately. They are recorded when a value out of the sampling period arrives
	// record samples if period elapsed
	if(t >= this->nextSampleT){
		this->logSampledValues(t);
	}

	// values independent of the sampling period
	this->eventCounter++;
	addValueToMovingAvg(signalValue); // moving average
	addValueToExpMovingAvg(signalValue, t, previousT);

	// values for the sampled period
	if(this->lastT != -1 && this->lastT != t){ // If more than one value arrives in the same T, only sample the last one for that T
		this->weightedSumCounter += lastValue * (t - this->lastT);
		this->sumCounter += std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		this->maxCounter = std::fmax(this->maxCounter, this->lastValue);
		this->minCounter = std::fmin(this->minCounter, this->lastValue);
	}

	this->lastT = t;
	this->lastValue = signalValue;
}

void SamplerLogger::logSampledValues(double t){
	double currentTime = this->nextSampleT;
	double totalSampledTime = this->samplePeriod;
	if(t < this->nextSampleT){ // this happens only when flushing
		currentTime = t; // we use the current time when flushing, so be more accurate with the timeAvg sampled
		totalSampledTime = currentTime - (this->nextSampleT - this->samplePeriod) ; // 'currentTime - this->nextSampleT' is the elapsed time since last sample
	}

	// record sample for last value
	if(this->lastT != -1){
		this->sumCounter += std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		double elapsedTimeForValue = currentTime - std::fmax(this->lastT, currentTime - this->samplePeriod); // lastT is lower in case there was no values in the whole samplingPeriod
		this->weightedSumCounter += this->lastValue * elapsedTimeForValue;

		this->maxCounter = std::fmax(this->maxCounter, this->lastValue);
		this->minCounter = std::fmin(this->minCounter, this->lastValue);
	} else { // there was no events in this period
		this->weightedSumCounter = lastValue;
		this->sumCounter = std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		this->maxCounter = lastValue;
		this->minCounter = lastValue;
	}

	// calculate weigthed avg
	double timeAvg = this->weightedSumCounter / totalSampledTime;

	// record samples
	this->baseLogger->logSignal(currentTime, timeAvg, this->variableName + ".timeAvg");
	this->baseLogger->logSignal(currentTime, this->eventCounter, this->variableName + ".evCount");
	this->baseLogger->logSignal(currentTime, this->maxCounter, this->variableName + ".max");
	this->baseLogger->logSignal(currentTime, this->minCounter, this->variableName + ".min");
	this->baseLogger->logSignal(currentTime, this->sumCounter, this->variableName + ".sum");
	this->baseLogger->logSignal(currentTime, this->sumCounter / this->avgCounter, this->variableName + ".avg");
	this->baseLogger->logSignal(currentTime, this->expMovingAvg, this->variableName + ".expMovingAvg");
	this->baseLogger->logSignal(currentTime, this->getMovingAvg(), this->variableName + ".movingAvg");

	// reset
	resetCounters();

	// set next sample time
	this->nextSampleT += this->samplePeriod;

	// record samples again (if more that one period had elapsed)
	this->lastT = currentTime;
	if(t >= this->nextSampleT){
		this->logSampledValues(t);
	}
}

void SamplerLogger::flush(double t) { // Flush
	this->logSampledValues(t); // log last values

	this->baseLogger->flush(t);
}

void  SamplerLogger::resetCounters(){
	this->eventCounter = 0;
	this->weightedSumCounter = 0;
	this->sumCounter = 0;
	this->avgCounter = 0;
	this->maxCounter = this->lastValue;
	this->minCounter = this->lastValue;
}

void SamplerLogger::addValueToMovingAvg(double value){
 if(this->valuesInWindow.size() >= this->movingAvgWindowSize){
	 this->valuesInWindow.erase(this->valuesInWindow.begin()); // remove first (pop_front)
 }

 this->valuesInWindow.push_back(value); // add to back

}

void SamplerLogger::addValueToExpMovingAvg(double value, double currentT, double previousT){
	if(previousT == -1) { // expMovingAvg base case
		this->expMovingAvg = value;
	} else	{ // expMovingAvg
		double alpha = (currentT - previousT) / this->samplePeriod; // alpha = (Tn - Tn-1) / W ; TODO: now getting the moving averange window=sampling period
		this->expMovingAvg = value + exp(-alpha) * (this->expMovingAvg - value); // MA_today = new_value + e^(-alpha) * ( MA_yesterdy - new_value)
	}
}

double SamplerLogger::getMovingAvg(){
	// TODO: this could be implemented much more efficient doing the calculation every time we add a value.
	double sum = 0;
	for (auto val : this->valuesInWindow){
		sum += val;
	}

	return sum / this->valuesInWindow.size(); // sum/N
}
