//CPP:quantum/qcount.cpp
#if !defined qcount_h
#define qcount_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"

class qcount: public Simulator {
double sigma;
double count0[1];
double count1[1];
int emit_port;
public:
	qcount(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
};

#endif
