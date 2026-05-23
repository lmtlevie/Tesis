#include "sendudp.h"

void sendudp::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	port=readDefaultParameterValue<int>(fvar );
	// port = atoi(va_arg(parameters, char*));
	char* pip = (char*) va_arg(parameters, char*);
	strcpy(ip,pip);

	printLog("[sendudp] port=%i, ip=%s\n",port,ip);

	for(int i=0;i<10;i++){
		y[i]=0;
	}

	sigma = 1e20;
}

double sendudp::ta(double t) {
	//This function returns a double.
	return sigma;
}

void sendudp::dint(double t) {
	sigma = 1e20;
}

void sendudp::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	double *xv; 
	char msg[200];
	xv = (double*)(x.value);
	//printLog("Bloque recibio: xv[0] = %f, xv[1] = %f, xv[2] = %f, xv[3] = %f, xv[4] = %f, t = %f \n",xv[0],xv[1],xv[2],xv[3],xv[4],t);
	//sprintf(msg,"xv[0] = %f, xv[1] = %f, xv[2] = %f, xv[3] = %f, xv[4] = %f \n",xv[0],xv[1],xv[2],xv[3],xv[4]);
	//printLog("Bloque recibio: xv[0] = %f, t = %f \n",xv[0],t);
	//sprintf(msg,"%f %f",xv[0],t);
	sprintf(msg,"%f",xv[0]);
	sendNET(port,ip,msg,strlen(msg));
	printLog("[SendUDP] simutime = %f realtime = %f msg %s ip=%s port=%d\n",t,getRealSimulationTime(),msg,ip,port);
	//sigma = 1e20;
	// y[0] = *xv;
	sigma = 0;
}

Event sendudp::lambda(double t) {
	y[0] = t;
	return Event(&y,0); // devuelvo el tiempo del envio
}

void sendudp::exit() {
	//Code executed at the end of the simulation.
}
