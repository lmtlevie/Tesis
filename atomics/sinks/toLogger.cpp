#include "toLogger.h"
void toLogger::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters, t);

	// logs everything without checking configured logLevel (fast to init, slow to execute if many model)
	this->logger->initSignals(std::vector<std::string>{"value"
		, "value1"
//		, "value2"
//		, "value3"
//		, "value4"
	});

// logs according to configured logLevel (slow to init, fast to execute if logLevel<<)
//	this->myLoggerToScilab->initSignals(std::map<std::string, int>{
//		{"value", LOG_LEVEL_IMPORTANT},
//		{"value1", LOG_LEVEL_PRIORITY},
//		{"value2", LOG_LEVEL_PRIORITY}
//	});
}

void toLogger::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
}

void toLogger::dext(Event x, double t) {
	double* aux=(double*)(x.value);

	debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s::dext: arrived signal with value=[%g][%g][%g]\n", t, this->getFullName().data(), aux[0], aux[1], aux[2]);

	this->logger->logSignal(t,aux[0], "value");
	this->logger->logSignal(t,aux[1], "value1");
//	myLoggerToScilab->logSignal(t,aux[2], "value2");
//	myLoggerToScilab->logSignal(t,aux[3], "value3");
//	myLoggerToScilab->logSignal(t,aux[4], "value4");
}

Event toLogger::lambda(double t) {
	return Event();
}
