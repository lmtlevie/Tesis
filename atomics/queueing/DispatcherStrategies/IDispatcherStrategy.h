#ifndef I_DispatcherStrategies_DispatcherStrategies_H_
#define I_DispatcherStrategies_DispatcherStrategies_H_

#include <queue>
#include <memory>

/**
 * Abstract class for to represent different Dispatcher assignment strategies
 */
class IDispatcherStrategy {
public:
	IDispatcherStrategy() { }

	virtual ~IDispatcherStrategy() { }

	virtual void addProcessor(uint processorId, uint maxCapacity) =0;
	virtual void init() =0;
	virtual void setIdleProcessor(uint processorId, uint newAvailableCapacity) =0;
	virtual int setNextProcessorBusy() =0;

	virtual int getIdleProcessors() =0;
};

#endif /* I_HltsvStrategies_HLTSV_STRATEGY_H_ */
