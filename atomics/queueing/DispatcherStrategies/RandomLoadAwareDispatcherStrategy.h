#ifndef DISPATCHER_STRATEGIES_RANDOM_LOAD_AWARE_STRATEGY_H_
#define DISPATCHER_STRATEGIES_RANDOM_LOAD_AWARE_STRATEGY_H_

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
class RandomLoadAwareDispatcherStrategy : public IDispatcherStrategy {
public:

	RandomLoadAwareDispatcherStrategy();

	void addProcessor(uint processorId, uint maxCapacity);
	void init();
	void setIdleProcessor(uint processorId, uint newAvailableCapacity);
	int setNextProcessorBusy();

	int getIdleProcessors();

private:
	std::deque<uint> idleProcessorIds; // used for RANDOM and Jsp
};

#endif /* DISPATCHER_STRATEGIES_RANDOM_STRATEGY_H_ */
