#include "HDF5Logger.h"
#include "root_coupling.h"

// static members
HighFive::File* HDF5Logger::FILE_HDF5 = nullptr;
int HDF5Logger::refcount = 0;
int HDF5Logger::max_refcount = 0;
int HDF5Logger::signal_count = 0;

HDF5Logger::HDF5Logger(Simulator* model, bool useMultipleSimulationRuns) : HDF5Logger(model, model->getFullName(), useMultipleSimulationRuns)
{
}

HDF5Logger::HDF5Logger(Simulator* model, const std::string& logingName, bool useMultipleSimulationRuns) :
																		model(model), useMultipleSimulationRuns(useMultipleSimulationRuns), logingName(logingName)
{
	std::replace(this->logingName.begin(), this->logingName.end(), '.', '/');
	HDF5Logger::refcount++;
	HDF5Logger::max_refcount = HDF5Logger::refcount;

	printLog(LOG_LEVEL_DEBUG, "Creating HDF5Logger for model %s \n", logingName.data());
}

void HDF5Logger::initSignals(const std::vector<std::string>& signalNames){
	// create it now just in case it fails (don't ruin the simulation at the end)
	std::string filename = this->get_output_filename();
	HDF5Logger::createFile(filename);
	

	printLog(LOG_LEVEL_DEBUG,"HDF5Logger('%s'):  initializing %d signals: \n", this->logingName.data(), signalNames.size());
	std::string fullSignalName;
	for (auto& signalName : signalNames) {
		std::string param = signalName;
		std::replace(param.begin(), param.end(), '.', '/'); // corrected parameter name to enforce use of '/' instead of '.' for model hierarchy
		fullSignalName = this->logingName + "/" + param;
		printLog(LOG_LEVEL_DEBUG,"\t init signal '%s' \n", fullSignalName.data());
		this->signals.insert({param, TimeSerie(fullSignalName)}); // TimeSerie creates the dataset in the HDF5 file
	}
	HDF5Logger::signal_count += this->signals.size();
}

void HDF5Logger::initSignals(const std::map<std::string,int>& variableNamesAndPriorities){

}

void HDF5Logger::logSignal(double t, double signalValue, const std::string& signalName){
	
	std::string param = signalName;
	std::replace(param.begin(), param.end(), '.', '/'); // corrected parameter name to enforce use of '/' instead of '.' for model hierarchy

	if(!this->signals.count(param)){
		//printLog(LOG_LEVEL_FULL_LOGGING ,"HDF5Logger for model '%s': received var '%s' which was not initialized. ignoring.. \n", this->logingName.data(), signalName.data());
		return;
	}

	auto& signal = this->signals.at(param);
	signal.time.push_back(t);
	signal.value.push_back(signalValue);

	// write batch if already fill up buffers
	if(signal.time.size() >= IN_MEMORY_BUFFER){
		HDF5Logger::writeToDataSet(this->get_output_filename(), this->logingName+"/"+param, signal);
		signal.time.clear();
		signal.value.clear();
	}
}

void HDF5Logger::flush(double t)
{
	std::string filename = this->get_output_filename();

	// printing
	if(HDF5Logger::refcount == HDF5Logger::max_refcount){ // first model to flush
		printLog(LOG_LEVEL_ALWAYS,"[%g] HDF5Logger starting flush %d signals to file: %s \n", t, HDF5Logger::signal_count, filename.data());
	}

	// write all time series
	printLog(LOG_LEVEL_DEBUG ,"[%g] HDF5Logger('%s') starting flush %d signals to file: %s \n", t, this->logingName.data(), this->signals.size(), filename.data());
	for(auto pair : this->signals){
		HDF5Logger::writeToDataSet(filename, this->logingName+"/"+pair.first, pair.second);
	}
	this->signals.clear();

	HDF5Logger::refcount--;
	if(HDF5Logger::refcount == 0 && FILE_HDF5 != nullptr) // WARNING: if any client forgets to call flush, the HDF5 file might be kept open
	{
		FILE_HDF5->flush();
		delete FILE_HDF5;
		FILE_HDF5 = nullptr;
		printLog(LOG_LEVEL_ALWAYS ,"[%g] HDF5Logger('%s') completed flushing all models. File is now closed \n", t, this->logingName.data(), this->signals.size(), filename.data());
	}
}

/*static*/ void HDF5Logger::writeToDataSet(
		const std::string& filename,
		const std::string& signalName,
		TimeSerie& timeSerie){
	printLog(LOG_LEVEL_DEBUG,"HDF5Logger appending %d values to dataset %s \n", timeSerie.time.size(), signalName.data());

	if(timeSerie.time.empty()) return;

	try{
		// just in case it was not created before
		HDF5Logger::createFile(filename);

		// get the dataset
		FILE_HDF5->flush(); // flush before loading (is it needed?)
		DataSet aioDataset = FILE_HDF5->getDataSet(signalName);

		// Resize to accomodate new data
		std::vector<size_t> dims = aioDataset.getSpace().getDimensions();
		printLog(LOG_LEVEL_DEBUG,"HDF5Logger resizing dataset from size %lu to size %lu \n", dims[1],  dims[1] + timeSerie.time.size());
		aioDataset.resize({2, dims[1] + timeSerie.time.size()});

		// Write at the end
		std::vector<std::vector<double>> temp = {timeSerie.time, timeSerie.value};
		aioDataset.select({0, dims[1]}, {2, timeSerie.time.size()}).write(temp);		
	}  catch (const std::exception& err) { // catch and print any HDF5 error        
		printLog(LOG_LEVEL_ERROR ,"HDF5Logger exception during writeToDataSet");
		printLog(LOG_LEVEL_ERROR ,"signalName='%s' timeSerie.size()=%d. Error: %s \n", signalName.data(), timeSerie.time.size(), err.what());
        std::cerr << err.what() << std::endl;
		abort();
    } catch(...){
		printLog(LOG_LEVEL_ERROR ,"HDF5Logger UNKNOWN exception during writeToDataSet");
		std::cerr << "HDF5Logger UNKNOWN exception during writeToDataSet" << std::endl;
		abort();
	}
}
std::string HDF5Logger::get_output_filename() {
	if(this->output_file_name.empty()){
		// TODO: we are using the root model name for the output file.
		// Previously a custom command-line parameter was used for this
		// purpose (SimulationName). Using the root model name is better for
		// parameter sweeping purposes from py2pdevs, but not when modeling
		// through the GUI. In that case, a new parameter should be defined
		// and configured by the sweeper model and used here instead of the
		// root model name. A good way to change this code is to always check
		// for this parameter, and fall back to the root model name if it is
		// not present.
		this->output_file_name = this->model->get_top()->getName();
		if (SimulationExperimentTracker::getCurrentSimuRun() >= 0){
			this->output_file_name += "_" + std::to_string(SimulationExperimentTracker::getCurrentSimuRun());
		}

		this->output_file_name += ".h5";
	}
	return this->output_file_name;
}

void HDF5Logger::createFile(const std::string& filename){
	if(FILE_HDF5 == nullptr)
	{
		printLog(1 ,"HDF5Logger: Creating file: %s \n", filename.data());

		// delete the file first, in case the handler is still used by another process
		remove(filename.data());

		// Create a new file using the default property lists.
		FILE_HDF5 = new HighFive::File(filename, File::ReadWrite | File::Create | File::Truncate);		
	}
}

//std::string HDF5Logger::getDataSetName(const H5::DataSet& ds)
//{
//    size_t len = H5Iget_name(ds.getId(),NULL,0);
//    char buffer[len];
//    H5Iget_name(ds.getId(),buffer,len+1);
//    std::string n = buffer;
//    return n;
//}
