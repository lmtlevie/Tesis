#include "Generator.h"
#include <algorithm>

void Generator::init(double t,...) {
    BaseSimulator::init(t);

    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters,t);

    // read parameters
    char* fvar;
    fvar = va_arg(parameters, char*);
    this->period = readDistributionParameter(fvar);
    debugMsg(LOG_LEVEL_INIT, "[%g] %s: Period: %s \n",t, this->getFullName().data(), this->period->getFullName().data());

    // program next transition
//    this->sigma = 0;
    this->sigma = this->period->nextValue(); // NOTE: no event at t=0
}

void Generator::dint(double t) {
   this->sigma = std::max(0.0, this->period->nextValue()); // disallow negative times. TODO: remove max when boundaries implemented for distributions (better the modeler to set cut distributtions properly than inforcing them in a hidder way)
}
void Generator::dext(Event x, double t) {
	debugMsg(LOG_LEVEL_ERROR, "[%g] %s::dext: Generator model does not expect input events \n ", t, this->getFullName().data());
    throw std::runtime_error("Generator::dext Generator model does not expect input events \n");
}

Event Generator::lambda(double t) {
	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s_lambda: generated event \n",t, this->getFullName().data());
    return Event(output, 0);
}
