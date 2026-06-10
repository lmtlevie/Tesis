#include "h.h"
#include "quantum_common.h"

void h::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
sigma = INFINITY;
wire.context = NULL;
wire.qubit = -1;
}
double h::ta(double t) { return sigma; }
void h::dint(double t) { sigma = INFINITY; }
void h::dext(Event e, double t) {
QuantumWire* in = (QuantumWire*) e.value;
if (!in || !in->context) {
	sigma = processing_time;
	return;
}
wire = *in;
double s = 1.0 / std::sqrt(2.0);
std::complex<double> U[4] = {
	std::complex<double>(s,0), std::complex<double>(s,0),
	std::complex<double>(s,0), std::complex<double>(-s,0)};
wire.context->apply_one(wire.qubit, U);
sigma = processing_time;
}
Event h::lambda(double t) { return Event(&wire, 0); }
void h::exit() {}
