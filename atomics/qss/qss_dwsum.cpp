#include "qss_dwsum.h"
void qss_dwsum::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	this->n=readDefaultParameterValue<int>(fvar );

	if(this->n >= MAX_INPUTS){
		debugMsg(LOG_LEVEL_EXCEPTION, "[INIT] %s::init: ERROR specified %d parameters. Current  implementation accepts a maximum of MAX_INPUTS=%d inputs.\n", this->getFullName().data(), this->n, MAX_INPUTS);
	}

	for (int i=0; i < this->n ; i++){
		fvar= va_arg(parameters,char*);
		K[i]=readDefaultParameterValue<double>(fvar );
	}

	order=1;

	for (int i=0;i<10;i++) {
		for (int j=0;j<MAX_INPUTS;j++) {
			X[j][i]=0;
		};
		y[i]=0;
	};

	sigma=INF;
}

void qss_dwsum::dint(double t) {
	sigma=INF;
}
void qss_dwsum::dext(Event x, double t) {
	double *Xv;
	Xv=(double*)x.value;
	if(x.port >= MAX_INPUTS){
		debugMsg(LOG_LEVEL_EXCEPTION, "[%f] %s::dext: ERROR received a signal in port %d. Current  implementation accepts a maximum of MAX_INPUTS=%d inputs\n", this->getFullName().data(), x.port, MAX_INPUTS);
	}

	switch(order) {
	case 1:
		X[x.port][0]=Xv[0];
		if (Xv[1]!=0){order=2;X[x.port][1]=Xv[1];}
		if (Xv[2]!=0){order=3;X[x.port][2]=Xv[2];}
		if (Xv[3]!=0){order=4;X[x.port][3]=Xv[3];}

		break;
	case 2:
		X[x.port][0]=Xv[0];
		X[x.port][1]=Xv[1];

		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(X[i],e,1);
			};
		};
		if (Xv[2]!=0){order=3;X[x.port][2]=Xv[2];}
		if (Xv[3]!=0){order=4;X[x.port][3]=Xv[3];}
		break;
	case 3:
		X[x.port][0]=Xv[0];
		X[x.port][1]=Xv[1];
		X[x.port][2]=Xv[2];
		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(X[i],e,2);
			};
		};
		if (Xv[3]!=0){order=4;X[x.port][3]=Xv[3];}
		break;
	case 4:
		X[x.port][0]=Xv[0];
		X[x.port][1]=Xv[1];
		X[x.port][2]=Xv[2];
		X[x.port][3]=Xv[3];
		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(X[i],e,3);
			};
		};
	}
	sigma=0;
}
Event qss_dwsum::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

	for (int j=0;j<order;j++) {
		y[j]=0;
		for (int i=0;i<n;i++) {
			y[j]=y[j]+K[i]*X[i][j];
		};
	};
	return Event(y,0);
}

