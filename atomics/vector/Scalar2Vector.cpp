#include "Scalar2Vector.h"
void Scalar2Vector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar= va_arg(parameters,char*);
	index=readDefaultParameterValue<double>(fvar );
	vec.valuePtr = std::shared_ptr<void>(new double[10], std::default_delete<double[]>()); // this is assuming the event has a double[10] as value. Can not use it for general purposes
	//	vec.valuePtr = std::make_shared<double*>();

	sigma=INF;

}
double Scalar2Vector::ta(double t) {
	//This function returns a double.
	return sigma;
}
void Scalar2Vector::dint(double t) {
	sigma=INF;
}
void Scalar2Vector::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	double* values = (double*) vec.valuePtr.get();
	for (int i=0; i<10; i++) {
		values[i]=x.getDouble(i); // this is assuming the event has a double[10] as value. Can not use it for general purposes
	}

	vec.index=index;
	sigma=0;
}
Event Scalar2Vector::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

	return Event(&vec,0);
}
void Scalar2Vector::exit() {
	//Code executed at the end of the simulation.
}
