#include "id.h"
#include "quantum_common.h"

void id::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
sigma = INFINITY;
wire.context = NULL;
wire.qubit = -1;
}
double id::ta(double t) { return sigma; }
void id::dint(double t) { sigma = INFINITY; }
void id::dext(Event e, double t) {
QuantumWire* in = (QuantumWire*) e.value;
if (in) wire = *in;
sigma = processing_time;
}
Event id::lambda(double t) { return Event(&wire, 0); }
void id::exit() {}
