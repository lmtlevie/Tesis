#include "LognormalDistributionParameter.h"

LognormalDistributionParameter::LognormalDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a LognormalDistributionParameter \n");
}

double LognormalDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->lognormal(this->parameterValues[0]/*zeta*/, this->parameterValues[1]/*sigma*/);
}

std::vector<std::string> LognormalDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_ZETA_NAME, PARAMETER_SIGMA_NAME } ;
};

std::string LognormalDistributionParameter::getName() {
	return std::string("LogNormal");
}
