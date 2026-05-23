#include "packethybridflowprop.h"
void packethybridflowprop::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

fluidpackets=0;

myLoggerToScilab = new aLoggerToScilab(1, this->myself,this->getName());
myLoggerToScilab->initSignals(std::vector<std::string>{"fluidpackets","prop","fluidLength","discretelength"});

for (int i=0;i<10;i++) {
prop[i]=0;}

sigma=INF;  // Goes waiting

return;
}
double packethybridflowprop::ta(double t) {
//This function returns a double.
return sigma;
}
void packethybridflowprop::dint(double t) {
sigma = INF ;
}
void packethybridflowprop::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition

	
if (x.port==0) {   // A new packet enters
  	advance_time(prop,e,-1);
	p = (Packet*)x.value;
	
	// double fl = mySTDEVS->exponential(p->getLength() * fluidpackets);
	fluidpackets=prop[0];
    if (fluidpackets<0) fluidpackets=0;
    p->setFluidPackets(fluidpackets);

    myLoggerToScilab->logSignal(t,fluidpackets,"fluidpackets");

    fluidpackets=0;
	sigma=0;
    printLog("[%g] HybridFlow: received Packet ID %u \n",t, p->ID);
    printLog("[%g] HybridFlow: with birthtime %g \n",t, p->birthTime);
};

if (x.port==1) {    
  double *Aux = (double*)x.value;
  for (int i=0;i<10;i++) {
		prop[i]=Aux[i];}
  sigma=INF;
};

myLoggerToScilab->logSignal(t,prop[0],"prop");
return;
}

Event packethybridflowprop::lambda(double t) {
//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

return Event(p,0);
}
void packethybridflowprop::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flush(999999);
}
