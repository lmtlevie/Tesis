//CPP:hybrid/DistributionParameters/PoissonDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_PoissonDistributionParameter_H_
#define DistributionParameters_PoissonDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class PoissonDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_RATE_NAME "rate"

public:
	PoissonDistributionParameter();
	PoissonDistributionParameter(double mu);

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_PoissonDistributionParameter_H_ */
