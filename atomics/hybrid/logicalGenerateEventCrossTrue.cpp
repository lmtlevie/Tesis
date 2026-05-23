#include "logicalGenerateEventCrossTrue.h"

void logicalGenerateEventCrossTrue::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	level= readDefaultParameterValue<double>(fvar );

//  performed at variable creation (in the .h)
//	for(int i=0;i<10;i++){u[i]=0;};
//	for(int i=0;i<10;i++){y[i]=0;};

	sigma=INF;
	sw=0;
}


void logicalGenerateEventCrossTrue::dint(double t) {
	advance_time(u,sigma,4);
	sw=-sw;
	u[0]=1e-20*sw;//to correct numerical errors
	sigma=minposroot(u,4);
}

void logicalGenerateEventCrossTrue::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);

	if(x.port == 0){
		u[0]=xv[0]-level;
		u[1]=xv[1];
		u[2]=xv[2];
		u[3]=xv[3];
		u[4]=xv[4];

		advance_time(y, e, -1); // advance output


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

Event logicalGenerateEventCrossTrue::lambda(double t) {
	if(u[0] >= this->level) {
		return Event(y,0);
	} else {
		return Event(); // don't send the event
	}

}
