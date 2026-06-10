//CPP:quantum/quantum_core.cpp
#if !defined quantum_core_h
#define quantum_core_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"

class quantum_core: public Simulator {
double sigma;
int nqubits;
int shots;
double shot_period;
public:
	quantum_core(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};

#endif
