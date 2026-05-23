#include "cross_detect_withDynamicOutput.h"

void cross_detect_withDynamicOutput::init(double t,...) {
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	level= readDefaultParameterValue<double>(fvar );


	for(int i=0;i<10;i++){u[i]=0;};
	for(int i=0;i<10;i++){y[i]=0;};

	sigma=INF;
	sw=0;
}

double cross_detect_withDynamicOutput::ta(double t) {
	return sigma;
}

void cross_detect_withDynamicOutput::dint(double t) {
	advance_time(u,sigma,4);
	sw=-sw;
	u[0]=1e-20*sw;//to correct numerical errors
	sigma=minposroot(u,4);

}

void cross_detect_withDynamicOutput::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);

	if(x.port == 0){
		u[0]=xv[0]-level;
		u[1]=xv[1];
		u[2]=xv[2];
		u[3]=xv[3];
		u[4]=xv[4];

		if (((sw==1)&&(u[0]<0))||((sw==-1)&&(u[0]>0))) {
			sigma=0;
		} else {
			sigma=minposroot(u,4);
			if (sw==0) {
				if (u[0]>0) {
					sw=1;
				} else {
					sw=-1;
				}
			}
		}
	}

	if(x.port == 1){ // new output value
		// copy input to the outputValue
		for (int i = 0; i < 10 ; i++){
			y[i]=xv[i];
		}

		sigma = sigma - e; //continue as before
	}
}

Event cross_detect_withDynamicOutput::lambda(double t) {
	return Event(y,0);
}

void cross_detect_withDynamicOutput::exit() {

}
