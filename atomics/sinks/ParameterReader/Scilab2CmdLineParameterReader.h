//CPP:sinks/ParameterReader/Scilab2CmdLineParameterReader.cpp

/*
 * Scilab2CmdLineParameterReader.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mbonaven
 */

#ifndef Scilab2CmdLineParameterReader_H_
#define Scilab2CmdLineParameterReader_H_

#include "pdevslib.h"
#include "IParameterReader.h"
#include <fstream>
#include <unordered_set>
#include <iomanip>
#include <limits>



/**
 * Reads parameters from Scilab and writes them to a file for Cmdline format
 * To use it run the simulation as follows: ./model -parameter_reading_backend Scilab2CmdLine -tf 0
 */
class Scilab2CmdLineParameterReader : public IParameterReader {

public:
	// TODO: this should be implemented in another place. Ej: ScilabParameterReader when implemented :-0
	static std::vector<double> ReadVectorFromScilab(const std::string& paramName);

	// TODO: singleton should be implemented from the ParameterReader.cpp once Scilab and Cmdline are IParameterReaders
	static Scilab2CmdLineParameterReader* getInstance(const std::string& cmdFileFullPath){
		if(Scilab2CmdLineParameterReader::Instance == nullptr){
			Scilab2CmdLineParameterReader::Instance = new Scilab2CmdLineParameterReader(cmdFileFullPath);
		}
		return Scilab2CmdLineParameterReader::Instance;
	}

	~Scilab2CmdLineParameterReader() {
		this->outputFile.flush();this->outputFile.close();
	}

protected:
	/**
	 * reads a parameter
	 */
	boost::any readParameterImpl(const std::string& paramName ) ;
	std::vector<double> readParameterVectImpl(const std::string& paramName );

private:
	static Scilab2CmdLineParameterReader* Instance;
	Scilab2CmdLineParameterReader(const std::string& cmdFileFullPath) : IParameterReader() {
			std::cout << "[Scilab2CmdLineParameterReader] Will write the CmdLine parameters file to:" << cmdFileFullPath << std::endl;

			// open file
			this->outputFile.open(cmdFileFullPath, std::ofstream::out | std::ofstream::trunc);
			if(!outputFile.is_open() ){
				 throw std::runtime_error("Scilab2CmdLineParameterReader error: File can not be opened for writing");
			}

			//this->outputFile <<  std::fixed; // this is needed because we are always writing doubles (which might be written in scientific notation), but some are read as int (which can not be read in scientific)
			this->outputFile << std::setprecision(std::numeric_limits<long double>::digits10 + 1); // set maximum precision (together with std::fixed it is very unconfortable to read)
		}

	std::ofstream outputFile;
	std::unordered_set<std::string> writenParameters; // to avoid repeating parameters
	void saveToCmdLineFile(const std::string& paramName, double value);
	void saveToCmdLineFile(const std::string& paramName, std::vector<double> value);
};

#endif /* Scilab2CmdLineParameterReader_H_ */
