//CPP:sinks/ParameterSweeper/ScilabParameterSweeper.cpp

#ifndef ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_
#define ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_

#include "pdevslib.h"
#include "sinks/ParameterReader.h"
#include "sinks/ParameterSweeper/IParameterSweeper.h"
#include "sinks/SimulationExperimentTracker.h"

/**
 * TODO: unimplemented class. To implement it you should base on ScilabSimulationTracker
 */
class ScilabParameterSweeper : public IParameterSweeper {
public:
	ScilabParameterSweeper(const std::string& simuRunVariableName, const std::string& parameterValuesVariableName, const std::string& parameterNamesVariableName) :
		IParameterSweeper(simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName) {
//		printLog(LOG_LEVEL_FULL_LOGGING, "ScilabParameterSweeper: unimplemented class. To implement it you should base on ScilabSimulationTracker\n");
//		throw std::runtime_error("unimplemented class. To implement it you should base on ScilabSimulationTracker");
	}

	/*
	 * Returns current run number based on the name for simuRunVariableName
	 */
	int getCurrentExperimentNumber();

	/*
	 * Returns total number of runs
	 */
	int getTotalNumberOfExperiments();

	/*
	 * Updates the parameters according to the currentExperimentNumber, parameterValuesVariableName and parameterNamesVariableName
	 */
	void UpdateParameters();

	void onExit();
private:
	void replaceParameterValue(const std::string& parameterName, const std::string& parameterValue);

};

#endif /* ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_ */
