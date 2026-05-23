#ifndef DISPATCHER_STRATEGIES_FIFO_STRATEGY_H_
#define DISPATCHER_STRATEGIES_FIFO_STRATEGY_H_

#include "sinks/ParameterReader.h"
#include "queueing/DispatcherStrategies/IDispatcherStrategy.h"
#include "queueing/Jobs/Job.h"

#include <vector>
#include <queue>
#include <memory>
#include <algorithm>    // std::random_shuffle

class FifoDispatcherStrategy : public IDispatcherStrategy {
public:

	FifoDispatcherStrategy();

	void addProcessor(uint processorId, uint maxCapacity);
	void init();
	void setIdleProcessor(uint processorId, uint newAvailableCapacity);
	int setNextProcessorBusy();

	int getIdleProcessors();

private:
	std::deque<uint> idleProcessorIds; // used for RANDOM and FIFO
};

#endif /* DISPATCHER_STRATEGIES_FIFO_STRATEGY_H_ */
