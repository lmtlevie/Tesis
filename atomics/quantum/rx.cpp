#include "rx.h"
#include "quantum_common.h"

void rx::init(double t,...) {
va_list parameters;
va_start(parameters,t);
char *fvar = va_arg(parameters,char*);
processing_time = qtheta(fvar);
fvar = va_arg(parameters,char*);
theta = qtheta(fvar);
phase = 0;
sigma = INFINITY;
state_in[0] = state_in[1] = std::complex<double>(0.0, 0.0);
state_out[0] = state_out[1] = std::complex<double>(0.0, 0.0);
}
double rx::ta(double t) { return sigma; }
void rx::dint(double t) { phase = 0; sigma = INFINITY; }
void rx::dext(Event x, double t) {
std::complex<double>* in = (std::complex<double>*) x.value;
state_in[0] = in[0];
state_in[1] = in[1];
phase = 1;
sigma = processing_time;
}
Event rx::lambda(double t) {
const double h = 0.5 * theta;
const std::complex<double> c(cos(h), 0.0);
const std::complex<double> s(0.0, -sin(h));
apply2x2(state_out, state_in, c, s, s, c);
return Event(&state_out, 0);
}
void rx::exit() {}
