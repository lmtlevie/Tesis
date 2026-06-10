//CPP:quantum/cx.cpp
#if !defined cx_h
#define cx_h
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_context.h"
class cx: public Simulator {
double sigma;
double processing_time;
int emit_port;
bool has_control;
bool has_target;
QuantumWire control_wire;
QuantumWire target_wire;
public:
	cx(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
