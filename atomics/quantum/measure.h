//CPP:quantum/measure.cpp
#if !defined measure_h
#define measure_h
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_context.h"
#include <random>
class measure: public Simulator {
double sigma;
double processing_time;
double output[1];
int output_port;
std::mt19937 rng;
public:
	measure(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
