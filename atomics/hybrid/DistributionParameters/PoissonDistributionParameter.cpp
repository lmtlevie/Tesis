#include "PoissonDistributionParameter.h"

PoissonDistributionParameter::PoissonDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a PoissonDistributionParameter \n");
}

PoissonDistributionParameter::PoissonDistributionParameter(double rate) : PoissonDistributionParameter(){
	this->parameterNames = this->getParameterNames();
	this->parameterValues.push_back(rate);
}

double PoissonDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->poisson(this->parameterValues[0]);
}

std::vector<std::string> PoissonDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_RATE_NAME } ;
};

std::string PoissonDistributionParameter::getName() {
	return std::string("Poisson");
}
