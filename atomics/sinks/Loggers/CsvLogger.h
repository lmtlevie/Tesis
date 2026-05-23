#if !defined CsvLogger_h
#define CsvLogger_h

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

/**
 * Logs to a cvs file
 */
class CsvLogger : public IPowerDEVSLogger {
	Simulator* model;
	bool useMultipleSimulationRuns;
	std::string logingName;

	std::map<std::string, long int> files; // <signalName, fileHandler>

	char buf[258]; // temporary writing buffer (instance var so as not to reserve memory each time)

public:

	CsvLogger(Simulator* model, bool useMultipleSimulationRuns);
	CsvLogger(Simulator* model, const std::string& baseLogingName, bool useMultipleSimulationRuns);

	void logSignal(double t, double signalValue, const std::string& signalName);
	void flush(double t);

	void initSignals(const std::vector<std::string>& variableNames);
	void initSignals(const std::map<std::string,int>& variableNamesAndPriorities);
private:
	void createFile(const std::string& signalName);
};

#endif
