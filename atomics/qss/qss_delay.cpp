#include "qss_delay.h"
void qss_delay::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	char *fvar=va_arg(parameters,char*);
	Delay=readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: delay: %f ms \n", this->getFullName().data(), this->Delay * 1000);

	for(int i=0;i<10;i++){y[i]=0;};
	PNi=1;
	PNo=0;
	sigma=INF;
}
//double qss_delay::ta(double t) {
//	return sigma;
//}
void qss_delay::dint(double t) {
	PNo=(PNo+1)%maxQ;
	if ( ((PNo+1)%maxQ)==PNi ) {
		sigma=INF;
	}else{
		sigma=Bt[PNo]+Delay-t;
		if(sigma<0){sigma=0;};
	};
}
void qss_delay::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	double* xv;
	xv= (double*) x.value;

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: arrived new signal=[%g, %g, %g, %g]\n",t, this->getFullName().data(), xv[0], xv[1], xv[2], xv[3]);
	if(PNi!=PNo){
		Bt[PNi-1]=t;
		Bu[PNi-1][0]=xv[0];
		Bu[PNi-1][1]=xv[1];
		Bu[PNi-1][2]=xv[2];
		Bu[PNi-1][3]=xv[3];

		if(PNi==((PNo+1)%maxQ)){
			sigma = Delay;
		} else {
			sigma = std::max(0.0, sigma-e); // continue as before
		}

		// check if PNi exceeds maxQ and wrap-around
		if((PNi+1) != (PNi+1)%(maxQ+1)){ // uses (maxQ+1) because the array index is PNi-1 (instead of PNi)
			debugMsg(LOG_LEVEL_DEBUG, "[%g] %s - qss_delay::dext - History queue exceeded, from now on it will wrap-around and replace old values. PNi=%l ; PNo=%l,  maxQ=%l ; (PNi+1)%MaxQ=%l \n", t, this->getFullName().data(), PNi, PNo, maxQ, (PNi+1)%maxQ);
			PNi = 1; // restart from 1 array index is PNi-1 (instead of PNi)
		} else {
			PNi++;
		}
	} else { // PNi ==PNo
		debugMsg(LOG_LEVEL_EXCEPTION, "[%g] %s - qss_delay::dext - WARNING! History queue exceed, wrapped-around and should delete unused values (need to increase maxQ, or delay configured too high). PNi=%l ; PNo=%l,  maxQ=%l ; (PNi+1)%MaxQ=%l \n", t, this->getFullName().data(), PNi, PNo, maxQ, (PNi+1)%maxQ);
	}
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: programmed output after %f (at t=%f)\n",t, this->getFullName().data(), sigma, sigma+t);
}
Event qss_delay::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
	y[0]=Bu[PNo][0];
	y[1]=Bu[PNo][1];
	y[2]=Bu[PNo][2];
	y[3]=Bu[PNo][3];

	debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[dext]: sending signal=[%g, %g, %g, %g]\n",t, this->getFullName().data(), y[0], y[1], y[2], y[3]);
	return Event(y,0);
}
//void qss_delay::exit() {
//
//}
