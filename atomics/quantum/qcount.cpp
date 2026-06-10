#include "qcount.h"

void qcount::init(double t,...) {
	count0[0] = 0.0;
	count1[0] = 0.0;
	emit_port = 0;
	sigma = INFINITY;
}

double qcount::ta(double t) {
	return sigma;
}

void qcount::dint(double t) {
	if (emit_port == 0) {
		emit_port = 1;
		sigma = 0;
		return;
	}
	emit_port = 0;
	sigma = INFINITY;
}

void qcount::dext(Event e, double t) {
	double* value = (double*) e.value;
	if (!value) {
		sigma = INFINITY;
		return;
	}
	if (value[0] >= 0.5) count1[0] += 1.0;
	else count0[0] += 1.0;
	emit_port = 0;
	sigma = 0;
}

Event qcount::lambda(double t) {
	if (emit_port == 0) return Event(count0, 0);
	return Event(count1, 1);
}

void qcount::exit() {}
