#include "qone.h"
#include "quantum_common.h"

void qone::init(double t,...) {
	va_list p; va_start(p,t);
	qubit = qparse_i(va_arg(p,char*));
	wire.context = quantum_context_for(this, qubit >= 0 ? qubit + 1 : 1);
	if (qubit < 0) qubit = wire.context->allocate_qubit();
	wire.context->set_initial_qubit(qubit, 1);
	wire.qubit = qubit;
	sigma = 0;
}
double qone::ta(double t) { return sigma; }
void qone::dint(double t) { sigma = INFINITY; }
void qone::dext(Event e, double t) {}
Event qone::lambda(double t) {
	wire.context = quantum_context_for(this, qubit >= 0 ? qubit + 1 : 1);
	wire.qubit = qubit;
	return Event(&wire, 0);
}
void qone::exit() {}
