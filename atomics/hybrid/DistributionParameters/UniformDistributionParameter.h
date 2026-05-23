//CPP:hybrid/DistributionParameters/NormalDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_UniformDistributionParameter_H_
#define DistributionParameters_UniformDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class UniformDistributionParameter : public BaseDistributionParameter {

public:
	UniformDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_UniformDistributionParameter_H_ */
