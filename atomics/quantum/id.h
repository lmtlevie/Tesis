//CPP:quantum/id.cpp
#if !defined id_h
#define id_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include <complex>

class id: public Simulator {
int phase;
double sigma;
double processing_time;
std::complex<double> state_in[2];
std::complex<double> state_out[2];
public:
	id(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
