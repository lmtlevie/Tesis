#include "gate.h"
void gate::init(double t,...) {
// The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);

// Par�metro opcional: tiempo de procesamiento
char *fvar = va_arg(parameters,char*);
processing_time = (strlen(fvar)==0 ? 0.0 : readDefaultParameterValue<double>(fvar));

// Fase inicial: esperando entrada
phase = 0; // passive

// No hay eventos internos programados
sigma = INFINITY;

// Estado vac�o (todav�a no lleg� ning�n qubit)
state_in[0]  = std::complex<double>(0.0, 0.0);
state_in[1]  = std::complex<double>(0.0, 0.0);

state_out[0] = std::complex<double>(0.0, 0.0);
state_out[1] = std::complex<double>(0.0, 0.0);
}
double gate::ta(double t) {
//This function returns a double.
return sigma;
}
void gate::dint(double t) {
phase = 0; // passive

// No hay m�s eventos internos programados
sigma = INFINITY;
}
void gate::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition


std::complex<double>* in = (std::complex<double>*) x.value;


state_in[0] = in[0];
state_in[1] = in[1];

phase = 1;

sigma = processing_time;
}
Event gate::lambda(double t) {
state_out[0] = state_in[0];
state_out[1] = state_in[1];
return Event(&state_out, 0);
}
void gate::exit() {
//Code executed at the end of the simulation.
}
