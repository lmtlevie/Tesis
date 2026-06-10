#include "z.h"
#include "quantum_common.h"

void z::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
sigma = INFINITY;
wire.context = NULL;
wire.qubit = -1;
}
double z::ta(double t) { return sigma; }
void z::dint(double t) { sigma = INFINITY; }
void z::dext(Event e, double t) {
QuantumWire* in = (QuantumWire*) e.value;
if (!in || !in->context) {
	sigma = processing_time;
	return;
}
wire = *in;
std::complex<double> U[4] = {
	std::complex<double>(1,0), std::complex<double>(0,0),
	std::complex<double>(0,0), std::complex<double>(-1,0)};
wire.context->apply_one(wire.qubit, U);
sigma = processing_time;
}
Event z::lambda(double t) { return Event(&wire, 0); }
void z::exit() {}
