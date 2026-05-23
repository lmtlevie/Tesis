//CPP:sinks/ParameterReader/IParameterReader.cpp

/*
 * IParameterReader.h
 *
 *  Created on: Mar 8, 2018
 *      Author: mbonaven
 */

#ifndef I_PARAMETER_READER_H_
#define I_PARAMETER_READER_H_

#include <string>
#include <iostream>
#include <boost/any.hpp>
#include <vector>

/**
 * Abstract class for Parameter Readers.
 *  Known implementations: TODO: ScilabParameterReader, CmdLineParameterReader
 */
class IParameterReader {
public:
	IParameterReader(){ }
	virtual ~IParameterReader() { }

	/**
	 * read vector parameters. Only works for std::vector<double>
	 * Uses this trick to have different methods to handle scalars and vectors. to call: reader->readParameter(paramName, static_cast<T*>(0));
	 */
	template<typename T>
	std::vector<T> readParameter(const std::string& paramName, std::vector<T> *){
		//throw std::runtime_error("IParameterReader error: unable to cast parameter to double. Vector not supported yet. Parameter:" + paramName);
		return this->readParameterVectImpl(paramName);
	}

	/**
	 * read scalar parameters
	 */
	template<typename T>
	T readParameter(const std::string& paramName, T*){
		boost::any res = this->readParameterImpl( paramName ); // see: https://stackoverflow.com/questions/7968023/c-virtual-template-method
		double result = -1;
		try {
			result = boost::any_cast<double>( res ); // always cast to double ;-(
		} catch(const boost::bad_any_cast &) {
			throw std::runtime_error("[IParameterReader::readParameter] error: unable to cast parameter to double. Vector not supported yet. Parameter:" + paramName);
		}

//		std::cout << "[IParameterReader::readParameter] Parameter "<< paramName << "=" <<  result << std::endl;
		return result;
	}

protected:
	/**
	 * reads a scalar parameter
	 */
	virtual boost::any readParameterImpl(const std::string& paramName )  = 0;

	/**
	 * reads a vector parameter
	 */
	virtual std::vector<double> readParameterVectImpl(const std::string& paramName )  = 0;
};

#endif /* IParameterReader_H_ */
