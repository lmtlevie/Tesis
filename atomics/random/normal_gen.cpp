#include "normal_gen.h"
void normal_gen::init(double t,...) {
va_list parameters;
va_start(parameters,t);

// No necesitamos parámetros

// Inicialización DEVS
phase = 1;     // active ? listo para emitir
sigma = 0;     // emitir inmediatamente

// Definir |0?
qbit[0] = std::complex<double>(1.0, 0.0); // alpha
qbit[1] = std::complex<double>(0.0, 0.0); // beta
}
double normal_gen::ta(double t) {
//This function returns a double.
return sigma;
}
void normal_gen::dint(double t) {
phase = 0;
sigma = INFINITY;
}
void normal_gen::dext(Event x, double t) {

}
Event normal_gen::lambda(double t) {
return Event(&qbit, 0);
}
void normal_gen::exit() {
//Code executed at the end of the simulation.
}
