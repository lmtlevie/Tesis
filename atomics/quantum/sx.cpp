#include "sx.h"
#include "quantum_common.h"

void sx::init(double t,...) {
va_list parameters;
va_start(parameters,t);
char *fvar = va_arg(parameters,char*);
processing_time = qtheta(fvar);
phase = 0;
sigma = INFINITY;
state_in[0] = state_in[1] = std::complex<double>(0.0, 0.0);
state_out[0] = state_out[1] = std::complex<double>(0.0, 0.0);
}
double sx::ta(double t) { return sigma; }
void sx::dint(double t) { phase = 0; sigma = INFINITY; }
void sx::dext(Event x, double t) {
std::complex<double>* in = (std::complex<double>*) x.value;
state_in[0] = in[0];
state_in[1] = in[1];
phase = 1;
sigma = processing_time;
}
Event sx::lambda(double t) {
const std::complex<double> h(0.5, 0.0);
const std::complex<double> i(0.0, 1.0);
apply2x2(state_out, state_in, h + i*h, h - i*h, h - i*h, h + i*h);
return Event(&state_out, 0);
}
void sx::exit() {}
