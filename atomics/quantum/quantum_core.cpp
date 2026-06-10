#include "quantum_core.h"
#include "quantum_common.h"
#include "quantum_context.h"
#include <string>

void quantum_core::init(double t,...) {
	va_list p; va_start(p,t);
	nqubits = qparse_i(va_arg(p,char*));
	if (nqubits < 1) nqubits = 1;
	quantum_context_for(this, nqubits)->configure(nqubits, std::string("0"));
	sigma = INFINITY;
}
double quantum_core::ta(double t) { return sigma; }
void quantum_core::dint(double t) { sigma = INFINITY; }
void quantum_core::dext(Event e, double t) {}
Event quantum_core::lambda(double t) { return Event(); }
void quantum_core::exit() { clear_quantum_context_for(this); }
