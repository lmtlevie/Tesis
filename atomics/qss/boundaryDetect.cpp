#include "boundaryDetect.h"
void boundaryDetect::init(double t,...) {
va_list parameters;
va_start(parameters,t);

char *fvar= va_arg(parameters,char*);
level= readDefaultParameterValue<double>(fvar );
fvar= va_arg(parameters,char*);
outval= readDefaultParameterValue<double>(fvar );


highDetection = false;
char* method = va_arg(parameters,char*);
if(strcmp(method,"HighDetection")==0) highDetection = true;

for(int i=0;i<10;i++){u[i]=0;};
for(int i=0;i<10;i++){y[i]=0;};

sigma=INF;
}
double boundaryDetect::ta(double t) {
return sigma;
}
void boundaryDetect::dint(double t) {
//advance_time(u,sigma,4);

//u[0]=1e-20*1;//to correct numerical errors
//sigma=minposroot(u,4);
sigma = std::numeric_limits<double>::infinity();

}
void boundaryDetect::dext(Event x, double t) {
double *xv;
xv=(double*)(x.value);

u[0]=xv[0]-level;
u[1]=xv[1];
u[2]=xv[2];
u[3]=xv[3];
u[4]=xv[4];

if(sigma > 0){
	if ( ((!highDetection && u[0]<=0) || (highDetection && u[0]>=0)) && lastT != t) {
		sigma=0;
	} else {
	  sigma=minposroot(u,4);
	}
}

}
Event boundaryDetect::lambda(double t) {
y[0]=outval;

lastT = t;
return Event(y,0);
}
void boundaryDetect::exit() {

}
