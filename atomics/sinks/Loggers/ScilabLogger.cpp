#include "ScilabLogger.h"

int ScilabLogger::TotalFlushingTime_ms = 0;
int ScilabLogger::TotalWrittingTime_us = 0;
int ScilabLogger::TotalMetricPoints = 0;
int ScilabLogger::TotalLoggedVariables = 0;
int ScilabLogger::TotalLoggedModels = 0;

ScilabLogger::ScilabLogger(Simulator* model, bool useMultipleSimulationRuns) : ScilabLogger(model, model->getFullName(), useMultipleSimulationRuns){

}

ScilabLogger::ScilabLogger(Simulator* model, const std::string&  logingName, bool useMultipleSimulationRuns) :
								model(model), useMultipleSimulationRuns(useMultipleSimulationRuns), logingName(logingName), modelWrittingTime_us(0), modelMetricPoints(0) {
	printLog(LOG_LEVEL_DEBUG, "Creating ScilabLogger for model %s \n", logingName.data());
}

/**
 * Initializes all variables without checking the logLevel
 */
void ScilabLogger::initSignals(const std::vector<std::string>& variableNames) {
	// Check priorities, assuming ALWAYS
	//	std::map<std::string,int> variableNamesAndPriorities;
	//	for (auto const & name : variableNames){
	//		variableNamesAndPriorities[name] = LOG_LEVEL_ALWAYS;
	//	}
	//
	//	this->initSignals(variableNamesAndPriorities);
	this->initSignals_internal(variableNames);
}

void ScilabLogger::initSignals(const std::map<std::string,int>& variableNamesAndPriorities){
	// filter variables which do not have enough importance
	std::vector<std::string> filteredVariableNames;
	int variableLogLevel;
	for(auto var : variableNamesAndPriorities){
		// read log level from config for this model variable
		variableLogLevel = readLogLevel(this->model, var.first);

		//std::cout << "Log level " << variableLogLevel << " for variable " << var.first <<"\n";
		if(variableLogLevel >= var.second){
			filteredVariableNames.push_back(var.first);
		}
	}

	this->initSignals_internal(filteredVariableNames);
}

void ScilabLogger::initSignals_internal(const std::vector<std::string>& variableNames) {
	ScilabLogger::TotalLoggedVariables += variableNames.size() + 1;
	ScilabLogger::TotalLoggedModels ++;

	// if there is nothing to log finish here
	if(variableNames.size() == 0) return;

	for (auto varName : variableNames) {
		// generates a file name to log data
		const char* fileName = this->createFile();
		printLog(1,"ScilabLogger: Using file %s for model.variable=%s.%s\n", fileName, this->logingName.data(), varName.data());

		this->signalNames[varName] = std::make_shared<SignalInfo>(varName, fileName);
	}
}

void ScilabLogger::logSignal(double t, double signalValue, const std::string& signalName) { // Logger
	auto initWrittingTime = std::chrono::system_clock::now();

	if(!this->signalNames.count(signalName)){
		printLog(LOG_LEVEL_FULL_LOGGING ,"ScilabLogger for model '%s': received var '%s' which was not initialized. ignoring.. \n", this->logingName.data(), signalName.data());
		return;
	}
	auto info = this->signalNames[signalName];

	printLog(LOG_LEVEL_FULL_LOGGING,"ScilabLogger for model '%s': received var '%s' with value=%f and timestamp %f \n", this->logingName.data(), signalName.data(), signalValue, t);

	// log  vars only log once per timestamp
	if(t > info->lastTime){ // if timestamp changed, log
		info->logCount++;

		// log last stored value
		int file = this->openFile(info->fileName);
		write(file, (char*) &info->lastTime, sizeof(double)); // write previous time value to the file
		write(file, (char*) &info->lastState, sizeof(double)); // write previous var value to the file
		close(file);
	}

	// Updates this signal
	this->signalNames[signalName]->lastState = signalValue;
	this->signalNames[signalName]->lastTime = t;

	// log writting times
	int writtingTime_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - initWrittingTime).count();
	ScilabLogger::TotalWrittingTime_us += writtingTime_us;
	this->modelWrittingTime_us += writtingTime_us;

	this->modelMetricPoints++;
	ScilabLogger::TotalMetricPoints++;
}

void ScilabLogger::flush(double t) { // Flush
	auto initFlushingTime = std::chrono::system_clock::now();

	char command[1024]; memset(command, 0x00, 1024);
	char multipleSimulationsIndex[1024]; memset(multipleSimulationsIndex, 0x00, 1024);

	printLog(LOG_LEVEL_FULL_LOGGING,"\n\n-----------  SIMULATION FINISED (logging %u variables for model %s) ----------------------\n ", this->signalNames.size(), this->logingName.data());

	// read values from file (TODO: check possible errors)
	for(auto search: this->signalNames){
		auto info = search.second;
		std::string signalName =  search.first;
		printLog(LOG_LEVEL_DEBUG,"ScilabLogger::flush - starting to log for %s.%s \n", this->logingName.data(), signalName.data());

		// log last stored value
		this->logSignal(INF,INF, signalName);
		this->modelMetricPoints--;
		ScilabLogger::TotalMetricPoints--;

		printLog(LOG_LEVEL_DEBUG,"ScilabLogger::flush - info->fileName=%s \n", info->fileName);
		int file = this->openFile(info->fileName);
		lseek(file, 32, SEEK_SET);
		write(file, &info->logCount, sizeof(int)); // write final number of rows
		close(file);

		// Load file and set variables in Scilab
		sprintf(command, "load %s ", info->fileName);
		printLog(LOG_LEVEL_DEBUG,"command = %s \n", command);
		executeVoidScilabJob(command, true);

		// TEMP //TODO: review if it is really necessary
		sprintf(command, "tempdevs1= [ 102 105 108 101 40 39 99 108 111 115 101 39 44 102 105 108 101 40 41 41 32]; execstr(char(tempdevs1)); clear tempdevs1;");
		executeVoidScilabJob(command, true);
		printLog(LOG_LEVEL_DEBUG,"command = %s \n", command);

		// To preserve the temporal matrix on Scilab (for debugging)
		// sprintf(command,"tempdevs_%s=tempdevs;",this->logingName.data());
		// executeVoidScilabJob(command,true);

		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: CURRENT RUN number of logs: %u . \n", info->logCount);

		// handle multiple simulation runs
		int cols = -1;

		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: getCurrentSimuRun = %u   \n", SimulationExperimentTracker::getCurrentSimuRun());
		if (this->useMultipleSimulationRuns && SimulationExperimentTracker::getCurrentSimuRun() >= 1 ) {
			printLog(LOG_LEVEL_DEBUG,"[DEBUG]: USING MULTIPLE SIMULATIONS!!! Simu N = %u  \n", SimulationExperimentTracker::getCurrentSimuRun());
			double *parametersMeta = new double[2];

			// get size of variable matrix with previous simulations
			sprintf(command, "size(" SCILAB_TIME_VARIABLE_NAME_FORMAT ")", this->logingName.data(), signalName.data());
			getScilabVector(command, &cols, parametersMeta);
			//rows = parametersMeta[0];
			cols = parametersMeta[1];

			//printLog(LOG_LEVEL_DEBUG,"[DEBUG]: variable %s has rows:%u and cols:%u . \n", signalNames[TIME_VARIABLE_NAME].name, rows, cols);

			// resize matrix(previous simulations) or array (current simulation) so that they match number of columns
			if (cols > info->logCount) { // resize array
				//printLog(LOG_LEVEL_DEBUG,"[DEBUG]: resizing   tempdevs \n");
				sprintf(command, "tempdevs(:, %u:%u) = %%nan;", info->logCount+1, cols);
				executeVoidScilabJob(command, true);
			} else if (cols < info->logCount) { // resize matrix
				// must resize all variables, so it is done afterwards before setting value
			}

			// set indexer
			sprintf(multipleSimulationsIndex, "(%u,:)", SimulationExperimentTracker::getCurrentSimuRun()+1);
		}

		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: multipleSimulationsIndex = %s \n", multipleSimulationsIndex);

		// resize variable to match with matrix number of columns
		if (this->useMultipleSimulationRuns && cols < info->logCount && SimulationExperimentTracker::getCurrentSimuRun() >= 1) {
			sprintf(command,  SCILAB_VARIABLE_NAME_FORMAT "(:, %u:%u)=%%nan;", this->logingName.data(), signalName.data(), cols+1, info->logCount);
			executeVoidScilabJob(command, true);
			sprintf(command,  SCILAB_TIME_VARIABLE_NAME_FORMAT "(:, %u:%u)=%%nan;", this->logingName.data(), signalName.data(), cols+1, info->logCount);
			executeVoidScilabJob(command, true);
		}

		// set variable value and time
		sprintf(command, SCILAB_VARIABLE_NAME_FORMAT "%s=tempdevs(%i,:);", this->logingName.data(), signalName.data(), multipleSimulationsIndex, 2);
		printLog(LOG_LEVEL_DEBUG,"command = %s \n", command);
		executeVoidScilabJob(command, true);
		sprintf(command, SCILAB_TIME_VARIABLE_NAME_FORMAT "%s=tempdevs(%i,:);", this->logingName.data(), signalName.data(), multipleSimulationsIndex, 1);
		executeVoidScilabJob(command, true);
		printLog(LOG_LEVEL_DEBUG,"command = %s \n", command);
	}
	// Comment the following line to preserve the temporary file (for debugging)
	//deleteFile(fileName);

	// Log flushing times
	int flushingTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - initFlushingTime).count();
	ScilabLogger::TotalFlushingTime_ms += flushingTime_ms;

	printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s flushing time: %i (ms) \n ", this->logingName.data(), flushingTime_ms);
	printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s writing time: %i (us) \n ", this->logingName.data(), this->modelWrittingTime_us);
	printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s logged metric points (not including T) : %i  \n ", this->logingName.data(), this->modelMetricPoints);
}

int ScilabLogger::openFile(const char* fileName) {
	int file = -1;
	if (getOs() == WINDOWS)
		file = open(fileName, O_WRONLY  | O_BINARY, 0666);
	else
		file = open(fileName, O_WRONLY , 0666);

	if(file == -1){
		printLog(LOG_LEVEL_ERROR,"Unable to open file %s for model %s\n", fileName, this->logingName.data());
	}

	lseek(file, 0, SEEK_END);

	return file;
}

const char* ScilabLogger::createFile() {
	char* fileName = new char[1024];
	tmpnam(fileName); // generates a random file name

	// open file in mode create
	int file = -1;
	if (getOs() == WINDOWS){
		file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
	} else {
		file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	}

	if(file == -1){
		printLog(LOG_LEVEL_ERROR,"Unable to open file %s for model %s\n", fileName, this->logingName.data());
	}

	// Write header and close
	char name[24] = { 29, 14, 22, 25, 13, 14, 31, 28, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
	write(file, name, 24);


	// create a 2x1 matrix (time and value
	int type = 1;
	write(file, &type, sizeof(int)); // FLOATING MATRX TYPE 1
	int cols = 2;
	write(file, &cols, sizeof(int)); // 2 INITIAL COLUMNS (value and time)
	int rows = 2;
	write(file, &rows, sizeof(int)); // 2 INITIAL ROWS
	short s = 0;
	for (int i = 1; i <= rows; i++) { // INICIALIZA A 0
		write(file, &s, sizeof(short));
	}

	close(file);

	return fileName;
}

void ScilabLogger::deleteFile(char* fileName) {
	struct stat info;

	//printLog(LOG_LEVEL_DEBUG,"ScilabLogger: deleting file %s \n",fileName);

	stat(fileName, &info);
	if (S_ISREG(info.st_mode) == 1) { // existe el archivo de datos, es necesario borrarlo.
		if (chmod(fileName, S_IRWXU) != 0)
			printLog(LOG_LEVEL_ERROR,"ScilabLogger: CHMOD Error: %s\n", strerror(errno));
		else {
			stat(fileName, &info);
			//printLog(10,"ScilabLogger: CHMOD permissions are: %08x\n", info.st_mode);
		}

		if (remove(fileName) != 0)
			printLog(LOG_LEVEL_ERROR,"ScilabLogger: REMOVE %s Error: %s\n", fileName, strerror(errno)); //Da error cuando el File tiene parametro de Solo Lectura
		else
			printLog(LOG_LEVEL_FULL_LOGGING,"ScilabLogger: REMOVE %s Successful!\n", fileName);
	}
}

