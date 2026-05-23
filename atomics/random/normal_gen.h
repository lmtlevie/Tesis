//CPP:/random/lib/stocc.cpp
//CPP:/random/lib/randomc.cpp
//CPP:random/normal_gen.cpp
#if !defined normal_gen_h
#define normal_gen_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "time.h"
#include "random/lib/randomc.h"
#include "random/lib/stocc.h"
#include "sinks/ParameterReader.h"


class normal_gen: public Simulator { 
int phase;       // 0 = passive, 1 = active
double sigma;

// Qubit de salida |0? = (1, 0)
std::complex<double> qbit[2];
public:
	normal_gen(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
