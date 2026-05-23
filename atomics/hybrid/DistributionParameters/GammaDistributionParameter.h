#ifndef DistributionParameters_GammaDistributionParameter_H_
#define DistributionParameters_GammaDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class GammaDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_SHAPE_NAME "shape"
#define PARAMETER_SCALE_NAME "scale"

public:
	GammaDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_GammaDistributionParameter_H_ */
