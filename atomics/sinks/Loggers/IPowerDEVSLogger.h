/*
 * IPowerDEVSLogger.h
 *
 *  Created on: Jun 3, 2015
 *      Author: mbonaven
 */

#ifndef I_POWERDEVS_LOGGER_H_
#define I_POWERDEVS_LOGGER_H_

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

/**
 * Abstract class for PowerDEVS loggers. Known implementations: OpenTsdbLogger and aLoggerToScilab
 */
class IPowerDEVSLogger {
public:
	virtual void initSignals(const std::vector<std::string>& variableNames)=0;
	virtual void initSignals(const std::map<std::string,int>& variableNamesAndPriorities)=0;
	virtual void logSignal(double t, double signalValue, const std::string& signalName) =0;
	virtual void flush(double t) =0;

	virtual ~IPowerDEVSLogger() { }
};

#endif /* IPowerDEVSLogger_H_ */
