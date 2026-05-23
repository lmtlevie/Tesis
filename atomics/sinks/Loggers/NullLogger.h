#if !defined NullLogger_h
#define NullLogger_h

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

#include "general/BaseSimulator.h"

/**
 * Just ignores all logs
 */
class NullLogger : public IPowerDEVSLogger {

	public:

		NullLogger(Simulator* model, bool useMultipleSimulationRuns){};
		NullLogger(Simulator* model, const std::string& baseLogingName, bool useMultipleSimulationRuns){};

		/**
		 * Initializes all variables without checking the logLevel
		 */
		void initSignals(const std::vector<std::string>& variableNames){};


		void initSignals(const std::map<std::string,int>& variableNamesAndPriorities){};
		void logSignal(double, double, const std::string&){};
		void flush(double t){};
};

#endif
