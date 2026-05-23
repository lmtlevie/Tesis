#include "qzero.h"
void qzero::init(double t,...) {
va_list parameters;
va_start(parameters,t);
phase = 1;
sigma = 0;
qbit[0] = std::complex<double>(1.0, 0.0);
qbit[1] = std::complex<double>(0.0, 0.0);
}
double qzero::ta(double t) {
return sigma;
}
void qzero::dint(double t) {
phase = 0;
sigma = INFINITY;
}
void qzero::dext(Event x, double t) {
}
Event qzero::lambda(double t) {
return Event(&qbit, 0);
}
void qzero::exit() {
}
