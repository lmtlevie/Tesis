#include "qzero.h"
#include "quantum_common.h"

void qzero::init(double t,...) {
	va_list p; va_start(p,t);
	qubit = qparse_i(va_arg(p,char*));
	wire.context = quantum_context_for(this, qubit >= 0 ? qubit + 1 : 1);
	if (qubit < 0) qubit = wire.context->allocate_qubit();
	wire.context->set_initial_qubit(qubit, 0);
	wire.qubit = qubit;
	shot = 0;
	sigma = 0;
}
double qzero::ta(double t) { return sigma; }
void qzero::dint(double t) {
	shot++;
	if (wire.context && shot < wire.context->shots()) sigma = wire.context->shot_period();
	else sigma = INFINITY;
}
void qzero::dext(Event e, double t) {}
Event qzero::lambda(double t) {
	wire.context = quantum_context_for(this, qubit >= 0 ? qubit + 1 : 1);
	wire.context->begin_shot(shot);
	wire.qubit = qubit;
	return Event(&wire, 0);
}
void qzero::exit() {}
