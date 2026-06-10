#include "y.h"
#include "quantum_common.h"

void y::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
sigma = INFINITY;
wire.context = NULL;
wire.qubit = -1;
}
double y::ta(double t) { return sigma; }
void y::dint(double t) { sigma = INFINITY; }
void y::dext(Event e, double t) {
QuantumWire* in = (QuantumWire*) e.value;
if (!in || !in->context) {
	sigma = processing_time;
	return;
}
wire = *in;
std::complex<double> U[4] = {
	std::complex<double>(0,0), std::complex<double>(0,-1),
	std::complex<double>(0,1), std::complex<double>(0,0)};
wire.context->apply_one(wire.qubit, U);
sigma = processing_time;
}
Event y::lambda(double t) { return Event(&wire, 0); }
void y::exit() {}
