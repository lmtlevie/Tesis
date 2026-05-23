#if !defined ConfigurationLogger_h
#define ConfigurationLogger_h

#include "sinks/ParameterReader.h"
#include "SamplerLogger.h"
#include "DiscreteSamplerLogger.h"

#define LOGGER_ID_VARIABLE_NAME "logger"

enum CONFIGURATION_LOGGERS { ALL, SAMPLER, DISCRETE_SAMPLER/*HISTOGRAM*/};

#define DEFAULT_LOGGER_ID CONFIGURATION_LOGGERS::ALL

class ConfigurationLogger : public IPowerDEVSLogger {
	private:
		Simulator* model;
		std::map<std::string, std::shared_ptr<IPowerDEVSLogger>> variableLogger;

	public:

		ConfigurationLogger(Simulator* model);
		void initSignals(const std::vector<std::string>& variableNames);
		void initSignals(const std::map<std::string,int>& variableNamesAndPriorities);
		void logSignal(double, double, const std::string&);
		void flush(double t);
	
	private:
		std::shared_ptr<IPowerDEVSLogger> getLogger(int loggerId, const std::string& varName);
		void createLoggerForEachVariable(const std::vector<std::string>& variableNames);

};

#endif
