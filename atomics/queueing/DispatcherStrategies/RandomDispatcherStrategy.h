#ifndef DISPATCHER_STRATEGIES_RANDOM_STRATEGY_H_
#define DISPATCHER_STRATEGIES_RANDOM_STRATEGY_H_

#include "sinks/ParameterReader.h"
#include "queueing/DispatcherStrategies/IDispatcherStrategy.h"
#include "queueing/Jobs/Job.h"

#include <vector>
#include <queue>
#include <memory>
#include <algorithm>    // std::random_shuffle

/**
 * Join the Shortest Queue strategy.
 * Next job will be assigned to the processor which is currently processing less jobs
 */
class RandomDispatcherStrategy : public IDispatcherStrategy {
public:

	RandomDispatcherStrategy();

	void addProcessor(uint processorId, uint maxCapacity);
	void init();
	void setIdleProcessor(uint processorId, uint newAvailableCapacity);
	int setNextProcessorBusy();

	int getIdleProcessors();

private:
	std::vector<uint> processorIds;
};

#endif /* DISPATCHER_STRATEGIES_RANDOM_STRATEGY_H_ */
