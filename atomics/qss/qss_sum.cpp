#include "qss_sum.h"
void qss_sum::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	order=1;
	for (int i=0;i<10;i++) {
		for (int j=0; j< N_INPUTS;j++) {
			X[j][i]=0;
		};
		y[i]=0;
	};

	sigma=INF;
}

void qss_sum::dint(double t) {
	sigma=INF;
}
void qss_sum::dext(Event x, double t) {
	double *Xv;
	Xv=(double*)x.value;
	if(x.port >= N_INPUTS){
		debugMsg(LOG_LEVEL_ALWAYS, "[%f] %s::dext: received a signal in port %d which is bigger than shape[0]=%d. Reshaping \n", t, this->getFullName().data(), x.port, N_INPUTS);
		X.resize(boost::extents[x.port+1][10]);
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

		for (int i=0;i<N_INPUTS;i++) {
			if (i!=x.port) {
				advance_time(GET_INPUT(i),e,1);
			};
		};
		if (Xv[2]!=0){order=3;X[x.port][2]=Xv[2];}
		if (Xv[3]!=0){order=4;X[x.port][3]=Xv[3];}
		break;
	case 3:
		X[x.port][0]=Xv[0];
		X[x.port][1]=Xv[1];
		X[x.port][2]=Xv[2];
		for (int i=0;i<N_INPUTS;i++) {
			if (i!=x.port) {
				advance_time(GET_INPUT(i),e,2);
			};
		};
		if (Xv[3]!=0){order=4;X[x.port][3]=Xv[3];}
		break;
	case 4:
		X[x.port][0]=Xv[0];
		X[x.port][1]=Xv[1];
		X[x.port][2]=Xv[2];
		X[x.port][3]=Xv[3];
		for (int i=0;i<N_INPUTS;i++) {
			if (i!=x.port) {
				advance_time(GET_INPUT(i),e,3);
			};
		};
	}
	sigma=0;
}
Event qss_sum::lambda(double t) {
	for (int j=0;j<order;j++) {
		y[j]=0;
		for (int i=0;i<N_INPUTS;i++) {
			y[j]=y[j]+X[i][j];
		};
	};
	return Event(y,0);
}

