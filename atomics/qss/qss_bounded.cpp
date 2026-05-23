#include "qss_bounded.h"
void qss_bounded::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type
for (int i=0;i<10;i++) {
	y[i]=0;
 	X[i]=0;
	q[i]=0;
};

char *fvar= va_arg(parameters,char*);
dQmin=readDefaultParameterValue<double>(fvar );

fvar= va_arg(parameters,char*);
dQrel=readDefaultParameterValue<double>(fvar );

fvar= va_arg(parameters,char*);
X[0]=readDefaultParameterValue<double>(fvar);

// TODO: getScilabVar
lowerBound = 0;

boundaryState = NO_BOUND;

dQ=fabs(X[0])*dQrel;
if (dQ<dQmin){dQ=dQmin;};

q[0]=X[0]; 
sigma=0;
}
double qss_bounded::ta(double t) {
//This function return a double.
return sigma;
}
void qss_bounded::dint(double t) {
advance_time(X,sigma,1);

if(X[0] <= lowerBound){
	boundaryState = LOWER_BOUND;
	X[1] = 0; // der=0 because we would cross the low_boundary
}

q[0]=X[0];
dQ=dQrel*fabs(X[0]);
if (dQ<dQmin)dQ=dQmin;
if (X[1]==0){
	sigma=INF;
} else {
	sigma=fabs(dQ/X[1]);
}

}
void qss_bounded::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value
//     'x.port' is the port number
double *derx;
double diffxq[10];
double boundary[10] = {0};
double dt1;

derx=(double*)x.value;

if (x.port==0) {
	X[0]=X[0]+X[1]*e;

	X[1]=derx[0];
	if(boundaryState == LOWER_BOUND && X[1] < 0){
		X[1] = 0;
	} else{
		boundaryState = NO_BOUND;
	}

	if (sigma>0){
		// calculate when X and q will differ in more than dQ
	   	diffxq[1]=-X[1];
   		diffxq[0]=q[0]-X[0]-dQ;
   		sigma=minposroot(diffxq,1);
   		diffxq[0]=q[0]-X[0]+dQ;
   		dt1=minposroot(diffxq,1);
   		if (dt1<sigma) sigma=dt1;
   		if (fabs(X[0]-q[0])>dQ) sigma=0;

   		// calculate when we will cross the lowBoundary
   		boundary[0]=X[0] - lowerBound;
   		boundary[1]=X[1];
   		boundary[2]=X[2];
   		boundary[3]=X[3];

   		if (boundary[0] == 0) advance_time(boundary,1e-20,-1); //we are in the boundary, advance to see where we go
   		if (boundary[0] < 0){
   			sigma = 0; // we already crossed the boundary, transition now
   			boundaryState = LOWER_BOUND;
   		} else{
   			double boundaryCross = minposroot(boundary,4); // when do we cross the boundary?
			if (boundaryCross < sigma){
				sigma = boundaryCross;
			}
   		}


	}
} else {
	X[0]=derx[0];
	sigma=0;
}
}
Event qss_bounded::lambda(double t) {
//This function return an event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% is a direction to the variable that contain the value.
//     %NroPort% is the port number (from 0 to n-1)

y[0]=X[0];
y[1]=X[1];
advance_time(y,sigma,1);
y[1]=0;

//if(y[0] < lowerBound) y[0] = lowerBound; TODO: might be necessary due to numerical errors

return Event(&y,0);
}
void qss_bounded::exit() {

}
