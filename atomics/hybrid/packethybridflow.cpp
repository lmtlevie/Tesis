#include "packethybridflow.h"
void packethybridflow::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

fluidpackets=0;

myLoggerToScilab = new aLoggerToScilab(1, this->myself,this->getName());
myLoggerToScilab->initSignals(std::vector<std::string>{"fluidpackets","fluidrate","fluidLength"});

for (int i=0;i<10;i++) {
fluidrate[i]=0;}

sigma=INF;  // Goes waiting

return;
}
double packethybridflow::ta(double t) {
//This function returns a double.
return sigma;
}
void packethybridflow::dint(double t) {
sigma = INF ;
}
void packethybridflow::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition

	fluidpackets = fluidpackets + e * fluidrate[0]+e*e*fluidrate[1]/2+e*e*e*fluidrate[2]/3;


	

if (x.port==0) {   // A new packet enters
  	advance_time(fluidrate,e,-1);
	p = (Packet*)x.value;
	if (fluidpackets<0) fluidpackets=0;
	p->setFluidPackets(fluidpackets);
	
	double fl = p->getLength() * p->getFluidPackets();
   myLoggerToScilab->logSignal(t,fluidpackets,"fluidpackets");
	myLoggerToScilab->logSignal(t,fl,"fluidLength");
	fluidpackets=0;
	
	sigma=0;     
    printLog("[%g] HybridFlow: received Packet ID %u \n",t, p->ID);
    printLog("[%g] HybridFlow: with birthtime %g \n",t, p->birthTime);
};

if (x.port==1) {    
  double *Aux = (double*)x.value;
  for (int i=0;i<10;i++) {
		fluidrate[i]=Aux[i];}
  sigma=INF;
};

// State based
myLoggerToScilab->logSignal(t,fluidrate[0],"fluidrate");
return;
}
Event packethybridflow::lambda(double t) {
//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

return Event(p,0);
}
void packethybridflow::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flush(999999);
}
