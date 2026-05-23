#include "packetmovavg.h"
void packetmovavg::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

char* fvar;
fvar = va_arg(parameters,char*);
alfa = (double)readDefaultParameterValue<double>(fvar);
printLog("[%g] MovAvg: alfa: %g \n",t,alfa);

for (int i=0;i<10;i++) {
	out_mavg[i]=0;
	};

sample = 0;
mavg = 0;
sigma = INF;  			//

return;
//
}
double packetmovavg::ta(double t) {
//This function returns a double.
return sigma;
}
void packetmovavg::dint(double t) {
sigma=INF;
return;
}
void packetmovavg::dext(Event x, double t) {
// For ANY current State do:
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

			double *xv;
			xv=(double*)(x.value);			
			
			sample = xv[0]; 
			mavg = (1-alfa)*mavg + alfa*sample;

			printLog("[%g] MovAvg: sample: %g mavg: %g \n",t,sample,mavg);
			sigma = 0;

return ;
}
Event packetmovavg::lambda(double t) {
// For ANY current State do:
		out_mavg[0]=mavg ;

		return Event(&out_mavg,0) ;
}
void packetmovavg::exit() {
//Code executed at the end of the simulation.
}
