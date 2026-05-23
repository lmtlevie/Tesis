#include "CustomSteps.h"
void CustomSteps::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar;
//	debugMsg(LOG_LEVEL_ALWAYS, "reading a vector<double> param: \n", fvar);

	fvar = va_arg(parameters,char*);
	this->stepTimeChanges = readDefaultParameterValue<std::vector<double> >(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: stepTimeChanges has %u elements \n", this->getFullName().data(), this->stepTimeChanges.size());

	fvar = va_arg(parameters,char*);
	this->stepValueChanges = readDefaultParameterValue<std::vector<double> >(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: stepValueChanges has %u elements \n", this->getFullName().data(), this->stepValueChanges.size());

	uint n = this->stepValueChanges.size();

	fvar = va_arg(parameters,char*);
	this->stepValueChanges1 = std::vector<double>(n, 0); // if no params assume 0
	if(strcmp(fvar, "")!=0){
		this->stepValueChanges1 = readDefaultParameterValue<std::vector<double> >(fvar);
	}

	fvar = va_arg(parameters,char*);
	this->stepValueChanges2 = std::vector<double>(n, 0); // if no params assume 0
	if(strcmp(fvar, "")!=0){
		this->stepValueChanges2 = readDefaultParameterValue<std::vector<double> >(fvar);
	}

	fvar = va_arg(parameters,char*);
	this->stepValueChanges3 = std::vector<double>(n, 0); // if no params assume 0
	if(strcmp(fvar, "")!=0){
		this->stepValueChanges3 = readDefaultParameterValue<std::vector<double> >(fvar);
	}

	if(n != this->stepTimeChanges.size() || n != this->stepValueChanges1.size() || n != this->stepValueChanges2.size() || n != this->stepValueChanges3.size()){
		//debugMsg(LOG_LEVEL_ERROR, "customSteps parameters are wrong. The size of the time and value changes must be the same. \n");
		throw std::runtime_error(this->getFullName() + ": customStep parameters are wrong. The size of the time and value arrays must be the same.");
	}

	for(int i=0; i<10; i++){ y[i]=0; };

	this->sigma = std::numeric_limits<double>::infinity(); // stay idle for ever
	if(!this->stepTimeChanges.empty()){
		this->sigma = stepTimeChanges.front(); // take the next t
	}
}

void CustomSteps::dint(double t) {
	// delete previously used values
	this->stepTimeChanges.erase(this->stepTimeChanges.begin());
	this->stepValueChanges.erase(this->stepValueChanges.begin());
	this->stepValueChanges1.erase(this->stepValueChanges1.begin());
	this->stepValueChanges2.erase(this->stepValueChanges2.begin());
	this->stepValueChanges3.erase(this->stepValueChanges3.begin());

	// set new sigma
	this->sigma = std::numeric_limits<double>::infinity(); // stay idle for ever
	if(!this->stepTimeChanges.empty()){
		this->sigma = std::max(0.0, stepTimeChanges.front() - t); // take the next t
	}
}

void CustomSteps::dext(Event x, double t) {
	// no events received by this models

}

Event CustomSteps::lambda(double t) {
	y[0] = stepValueChanges.front();
	y[1] = stepValueChanges1.front();
	y[2] = stepValueChanges2.front();
	y[3] = stepValueChanges3.front();
	//debugMsg(LOG_LEVEL_ALWAYS, "[%g] %s - sending event with value %g \n", t, this->getFullName().data(), y[0]);
	return Event(&y[0],0);
}


