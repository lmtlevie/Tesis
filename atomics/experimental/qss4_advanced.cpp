#include "qss4_advanced.h"

void qss4_advanced::init(double t,...) {

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
	
	dQ=fabs(X[0])*dQrel;
	if (dQ<dQmin){dQ=dQmin;};
	
	q[0]=X[0]; 

	// create qss-plot file
	char buff[1024], buff2[1024];
	sprintf(PlotFile,"plots/%ld.qssplot",(long int)this);//
	long int fd = PDFileOpen(PlotFile,'w');
	if(!fd)
		throw std::runtime_error("Failed to create gnuplot files! Make sure the 'plots' subdirectory already exists.");

	// inicializacion del log para QSS plot
	// 0:x,dQrel,dQmin
	// it=0
	// ft=RootSimulator::FinalTime
	// t:0
	// x:<indice de variable> <a0> <a1> <a2> <a3>
	// q:<indice de variable> <quantum actual> <a0> <a1> <a2>
	// sim:start
	sprintf(buff,"0:x,%f,%f\n",dQrel,dQmin);
	strcat(buff,"it:0\t");
	sprintf(buff2,"ft:%f\n",RootSimulator::FinalTime);
	strcat(buff,buff2);
	sprintf(buff2,"t:%f\n",t);
	strcat(buff,buff2);
	sprintf(buff2,"x:0\t%f\t%f\t%f\t%f\t%f\n",X[0],X[1],X[2],X[3],X[4]);
	strcat(buff,buff2);
	sprintf(buff2,"q:0\t%f\t%f\t%f\t%f\t%f\n",dQ,q[0],q[1],q[2],q[3]);
	strcat(buff,buff2);
	strcat(buff,"sim:start\n");

	// cierro y abro el archivo de log para escribirlo
	PDFileWrite(fd,buff,strlen(buff));
	PDFileClose(fd);

	foutput = PDFileOpen(PlotFile,'a');
	sigma=0;

}
	
double qss4_advanced::ta(double t) {

	//This function return a double.
	return sigma;

}

void qss4_advanced::dint(double t) {
	advance_time(X,sigma,4);
	q[0]=X[0];
	q[1]=X[1];
	q[2]=X[2];
	q[3]=X[3];
	dQ=dQrel*fabs(X[0]);
	if (dQ<dQmin)dQ=dQmin;
	if (X[4]==0) {
		sigma=INF;
	} else {
		sigma=pow(fabs(dQ/X[4]),1.0/4);
	}

	// t:timestamp
	// x:0 <a0> <a1> -> el polinomio q no hace falta mostrarlo, y el <quantum actual> es 0 porque se acaba de actualizar.

	if (t>0){
		// t:t
		// x:0 X[0] X[1] X[2] X[3] X[4]
		char buf [1024];
		sprintf(buf,"t:%f\nx:0\t%f\t%f\t%f\t%f\t%f\n",t,X[0],X[1],X[2],X[3],X[4]);
		PDFileWrite(foutput,buf,strlen(buf));
	}
};

void qss4_advanced::dext(Event x, double t) {

	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value
	//     'x.port' is the port number
	double *derx;
	double diffxq[10];
	double dt1;
	
	derx=(double*)x.value;
	 
	if (x.port==0) { 
		X[0]=evaluate_poly(X,e,4);
		X[1]=derx[0];
		X[2]=derx[1]/2;
		X[3]=derx[2]/3;
		X[4]=derx[3]/4;
		if (sigma>0){
			advance_time(q,e,3);
			diffxq[1]=q[1]-X[1];
			diffxq[2]=q[2]-X[2];
			diffxq[3]=q[3]-X[3];
			diffxq[4]=-X[4];
			diffxq[0]=q[0]-X[0]-dQ;
			sigma=minposroot(diffxq,4);
			diffxq[0]=q[0]-X[0]+dQ;
			dt1=minposroot(diffxq,4);
			if (dt1<sigma) sigma=dt1;
			if ((sigma>1e19)&&(X[4]!=0)) {
				//We suspect numerical error, and try with third order roots
				sigma=minposroot(diffxq,3);
				diffxq[0]=q[0]-X[0]-dQ;
				dt1=minposroot(diffxq,3);
				if (dt1<sigma) sigma=dt1;
			};      
			if (fabs(X[0]-q[0])>dQ) {
				sigma=1e-20;
			};       
		}
	} else {
		advance_time(X,e,4);
		X[0]=derx[0];
		sigma=0;
	}

	if (sigma >0){
		// t:timestamp
		// x:0 <a0> <a1> <a2> <a3>
		// q:0 <QUANTUM ACTUAL> <a0> <a1> <a2>
	
		char buf[1024];
		sprintf(buf,"t:%f\nx:0\t%f\t%f\t%f\t%f\t%f\nq:0\t%f\t%f\t%f\t%f\t%f\n",t,X[0],X[1],X[2],X[3],X[4],dQ,q[0],q[1],q[2],q[3]);
		PDFileWrite(foutput,buf,strlen(buf));
	}
}

Event qss4_advanced::lambda(double t) {

	//This function return an event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% is a direction to the variable that contain the value.
	//     %NroPort% is the port number (from 0 to n-1)
	
	
	y[0]=X[0];
	y[1]=X[1];
	y[2]=X[2];     
	y[3]=X[3];     
	y[4]=X[4];     
	advance_time(y,sigma,4);
	y[4]=0;
	return Event(&y,0);

}

void qss4_advanced::exit() {

	char buf[128];
	strcpy(buf,"sim:end");

	PDFileWrite(foutput,buf,strlen(buf));
	PDFileClose(foutput);

}
