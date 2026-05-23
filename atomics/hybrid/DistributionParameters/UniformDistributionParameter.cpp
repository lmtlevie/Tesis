#include "UniformDistributionParameter.h"

UniformDistributionParameter::UniformDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a UniformDistributionParameter \n");
}

double UniformDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->uniform();
}

std::vector<std::string> UniformDistributionParameter::getParameterNames(){
	return std::vector<std::string> {  } ;
};

std::string UniformDistributionParameter::getName() {
	return std::string("Uniform");
}
