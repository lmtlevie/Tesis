#if !defined SamplerLogger_h
#define SamplerLogger_h

#include "sink/ParameterReader.h"
#include "sink/Loggers/IPowerDEVSLogger.h"
#include <math.h>       /* exp */

#include <limits>

/**
 *  The sampler receives values every time a variable is logged (it changes), but logs only every a fixed time period (ej: 1s). When this period elapses the sampler logs the following metrics
    - max: maximum value received during the period
    - min: minimum value received during the period
    - sum: sum of values received during the period
    - avg: average of received values (sum/nValues)
    - timeAvg: time weighted average of the received values
       I.E: timeAvg=SUM_1_N( valueN * etN) / samplingPeriod, where value1.. valueN are the received values, and et1 ..et N are the elapsed time where the valueN was observed.
    - movingAvg: implemented as a EMMA (exponential moving average with alpha=1-exp(-[tN - tN-1]/W). MA_today = new_value + e^(-alpha) * ( MA_yesterdy - new_value)

 For all previous metrics, the Sampler only records one value per timestamp. I.E: 2 values for the same simulated time, it only records the last one

    - evCount: count of calls to logSignal, even it they are in the same timestamp


  UN IMPLEMENTED NOTES:
   * There are 2 different type of variables we can sample:
 * 1) continuous signals, which send an event when there is change in value (if there are no events it is assumed the value remained the same).
 *    When there is a period change, the next period should assume the value is kept until next event arrives
 *       2 values generated in the same simulated time represent the value changed to the last value. The first value is not considered in any metric
 *       sum does not really make sense, it should be a time-weighted sum or integral
 *       timeAvg makes more sense than avg
 *       min, max take into account the value from the previous sample (the value does not change)
 *
 * 2) discrete values, which send an event when a value is logged (between 2 consecutive events nothing can be assumed about the value of the variable)
 *     When there is a period change, the next period can not assume anything about the value of the variable until next event arrives
 *        all metrics only consider values from the current period
 *        min, max are NaN if no values occur in the period
 *        sum, avg, timeAvg are 0 if no values occur in the period
 */
class SamplerLogger : public IPowerDEVSLogger {

#define SAMPLER_PERIOD_VARIABLE_NAME "sample_period"


private:
	// parameters
	double samplePeriod = 0;

	// State Variables
	double lastT = -1;
//	double lastSampleT = 0; // records the last time a sample was logged (so that last log can be weighted accurately)
	double nextSampleT = -1;

	// counters
	double weightedSumCounter = 0;
	double lastValue = std::numeric_limits<double>::quiet_NaN();
	double maxCounter  = std::numeric_limits<double>::quiet_NaN();
	double minCounter  = std::numeric_limits<double>::quiet_NaN();
	double sumCounter  = 0;
	double avgCounter = 0;
	uint eventCounter  = 0;
	double expMovingAvg = 0;
	std::vector<double> valuesInWindow;


	// base logger to use
	Simulator* model;
	std::shared_ptr<IPowerDEVSLogger> baseLogger;
	std::string variableName; // name of the variable we are sampling

public:
	double movingAvgWindowSize = 10000; // TODO: get this by a configuration parameter

	/*! read sample period from configuration using model's name */
	SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger);
	SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger, double samplePeriod);
	void initSignals(const std::vector<std::string>& variableNames);
	void initSignals(const std::map<std::string,int>& variableNamesAndPriorities);
	void logSignal(double, double, const std::string&);
	void flush(double t);

private:
	void resetCounters();
	void logSampledValues(double t);

	void addValueToExpMovingAvg(double value, double currentT, double previousT); // expMovingAvg
	void addValueToMovingAvg(double value); // moving avg
	double getMovingAvg();


};

#endif
