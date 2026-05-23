#include "x.h"
#include "quantum_common.h"
void x::init(double t,...) {
// The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);

// Par�metro opcional: tiempo de procesamiento
char *fvar = va_arg(parameters,char*);
processing_time = qtheta(fvar);

// --------------------
// Inicializaci�n DEVS
// --------------------

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
double x::ta(double t) {
//This function returns a double.
return sigma;
}
void x::dint(double t) {
phase = 0;
sigma = INFINITY;
}
void x::dext(Event x, double t) {
std::complex<double>* in = (std::complex<double>*) x.value;

// Guardar estado entrante

state_in[0] = in[0];
state_in[1] = in[1];

// Pasar a estado de procesamiento

phase = 1; // busy

// Programar salida inmediata (o con latencia)

sigma = processing_time;
}
Event x::lambda(double t) {
state_out[0] = state_in[1];
state_out[1] = state_in[0];

// Devolver evento con el vector de salida

return Event(&state_out, 0);
}
void x::exit() {
//Code executed at the end of the simulation.
}
