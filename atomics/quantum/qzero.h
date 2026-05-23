//CPP:quantum/qzero.cpp
#if !defined qzero_h
#define qzero_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include <complex>

class qzero: public Simulator {
int phase;
double sigma;
std::complex<double> qbit[2];
public:
	qzero(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
