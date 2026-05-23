#ifndef DISPATCHER_STRATEGIES_JSQ_STRATEGY_H_
#define DISPATCHER_STRATEGIES_JSQ_STRATEGY_H_

#include "sinks/ParameterReader.h"
#include "queueing/DispatcherStrategies/IDispatcherStrategy.h"
#include "ProcessorLoad.h"
#include "queueing/Jobs/Job.h"

#include <vector>
#include <queue>
#include <memory>

/**
 * Join the Shortest Queue strategy.
 * Next job will be assigned to the processor which is currently processing less jobs
 */
class JsqDispatcherStrategy : public IDispatcherStrategy {

private:
	std::vector<std::shared_ptr<ProcessorLoad>> processors; // TODO: // a fixed sized-array would be more efficient.
	uint totalIdleProcessors = 0; // only used to return the count in O(1)
public:

	JsqDispatcherStrategy();

	void addProcessor(uint processorId, uint maxCapacity);
	void init();
	void setIdleProcessor(uint processorId, uint newAvailableCapacity);
	int setNextProcessorBusy();

	int getIdleProcessors();

private:
	void printLoad(int logLevel);

	std::vector<std::shared_ptr<ProcessorLoad>>::iterator findDcm(int processorId){
			return std::find_if(this->processors.begin(), this->processors.end(), [&](std::shared_ptr<ProcessorLoad> const& p) {
				return p->getProcessorId() == processorId;
			});
	}
};

#endif /* DISPATCHER_STRATEGIES_Jsp_STRATEGY_H_ */
