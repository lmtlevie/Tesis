#include "qss_nlfunction.h"
void qss_nlfunction::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//      %Type% is the parameter type
	expre = va_arg(parameters,char*);

	char *fvar= va_arg(parameters,char*);
	n = (int) readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s - qss_nlfunction::init:  n=%d \n", t, this->getFullName().data(), n);

	dt=1e-8;
	sigma=INF;

	for (int i=0;i<10;i++) {
		for (int j=0;j<n;j++) {
			u[j][i]=0;
			uaux[j][i]=0;
		};
		y[i]=0;
	};
	order=1;

	PRVar uvar0=new RVar ( "u0" , &uaux[0][0] );
	PRVar uvar1=new RVar ( "u1" , &uaux[1][0] );
	PRVar uvar2=new RVar ( "u2" , &uaux[2][0] );
	PRVar uvar3=new RVar ( "u3" , &uaux[3][0] );
	PRVar uvar4=new RVar ( "u4" , &uaux[4][0] );
	PRVar uvar5=new RVar ( "u5" , &uaux[5][0] );
	PRVar uvar6=new RVar ( "u6" , &uaux[6][0] );
	PRVar uvar7=new RVar ( "u7" , &uaux[7][0] );
	PRVar uvar8=new RVar ( "u8" , &uaux[8][0] );
	PRVar uvar9=new RVar ( "u9" , &uaux[9][0] );
	PRVar vartab[10];
	vartab[0]=uvar0;
	vartab[1]=uvar1;
	vartab[2]=uvar2;
	vartab[3]=uvar3;
	vartab[4]=uvar4;
	vartab[5]=uvar5;
	vartab[6]=uvar6;
	vartab[7]=uvar7;
	vartab[8]=uvar8;
	vartab[9]=uvar9;
	pop=new ROperation ((char*)expre, n, (RVar**)vartab);
}
//double qss_nlfunction::ta(double t) {
//return sigma;
//}
void qss_nlfunction::dint(double t) {
	sigma=INF;
}
void qss_nlfunction::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value
	//     'x.port' is the port number
	double *xv,fdt,f2dt;
	xv=(double*)(x.value);

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s - qss_nlfunction::ext: port %i=[%f][%f][%f] \n", t, this->getFullName().data(), x.port, xv[0], xv[1], xv[2]);

	switch(order) {
	case 1:
		u[x.port][0]=xv[0];
		if (xv[1]!=0){order=2;u[x.port][1]=xv[1];}
		if (xv[2]!=0){order=3;u[x.port][2]=xv[2];}
		if (xv[3]!=0){order=4;u[x.port][3]=xv[3];}

		break;

	case 2:
		u[x.port][0]=xv[0];
		u[x.port][1]=xv[1];
		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(u[i],e,1);
			};
		};
		if (xv[2]!=0){order=3;u[x.port][2]=xv[2];}
		if (xv[3]!=0){order=4;u[x.port][3]=xv[3];}

		break;

	case 3:
		u[x.port][0]=xv[0];
		u[x.port][1]=xv[1];
		u[x.port][2]=xv[2];
		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(u[i],e,2);
			};
		};
		if (xv[3]!=0){order=4;u[x.port][3]=xv[3];}

		break;

	case 4:
		u[x.port][0]=xv[0];
		u[x.port][1]=xv[1];
		u[x.port][2]=xv[2];
		u[x.port][3]=xv[3];
		for (int i=0;i<n;i++) {
			if (i!=x.port) {
				advance_time(u[i],e,3);
			};

		};
	}

	//printLog("[%g] qss_nlfunction::dext - u[x.port]=[%f][%f][%f] \n", t, u[x.port][0], u[x.port][1], u[x.port][2]);
	//printLog("[%g] qss_nlfunction::dext - order=%i \n", t, order);

	sigma=0;
	if (e>1e-15){dt=e/100;}
	//printLog("[%g] qss_nlfunction::dext - dt=%f   ; e=%f\n", t, dt, e);

	switch(order) {
	case 1:
		//printLog("[%g] qss_nlfunction::ext case1 ANTES- pop->Val()=%f  ; uaux[x.port][0]=%f \n", t, pop->Val(), uaux[x.port][0]);
		uaux[x.port][0]=u[x.port][0];
		f[0]=pop->Val();
		//printLog("[%g] qss_nlfunction::ext case1 DESPUES- pop->Val()=%f  ; uaux[x.port][0]=%f ; f[0]=%f \n", t, pop->Val(), uaux[x.port][0], f[0]);
		break;

	case 2:
		for (int i=0;i<n;i++){
			uaux[i][0]=u[i][0];
			uaux[i][1]=u[i][1];
		};
		f[0]=pop->Val();
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,1);
		};
		f[1]=(pop->Val() - f[0])/dt;

		break;

	case 3:
		for (int i=0;i<n;i++){
			uaux[i][0]=u[i][0];
			uaux[i][1]=u[i][1];
			uaux[i][2]=u[i][2];
		};
		f[0]=pop->Val();
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,2);
		};
		fdt=pop->Val();
		f[1]=(fdt - f[0])/dt;
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,2);
		};
		f[2]=(pop->Val()-2*fdt+f[0])/dt/dt/2;
		break;

	case 4:
		for (int i=0;i<n;i++){
			uaux[i][0]=u[i][0];
			uaux[i][1]=u[i][1];
			uaux[i][2]=u[i][2];
			uaux[i][3]=u[i][3];
		};
		f[0]=pop->Val();
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,3);
		};
		fdt=pop->Val();
		f[1]=(fdt - f[0])/dt;
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,3);
		};
		f2dt=pop->Val();
		f[2]=(f2dt-2*fdt+f[0])/dt/dt/2;
		for (int i=0;i<n;i++) {
			advance_time(uaux[i],dt,3);
		};
		f[3]=(pop->Val()-3*f2dt+3*fdt-f[0])/dt/dt/dt/6;
	};

	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s - qss_nlfunction::ext:  f=[%f][%f][%f] \n", t, this->getFullName().data(), f[0], f[1], f[2]);
	sigma=0;
}
Event qss_nlfunction::lambda(double t) {
	for (int i=0;i<order;i++) {
		y[i]=f[i];
	};
	debugMsg(LOG_LEVEL_PRIORITY, "[%g] %s - qss_nlfunction::lambda:  output: [%f],[%f],[%f], \n", t, this->getFullName().data(), y[0], y[1], y[2]);
	return Event(y,0);
}
//void qss_nlfunction::exit() {
//
//}
