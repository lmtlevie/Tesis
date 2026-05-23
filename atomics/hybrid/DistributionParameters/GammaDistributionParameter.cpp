#include "GammaDistributionParameter.h"

GammaDistributionParameter::GammaDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a GammaDistributionParameter \n");
}

double GammaDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->gamma(this->parameterValues[0]/*shape*/, this->parameterValues[1]/*scale*/);
}

std::vector<std::string> GammaDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_SHAPE_NAME, PARAMETER_SCALE_NAME } ;
};

std::string GammaDistributionParameter::getName() {
	return std::string("Gamma");
}
