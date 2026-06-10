//CPP:quantum/h.cpp
#if !defined h_h
#define h_h
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_context.h"
class h: public Simulator {
double sigma;
double processing_time;
QuantumWire wire;
public:
	h(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
