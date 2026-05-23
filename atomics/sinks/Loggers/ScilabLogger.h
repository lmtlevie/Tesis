#if !defined ScilabLogger_h
#define ScilabLogger_h

// pre defined Scilab  logging levels
#if !defined SCILAB_LOG_LEVEL
#define SCILAB_LOG_LEVEL_ALWAYS 10
#define SCILAB_LOG_LEVEL_IMPORTANT 100
#define SCILAB_LOG_LEVEL_PRIORITY 1000
#define SCILAB_LOG_LEVEL_DEBUG 9999999

// LOG_LEVEL=0 no logging; LOG_LEVEL=1 only important logs; ... LOG_LEVEL=9999 log everything
#define SCILAB_LOG_LEVEL SCILAB_LOG_LEVEL_ALWAYS
#endif

#include "simulator.h"
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include <vector>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctime>

#include "sink/ParameterReader.h"
#include "sink/SimulationExperimentTracker.h"
#include "IPowerDEVSLogger.h"

class ScilabLogger : public IPowerDEVSLogger {
#define BACKUP_DIRECTORY "ScilabLogs"
#define TIME_VARIABLE_NAME "t"
#define SCILAB_VARIABLE_NAME_FORMAT "%s.%s.value" // modelName.variableName
#define SCILAB_TIME_VARIABLE_NAME_FORMAT "%s.%s.t" // modelName.variableName

public:

	// Logging Metric (for debugging)
	static int TotalFlushingTime_ms; // total ms which all scilab loggers took to flush
	static int TotalWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
	static int TotalMetricPoints; // total ms which all scilab loggers took to write logs to disk
	static int TotalLoggedVariables; // total ms which all scilab loggers took to write logs to disk
	static int TotalLoggedModels; // total ms which all scilab loggers took to write logs to disk

	ScilabLogger(Simulator* model, bool useMultipleSimulationRuns);
	ScilabLogger(Simulator* model, const std::string& baseLogingName, bool useMultipleSimulationRuns);

	/**
	 * Initializes all variables without checking the logLevel
	 */
	void initSignals(const std::vector<std::string>& variableNames);

	/**
	 * Initializes variables checking the logLevel, which might be slow  reading logLevel from scilab
	 */
	void initSignals(const std::map<std::string,int>& variableNamesAndPriorities);
	void logSignal(double, double, const std::string&);
	void flush(double t);

private:
	class SignalInfo {
	public:
		double lastState;
		double lastTime;
		const std::string name;
		const char* fileName;
		int logCount;

		SignalInfo(const std::string& name, const char* fileName):
			lastState(0), lastTime(std::numeric_limits<double>::max()), name(name), fileName(fileName), logCount(0)
		{};

		bool operator==(const SignalInfo& other) const {
			return name == other.name;
		};
	} ;

	void initSignals_internal(const std::vector<std::string>& variableNames);
	int openFile(const char* fileName);
	const char* createFile();
	void deleteFile(char* fileName);

	Simulator* model;
	bool useMultipleSimulationRuns;
	std::string logingName;
	std::map<std::string, std::shared_ptr<SignalInfo>> signalNames;

	// Logging Metric (for debugging)
	int modelWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
	int modelMetricPoints; // total ms which all scilab loggers took to write logs to disk


};

#endif
