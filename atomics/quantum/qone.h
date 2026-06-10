//CPP:quantum/qone.cpp
#if !defined qone_h
#define qone_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_context.h"

class qone: public Simulator {
double sigma;
int qubit;
QuantumWire wire;
public:
	qone(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
