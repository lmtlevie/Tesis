#include "toscilab_offline.h"
void toscilab_offline::init(double t,...) {
	va_list parameters;
	va_start(parameters, t);
	time_var = va_arg(parameters,char*);
	signal_var = va_arg(parameters,char*);


	myLoggerToScilab = new ScilabLogger(this, false);

	// logs everything without checking configured logLevel (fast to init, slow to execute if many model)
	this->myLoggerToScilab->initSignals(std::vector<std::string>{"value"
		, "der1"
//		, "der2"
//		, "der3"
//		, "der4"
	});

// logs according to configured logLevel (slow to init, fast to execute if logLevel<<)
//	this->myLoggerToScilab->initSignals(std::map<std::string, int>{
//		{"value", LOG_LEVEL_IMPORTANT},
//		{"value1", LOG_LEVEL_PRIORITY},
//		{"value2", LOG_LEVEL_PRIORITY}
//	});
}
double toscilab_offline::ta(double t) {
	return Sigma;
}
void toscilab_offline::dint(double t) {
	Sigma=4e10;
}
void toscilab_offline::dext(Event x, double t) {
	double* aux=(double*)(x.value);
	myLoggerToScilab->logSignal(t,aux[0], "value");
	myLoggerToScilab->logSignal(t,aux[1], "der1");
//	myLoggerToScilab->logSignal(t,aux[2], "der2");
//	myLoggerToScilab->logSignal(t,aux[3], "der3");
//	myLoggerToScilab->logSignal(t,aux[4], "der4");
}
Event toscilab_offline::lambda(double t) {
	return Event();
}
void toscilab_offline::exit() {
	myLoggerToScilab->flush(99999);
}
