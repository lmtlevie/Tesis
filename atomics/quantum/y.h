//CPP:quantum/y.cpp
#if !defined y_h
#define y_h
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_context.h"
class y: public Simulator {
double sigma;
double processing_time;
QuantumWire wire;
public:
	y(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
