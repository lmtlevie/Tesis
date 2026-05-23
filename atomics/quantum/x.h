//CPP:quantum/x.cpp
#if !defined x_h
#define x_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "time.h"
#include "quantum_common.h"
#include <complex>


class x: public Simulator { 
// Fase del modelo
int phase; 
// 0 = passive
// 1 = busy

// Tiempo hasta pr�ximo evento interno
double sigma;
double processing_time;

// Vector de estado actual |?? = [alpha, beta]
std::complex<double> state_in[2];

// Vector de salida X|??
std::complex<double> state_out[2];
public:
	x(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
