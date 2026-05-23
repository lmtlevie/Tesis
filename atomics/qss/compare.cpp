#include "compare.h"
void compare::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	low= readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_IMPORTANT, "[INIT] %s: low=%g \n", this->getFullName().data(), low);

	fvar= va_arg(parameters,char*);
	up= readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_IMPORTANT, "[INIT] %s: up=%g \n", this->getFullName().data(), up);

	for(int i=0;i<10;i++){
		u[0][i]=0;
		u[1][i]=0;
		y[i]=0;
		du[i]=0;
	};
	sw = UNDEFINED;

	sigma=0;
}
//double compare::ta(double t) {
//return sigma;
//}
void compare::dint(double t) {
//	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::dint START - sigma=%g ; sw=%d ; u0=[%g][%g][%g] ; u1=[%g][%g][%g] \n", t, this->getFullName().data(), sigma, sw, u[0][0], u[0][1], u[0][2], u[1][0], u[1][2], u[1][2]);
	advance_time(u[0],sigma,-1);
	advance_time(u[1],sigma,-1);
	for (int i=0;i<4;i++) {
		du[i]=u[0][i]-u[1][i];
	}

	if (sigma>0||du[0]==0){
		du[0]=0;
		advance_time(du,1e-20,-1);
	}

	if(du[0] > -1e-6 && du[0] < 1e-6){ // difference is too small and might cause many iterations
		if (sw==LOW) du[0] = 1e-6;
		if (sw==UP) du[0] = -1e-6;
	}

	sigma=minposroot(du,4);
	sw = sw==UP? LOW : UP; // change state
}
void compare::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);

	//printLog("t=%g : We entered delta_ext \n",t);

	u[x.port][0]=xv[0];
	u[x.port][1]=xv[1];
	u[x.port][2]=xv[2];
	u[x.port][3]=xv[3];

	advance_time(u[1-x.port],e,-1);

	for (int i=0;i<4;i++) {
		du[i]=u[0][i]-u[1][i];
	};

	if (du[0]==0){advance_time(du,1e-20,-1);} //We cross the discontinuity

	if (((du[0]>0)&&(sw==UP))||((du[0]<=0)&&(sw==LOW))){ //we migth be in the wrong position
		sigma=0;
	} else {
		if ((sigma>0)||(t>0))sigma=minposroot(du,4);
	};
}
Event compare::lambda(double t) {
	// at the very beginning we check were we are
	if(sw == UNDEFINED){
		sw = (du[0]>0)? UP : LOW;
	}

	if (sw==LOW) {
		y[0]=low;
	} else {
		y[0]=up;
	};

	return Event(y,0);
}
//void compare::exit() {
//
//}
