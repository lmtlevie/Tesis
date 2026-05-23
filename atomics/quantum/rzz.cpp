#include "rzz.h"
#include "quantum_common.h"

void rzz::init(double t,...) {
va_list parameters;
va_start(parameters,t);
char *fvar = va_arg(parameters,char*);
processing_time = qtheta(fvar);
fvar = va_arg(parameters,char*);
theta = qtheta(fvar);
phase = 0;
sigma = INFINITY;
emit_port = -1;
has0 = has1 = false;
}
double rzz::ta(double t) { return sigma; }
void rzz::dint(double t) {
if (emit_port == 0) {
	emit_port = 1;
	sigma = 0;
} else {
	phase = 0;
	emit_port = -1;
	has0 = has1 = false;
	sigma = INFINITY;
}
}
void rzz::dext(Event x, double t) {
std::complex<double>* in = (std::complex<double>*) x.value;
if (x.port == 0) {
	q0[0] = in[0];
	q0[1] = in[1];
	has0 = true;
} else {
	q1[0] = in[0];
	q1[1] = in[1];
	has1 = true;
}
if (has0 && has1) {
	std::complex<double> psi[4];
	tensor_from_qubits(q0, q1, psi);
	const double h = 0.5 * theta;
	const std::complex<double> p0(cos(-h), sin(-h));
	const std::complex<double> p1(cos(h), sin(h));
	psi[0] *= p0;
	psi[1] *= p1;
	psi[2] *= p1;
	psi[3] *= p0;
	split_qubits(psi, out0, out1);
	phase = 1;
	emit_port = 0;
	sigma = processing_time;
}
}
Event rzz::lambda(double t) {
if (emit_port == 0)
	return Event(&out0, 0);
return Event(&out1, 1);
}
void rzz::exit() {}
