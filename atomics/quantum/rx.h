//CPP:quantum/rx.cpp
#if !defined rx_h
#define rx_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include <complex>

class rx: public Simulator {
int phase;
double sigma;
double processing_time;
double theta;
std::complex<double> state_in[2];
std::complex<double> state_out[2];
public:
	rx(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
