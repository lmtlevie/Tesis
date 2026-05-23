#include "ifEqualGenerateEvent.h"

void ifEqualGenerateEvent::init(double t,...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	this->level= readDefaultParameterValue<double>(fvar );

//  performed at variable creation (in the .h)
//	for(int i=0;i<10;i++){u[i]=0;};
//	for(int i=0;i<10;i++){y[i]=0;};

	this->sigma = std::numeric_limits<double>::infinity();
}


void ifEqualGenerateEvent::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
}

void ifEqualGenerateEvent::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);

	if(x.port == 0){
		u[0]=xv[0]-level;
		u[1]=xv[1];
		u[2]=xv[2];
		u[3]=xv[3];
		u[4]=xv[4];

		advance_time(y, e, -1); // advance output

		if(u[0]==0){ // if already equal, generate output immediately
			this->sigma = 0;
		} else {
			double lower_crossing = minposroot(u,4);
			u[0] = -u[0]; // change value to detect crossing from higher values
			double upper_crossing = minposroot(u,4);

			// generate output whenever it will be equal
			this->sigma = std::min(upper_crossing, lower_crossing);
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

Event ifEqualGenerateEvent::lambda(double t) {
	return Event(y,0);
}
