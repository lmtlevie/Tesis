#include "CsvLogger.h"

CsvLogger::CsvLogger(Simulator* model, bool useMultipleSimulationRuns) : CsvLogger(model, model->getFullName(), useMultipleSimulationRuns){

}

CsvLogger::CsvLogger(Simulator* model, const std::string& logingName, bool useMultipleSimulationRuns) :
						model(model), useMultipleSimulationRuns(useMultipleSimulationRuns), logingName(logingName) {
	printLog(LOG_LEVEL_DEBUG, "Creating CsvLogger for model %s \n", logingName.data());
}

// empty implementations
void CsvLogger::initSignals(const std::vector<std::string>& variableNames){

}
void CsvLogger::initSignals(const std::map<std::string,int>& variableNamesAndPriorities){

}

void CsvLogger::logSignal(double t, double signalValue, const std::string& signalName){
	if(!this->files.count(signalName)){
		this->createFile(signalName);
	}

	auto file = this->files[signalName];

	// write
	sprintf(this->buf, "%.8g, %.8g\n", t, signalValue);
	PDFileWrite(file, buf, strlen(buf));
}

void CsvLogger::createFile(const std::string& signalName){
	sprintf(this->buf, "%s.%s.csv", this->logingName.data(), signalName.data());
	this->files[signalName] = PDFileOpen(this->buf, 'w');
}

void CsvLogger::flush(double t){
	// close all files
	for (auto mit = files.begin(); mit != files.end(); mit++) {
		PDFileClose((*mit).second);
	}
}
