#include "id.h"
#include "quantum_common.h"

void id::init(double t,...) {
va_list parameters;
va_start(parameters,t);
char *fvar = va_arg(parameters,char*);
processing_time = qtheta(fvar);
phase = 0;
sigma = INFINITY;
state_in[0] = state_in[1] = std::complex<double>(0.0, 0.0);
state_out[0] = state_out[1] = std::complex<double>(0.0, 0.0);
}
double id::ta(double t) { return sigma; }
void id::dint(double t) { phase = 0; sigma = INFINITY; }
void id::dext(Event x, double t) {
std::complex<double>* in = (std::complex<double>*) x.value;
state_in[0] = in[0];
state_in[1] = in[1];
phase = 1;
sigma = processing_time;
}
Event id::lambda(double t) {
apply2x2(state_out, state_in,
	std::complex<double>(1.0, 0.0), std::complex<double>(0.0, 0.0),
	std::complex<double>(0.0, 0.0), std::complex<double>(1.0, 0.0));
return Event(&state_out, 0);
}
void id::exit() {}
