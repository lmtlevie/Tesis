#if !defined HDF5Logger_h
#define HDF5Logger_h

#include <algorithm>
#include "simulator.h"
#include <stdlib.h>
#include "string.h"
#include <stdio.h>
#include <map>
#include <unordered_map>

#include "sink/ParameterReader.h"
#include "sink/SimulationExperimentTracker.h"
#include "IPowerDEVSLogger.h"
#include "utils/options.h"

#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5Reference.hpp>

using namespace HighFive;

/**
 * Writes a HDF5 file with all logged varibles, using a format that can also be loaded by scilab
 *
 *
 * Writing to the file happens in batches of IN_MEMORY_BUFFER.
 * IN_MEMORY_BUFFER should be a trade-off between I/O (performance) and memory used by the simulation depending on the amount of variables logged
 *
 */
class HDF5Logger : public IPowerDEVSLogger {

//		static constexpr int IN_MEMORY_BUFFER  = 65536; //(1MB) Max number of  data kept in memory. After it is dumped to the .h5 file
		static constexpr int IN_MEMORY_BUFFER  = 64; //(1MB) Max number of  data kept in memory. After it is dumped to the .h5 file
//		static constexpr int IN_MEMORY_BUFFER  = 1; // temporary for debugging all values


		// in-memory to store signals. It is later mapped to two h5 datasets
		// 	creates the HDF5 dataset during construction, to avoid problems at the end of the simulation
		struct TimeSerie{
			std::vector<double> time;
			std::vector<double> value;

			TimeSerie(const std::string& signalName){ // pre-reserve memory
				if(FILE_HDF5 == nullptr){ // check hdf5 file is already in place
					printLog(LOG_LEVEL_ERROR, "Attempting to create Timeserie dataset '%s' but FILE_HDF5 was not yet created", signalName.data());
					throw std::runtime_error("Attempting to create a Timeserie dataset but FILE_HDF5 was not yet created");
				}

				time.reserve(IN_MEMORY_BUFFER);
				value.reserve(IN_MEMORY_BUFFER);

//				printLog(LOG_LEVEL_FULL_LOGGING,"HDF5Logger creating dataset for %s \n", signalName.data());

				// Create a 2D dataspace with initial shape and max shape to be able to enlarge later. see: https://github.com/BlueBrain/HighFive/blob/master/src/examples/create_extensible_dataset.cpp
				DataSetCreateProps props;
				DataSpace dataspace = DataSpace({2, 0}, {2, DataSpace::UNLIMITED});
				props.add(Chunking(std::vector<hsize_t>{2, IN_MEMORY_BUFFER})); // Use chunking, is IN_MEMORY_BUFFER the correct chuck size?

				// Create the datasets
				auto _aioDataset = FILE_HDF5->createDataSet<double>(signalName, dataspace, props);
			};
		};


// static class members
private:
	static HighFive::File* FILE_HDF5; // single static instance of the file shared by all loggers
	static int refcount; // number of logger instances, need as last model to flush should close the single static instance of the file
	static int max_refcount; // need as to identify first model to flush (for printing)
	static int signal_count; // counter of total signals (for printing)
	static void createFile(const std::string& filename);
public:
	static void writeToDataSet(const std::string& filename, const std::string& signalName, TimeSerie& data);	

// instance members
private:

	Simulator* model;
	bool useMultipleSimulationRuns;
	std::string logingName;
	std::string output_file_name;

	std::map<std::string, TimeSerie> signals; // <signalName, TimeSerie>

	std::string get_output_filename();

public:
	HDF5Logger(Simulator* model, bool useMultipleSimulationRuns);
	HDF5Logger(Simulator* model, const std::string& baseLogingName, bool useMultipleSimulationRuns);

	void logSignal(double t, double signalValue, const std::string& signalName);
	void flush(double t);

	void initSignals(const std::vector<std::string>& variableNames);
	void initSignals(const std::map<std::string,int>& variableNamesAndPriorities);
};

#endif
