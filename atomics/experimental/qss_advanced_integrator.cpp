#include "qss_advanced_integrator.h"

void qss_advanced_integrator::init(double t,...) {

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//      %Type% is the parameter type
	Method=va_arg(parameters,char*);
	
	char *fvar1= va_arg(parameters,char*);
	char *fvar2= va_arg(parameters,char*);
	char *fvar3= va_arg(parameters,char*);
	char *fvar4= va_arg(parameters,char*);
	
	/**********************************************
	* NOTE: The actual implementation of the QSS solvers
	* have been split in separate files for maintenance 
	* reasons.
	* QSS solver is in atomics/qss/qss.cpp
	* QSS2 solver is in atomics/qss/qss2.cpp
	* ...
	* ...
	* You can edit any one of them like a normal Atomic
	* model (from the Atomic Editor)
	***********************************************/
	if (strcmp(Method,"QSS")==0) {
		met=QSS;
		solver= new qss_advanced(name);
	} else if (strcmp(Method,"QSS2")==0) {
	    	met=QSS2;       
		solver= new qss2_advanced(name);
	} else if (strcmp(Method,"QSS3")==0){
		met=QSS3;  
		solver= new qss3_advanced(name);
	} else if (strcmp(Method,"QSS4")==0){
	    	met=QSS4;  
		solver= new qss4_advanced(name);
	}
	solver->init(t,fvar1,fvar2,fvar3,fvar4);
		
}

double qss_advanced_integrator::ta(double t) {

	//This function return a double.
	return solver->ta(t);

}

void qss_advanced_integrator::dint(double t) {

	solver->dint(t);

}

void qss_advanced_integrator::dext(Event x, double t) {

	solver->e = e;
	solver->dext(x,t);

}

Event qss_advanced_integrator::lambda(double t) {

	Event e=solver->lambda(t);
	return e;

}

void qss_advanced_integrator::exit() {

	solver->exit();
	delete solver;

}
