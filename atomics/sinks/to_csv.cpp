#include "to_csv.h"
void to_csv::init(double t,...) {
	va_list parameters;
	va_start(parameters, t);

	this->fileName = this->getFullName() + ".csv";

	sigma = std::numeric_limits<double>::infinity();
	file = -1;
}
double to_csv::ta(double t) {
	return sigma;
}
void to_csv::dint(double t) {
	sigma = std::numeric_limits<double>::infinity();
}
void to_csv::dext(Event x, double t) {
	char buf[1024];
	double *aux;
	//std::cout << this->getFullName() << " received dext" << std::endl;
	if (x.port >= 0) {
		if(file == -1){
			file = PDFileOpen(fileName.data(), 'w');
		}

		aux = (double*)(x.value);
		sprintf(buf, "%g, %g\n", t, *aux);
		PDFileWrite(file, buf, strlen(buf));
	}
}
Event to_csv::lambda(double t) {
	return Event(0,0);
}
void to_csv::exit() {
	PDFileClose(file);
}
