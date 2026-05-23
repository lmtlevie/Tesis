#include "gnuplotxy2.h"
void gnuplotxy2::init(double t,...) {

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	char *format[5], *gformat, buff[1024], ninput[3]="  ", Script[128];

	gformat = (char*) va_arg(parameters, char*);   // general formatting
	format[0] = (char*) va_arg(parameters, char*); // formatting options
	//format[1] = (char*) va_arg(parameters, char*); // additional formatting options

	sprintf(FName,"plots/%ld.csv",(long int)this); // csv file with data points
	sprintf(Script,"plots/%ld.plt",(long int)this);// plt file with gnuplot script

	// open csv file, write "\n" and close file
	long int fd = PDFileOpen(FName,'w');
	PDFileWrite(fd,"\n",1);
	PDFileClose(fd);

	n = 2; // formatting lines on graphical interface
	hasOutput = 0;
	char buff2[1096];
	strcpy(buff2, gformat);

	// set final time
	char *p = strstr(buff2,"%tf");
	if (p!=NULL)
	{
		p[1]='g';
		p[2]=' ';
	}
	// sprintf(buff,buff2,getFinalTime());
	sprintf(buff,buff2,RootSimulator::FinalTime);

	// replace every "@" with ";"
	for (unsigned int i=0;i<strlen(buff);i++) 
	   if (buff[i]=='@') buff[i]=';'; // Escaping characters
	strcat(buff,"\nset datafile separator \",\"\nplot ");
	//for (int i=0;i<n;i++) {
	//	strcat(buff," \"");
	//	strcat(buff,FName);
	//	strcat(buff,"\" using 1:");
	//	ninput[0]='2'+i;
	//	strcat(buff, ninput);
	//	strcat(buff, format[i]);
	//	if (i<n-1) strcat( buff,", ");
	//	}
	strcat(buff," \"");
	strcat(buff,FName);
	strcat(buff,"\" using 2:3 ");
	strcat(buff,format[0]);
	//strcat(buff,format[1]);

	strcat(buff,"\n");
	strcpy(printString,buff);
	strcat(buff,"pause 0.5\nload \"");
	strcat(buff,Script);
	strcat(buff,"\"\n");

	// open plt file, write formatting directives, close file
	fd = PDFileOpen(Script,'w');
	PDFileWrite(fd,buff,strlen(buff));
	PDFileClose(fd);

	if (getOs()!=WINDOWS) {
		spawnProcess("/usr/bin/gnuplot",Script);
		// sprintf(buff,"find ./plots/%s.csv -ctime 1 -exec rm -rf {} \\;","*");
		// sprintf(buff,"find ./plots/%s.csv -ctime -1 -exec rm -rf '{}' \\;","*");
		// printLog(buff);
		// int res = system(buff);//delete old .csv files
		// sprintf(buff,"find ./plots/%s.plt -ctime 1 -exec rm -rf {} \\;","*");
		// sprintf(buff,"find ./plots/%s.plt -ctime -1 -exec rm -rf '{}' \\;","*");
		// res = system(buff);//delete old .plt files
		//if (res<=0) printLog("Failed system in gnuplot\n");
	} else {
		char buff[1024];
		strcpy(buff,Script);
		strcat(buff," -");
		spawnProcess("../bin/gnuplot/bin/wgnuplot.exe",buff);
	}
	foutput = PDFileOpen(FName,'a');

	order = 1;
	sigma = 1e20;

}
double gnuplotxy2::ta(double t) {
	//This function returns a double.
	return sigma;
}
void gnuplotxy2::dint(double t) {
	sigma=1e20;
}
void gnuplotxy2::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	double *Xv;
	char temp[64];
	char buf[1024];
	Xv = (double*)(x.value);

	//printLog("Recibi un evento %g\n",Aux[0]);

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

	// csv file row: "tvalue, xvalue, yvalue\n"
	sprintf(temp,"%g, ",t);
	strcpy(buf,temp);
	sprintf(temp,"%g, ",(X[0][0]));
	strcat(buf,temp);
	sprintf(temp,"%g\n",(X[1][0]));
	strcat(buf,temp);
	PDFileWrite(foutput,buf,strlen(buf));

	#ifdef __WINDOWS__
	fclose((FILE*)foutput);
	foutput=(int)fopen(FName,"a"); 
	#endif

	sigma=1e20;
}
Event gnuplotxy2::lambda(double t) {
	return Event();
}
void gnuplotxy2::exit() {

	PDFileClose(foutput);
	char Script[128];
	char buff[128];
	sprintf(Script,"plots/%ld.plt",(long int)this);
	long int fd=PDFileOpen(Script,'w');
	PDFileWrite(fd,printString,strlen(printString));

	if (getOs()!=WINDOWS){
		strcpy(buff,"\nset terminal wxt persist"); 
		strcpy(buff,"\npause -1"); //strcpy(buff,"\nset terminal qt persist");
	} 	else {
		strcpy(buff,"\npause 3600");
	}

	PDFileWrite(fd,buff,strlen(buff));
	PDFileClose(fd);
}
