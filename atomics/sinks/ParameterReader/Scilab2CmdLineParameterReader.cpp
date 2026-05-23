/*
 * Scilab2CmdLineParameterReader.cpp
 *
 *  Created on: Mar 8, 2018
 *      Author: mbonaven
 */

#include "Scilab2CmdLineParameterReader.h"

Scilab2CmdLineParameterReader* Scilab2CmdLineParameterReader::Instance = nullptr;

boost::any Scilab2CmdLineParameterReader::readParameterImpl(const std::string& paramName )  {
	//std::cout << "[Scilab2CmdLineParameterReader::readParameterImpl] Reading parameter:" << paramName << std::endl;
	// read value
	double value = getScilabVar(paramName.data(), true);

	// save it
	this->saveToCmdLineFile(paramName, value);

	return value;
}

std::vector<double> Scilab2CmdLineParameterReader::readParameterVectImpl(const std::string& paramName ) {
	std::vector<double> value = Scilab2CmdLineParameterReader::ReadVectorFromScilab(paramName);

	// save it
	this->saveToCmdLineFile(paramName, value);

	return value;
}

void Scilab2CmdLineParameterReader::saveToCmdLineFile(const std::string& paramName, double value) {
	//std::cout << "[Scilab2CmdLineParameterReader::saveToCmdLineFile] writing to file. " << paramName << '=' << value << std::endl;
	auto search = this->writenParameters.find(paramName);
	if(search != this->writenParameters.end()) return; // parameter already written

	if(!outputFile.is_open() ){
		 throw std::runtime_error("Scilab2CmdLineParameterReader error: File was not accesible any more");
	}

	// write to file
	this->outputFile << paramName << '=' << value <<  std::endl;

	// remember
	this->writenParameters.insert(paramName);
}

void Scilab2CmdLineParameterReader::saveToCmdLineFile(const std::string& paramName, std::vector<double> vec) {
	if(vec.size()==0) return;

	//std::cout << "[Scilab2CmdLineParameterReader::saveToCmdLineFile] writing to file. " << paramName << '=' << value << std::endl;
	auto search = this->writenParameters.find(paramName);
	if(search != this->writenParameters.end()) return; // parameter already written

	if(!outputFile.is_open() ){
		 throw std::runtime_error("Scilab2CmdLineParameterReader error: File was not accesible any more");
	}

	// write to file
	this->outputFile << paramName << "= {" << vec[0];
	for(int i = 1; i < vec.size(); i++){
		this->outputFile << ", "<< vec[i];
	}
	this->outputFile << "}" <<  std::endl;

	// remember
	this->writenParameters.insert(paramName);
}


std::vector<double> Scilab2CmdLineParameterReader::ReadVectorFromScilab(std::string const&  paramName){
	char command[1024];
	double *vectorMeta = new double [2];
	int rows, cols;

	// get size of vector
	sprintf(command,"size(%s)",paramName.data());
	getScilabVector(command, &cols, vectorMeta );
	rows = vectorMeta[0];
	cols = vectorMeta[1];

	// validate it is not a matrix
	if(rows != 1) {
		// check that variable exists-
		double f;
		sprintf(command,"exists('%s')", paramName.data());
		executeScilabJob(command,false);
		getAns(&f,1,1);
		if (f==0) {
			printLog("Variable %s does not exists! \n", paramName.data());
			throw std::runtime_error("Scilab Variable does not exists!");
		} else {
			printLog(0,"reading matrices from scilab is not supported. You must Implement getScialbMatrix in globals.h");
			throw std::runtime_error("reading matrices from scilab not supported. You must Implement getScialbMatrix in globals.h");
		}
	}

	// reserve memory // TODO probably can be done directly with std::vector()[0] as it reserves contiguous memory
	double *vector = new double[cols];
//	sprintf(command,"%s",paramName.data());
	getScilabVector(paramName.data(), &cols, vector );

	// create a std::vector
	std::vector<double> ret(vector, vector + cols);

	// release memory
	delete vector;

	return ret;
}
