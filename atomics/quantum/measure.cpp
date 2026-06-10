#include "measure.h"
#include "quantum_common.h"
#include <cmath>

void measure::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
char *fvar = va_arg(p,char*);
unsigned long seed = (fvar && *fvar) ? std::strtoul(fvar, NULL, 10) : 0UL;
if (seed == 0UL) {
	std::random_device rd;
	rng.seed(rd());
} else {
	rng.seed((unsigned)seed);
}
output[0] = -1.0;
sigma = INFINITY;
}
double measure::ta(double t) { return sigma; }
void measure::dint(double t) { sigma = INFINITY; }
void measure::dext(Event e, double t) {
QuantumWire* wire = (QuantumWire*) e.value;
if (!wire || !wire->context) {
	output[0] = -1.0;
	sigma = processing_time;
	return;
}
int outcome = wire->context->measure(wire->qubit, rng);
output[0] = (double)outcome;
sigma = processing_time;
}
Event measure::lambda(double t) { return Event(output, 0); }
void measure::exit() {}
