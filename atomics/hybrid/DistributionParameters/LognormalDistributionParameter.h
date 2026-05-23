//CPP:hybrid/DistributionParameters/LognormalDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_LognormalDistributionParameter_H_
#define DistributionParameters_LognormalDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class LognormalDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_ZETA_NAME "zeta"
#define PARAMETER_SIGMA_NAME "sigma"

public:
	LognormalDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_LognormalDistributionParameter_H_ */
