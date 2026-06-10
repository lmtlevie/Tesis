#include "cx.h"
#include "quantum_common.h"

void cx::init(double t,...) {
va_list p; va_start(p,t);
processing_time = qparse_d(va_arg(p,char*));
sigma = INFINITY;
emit_port = 0;
has_control = false;
has_target = false;
control_wire.context = NULL;
control_wire.qubit = -1;
target_wire.context = NULL;
target_wire.qubit = -1;
}
double cx::ta(double t) { return sigma; }
void cx::dint(double t) {
	if (emit_port == 0) {
		emit_port = 1;
		sigma = 0;
		return;
	}
	emit_port = 0;
	has_control = false;
	has_target = false;
	sigma = INFINITY;
}
void cx::dext(Event e, double t) {
QuantumWire* in = (QuantumWire*) e.value;
if (!in || !in->context) {
	sigma = INFINITY;
	return;
}
if (e.port == 0) {
	control_wire = *in;
	has_control = true;
} else if (e.port == 1) {
	target_wire = *in;
	has_target = true;
}
if (!has_control || !has_target) {
	sigma = INFINITY;
	return;
}
if (control_wire.context != target_wire.context) {
	sigma = INFINITY;
	return;
}
control_wire.context->apply_cnot(control_wire.qubit, target_wire.qubit);
emit_port = 0;
sigma = processing_time;
}
Event cx::lambda(double t) {
	return Event(emit_port == 0 ? (void*)&control_wire : (void*)&target_wire, emit_port);
}
void cx::exit() {}
