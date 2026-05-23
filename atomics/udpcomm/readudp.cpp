#include "readudp.h"

void readudp::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	port = atoi(va_arg(parameters,char*));
	RequestNET(port,this);

	for(int i=0;i<10;i++){
		u[i]=0;
		y[i]=0;
	}

	sigma = 1e20;
}

double readudp::ta(double t) {
	//This function returns a double.
	return sigma;
}

void readudp::dint(double t) {
	sigma = 1e20;
}

void readudp::dext(Event x, double t) {

	char * aux;
	double xv;
	aux = (char *) x.value;
	xv = atof(aux);
	// char* aux = strchr((char*)x.value,'=');
	// if(aux == NULL){
	// printLog("Mensaje recibido con formato erroneo: %s\n",x.value);
	//	return;
	//}
	printLog("[AtomicReadUDP] simutime = %f, realtime = %f Mensaje recibido: %d\n",t,getRealSimulationTime(),xv);
	//strcpy(y,(char*)x.value);
	u[0] = xv;
	//printLog("[AtomicReadUDP] Mensaje recibido: %s\n",x.value);
	sigma = 0;
}

Event readudp::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
	//sigma = 1e20;
	//Event out =  Event(&y,0);
	//out.setRealTimeMode(REALTIME);
	//return out;
	
	//if (flag == 0){
	//	y[0] = u[0];
	//	flag = 1;
	//	sigma = 0; // genero una transición interna para sacar la otra salida
	//	return Event(&y[0],0);
	//} else {
	//	y[0] = t;
	//	flag = 0;
	//	sigma = 1e20;
	//	return Event(&y[0],1);
	//}
	y[0] = u[0];
	return Event(&y[0],0);
}

void readudp::exit() {
	//Code executed at the end of the simulation.
}
