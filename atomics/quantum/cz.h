//CPP:quantum/cz.cpp
#if !defined cz_h
#define cz_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include <complex>

class cz: public Simulator {
int phase;
double sigma;
double processing_time;
int emit_port;
bool has0;
bool has1;
std::complex<double> q0[2];
std::complex<double> q1[2];
std::complex<double> out0[2];
std::complex<double> out1[2];
public:
	cz(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
