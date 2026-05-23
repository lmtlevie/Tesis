#include "to_csv_vectorial.h"
void to_csv_vectorial::init(double t,...) {
	// Do not call base class to avoid creating Configuration logger
	//BaseSimulator::init(t);
	this->debugLevel = readDebugLevel(this);
	this->logger = std::make_shared<CsvLogger>(this, false);

	va_list parameters;
	va_start(parameters, t);

	sigma=std::numeric_limits<double>::infinity();
}

void to_csv_vectorial::dint(double t) {
	sigma=10e10;
}

void to_csv_vectorial::dext(Event x, double t) {
	auto vectEvet=*(VECDEVS::vector*)x.value;
	this->logger->logSignal(t, vectEvet.value[0], "_idx"+std::to_string(vectEvet.index));
}

Event to_csv_vectorial::lambda(double t) {
	return Event(0,0);
}

