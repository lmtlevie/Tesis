#include "hybridRateToEvents.h"
void hybridRateToEvents::init(double t,...) {
    BaseSimulator::init(t);

    va_list parameters;
    va_start(parameters,t);

    char* fvar;

    fvar = va_arg(parameters,char*);
    this->delta_min =  readDefaultParameterValue<double>(fvar );

    debugMsg(LOG_LEVEL_DEBUG, "[%g] %s: delta_min: %g sec \n",t, this->getFullName().data(), this->delta_min);

    fvar = va_arg(parameters,char*);
    this->generationDistribution = std::string(fvar);
    if(this->generationDistribution == "normal"){ // in case of normal distribution
        fvar = va_arg(parameters,char*);
        this->normalVariance =  readDefaultParameterValue<double>(fvar );
    }

    sigma = std::numeric_limits<double>::infinity();

    this->logger->initSignals(std::vector<std::string>{"rate", "sentEvent"});

    return;
}

double hybridRateToEvents::getStochasticSigma(double mean){
    if(this->generationDistribution == "constant"){
        return mean;
    } else if (this->generationDistribution == "normal"){
        return STDEVS::getUniqueGenerationInstance()->normal(mean, mean/this->normalVariance);
    } else if (this->generationDistribution == "exponential"){
        return STDEVS::getUniqueGenerationInstance()->exponential(mean);
    }
}

void hybridRateToEvents::dint(double t) {
    advance_time(this->rate_hz, e, -1);
    if(this->rate_hz[0]<0) this->rate_hz[0] = 0; // bad bad bad

    this->logger->logSignal(t, this->rate_hz[0], "rate");

    // set new mean
    this->sigma = getStochasticSigma(1/this->rate_hz[0]); //std::max(0.0, STDEVS::getUniqueGenerationInstance()->normal(this->sigma, this->sigma/this->normalVariance));
    this->sigma  = std::max(0.0, this->sigma - (t-this->lastEventTime));

    this->generateEvent = true;
    if(this->sigma > this->delta_min){
        this->sigma = this->delta_min;
        this->generateEvent = false;
    }
}

void hybridRateToEvents::dext(Event x, double t) {
    switch (x.port) {
    case 0: { // inport0: rate_hz
        double *aux = (double*)x.value;
        for (int i=0;i<10;i++) { // copy polinomio (QSS values)
            this->rate_hz[i]=aux[i];
        }

//        this->sigma = std::max(0.0, 1/this->rate_hz[0] - (t-this->lastEventTime)); // new sigma for new rate
        this->sigma = getStochasticSigma(1/this->rate_hz[0]); //std::max(0.0, STDEVS::getUniqueGenerationInstance()->normal(this->sigma, this->sigma/this->normalVariance));
        this->sigma  = std::max(0.0, this->sigma - (t-this->lastEventTime));

        this->generateEvent = true;
        if(this->sigma > this->delta_min){
            this->sigma = this->delta_min; // force a transition sooner to update rate[0] without generating an event
            this->generateEvent = false;
        }

        this->logger->logSignal(t, this->rate_hz[0], "rate");
        debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: New rate arrived: rate[0]=%g, rate[1]=%g . Sigma=%g\n",t, this->getFullName().data(), rate_hz[0], rate_hz[1], this->sigma);
        break;
    }
    default: {
        throw std::runtime_error("hybridRateToEvents::dext: wrong port /n");
        break;
    }
    }

    return ;
}

Event hybridRateToEvents::lambda(double t) {
    if(this->generateEvent){
        debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Sending Event \n", t, this->getFullName().data());

        this->logger->logSignal(t, 1, "sentEvent");
        this->lastEventTime = t;
        return Event(this->output, 0);
    } else{
        debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[lambda] Update rate[0] \n", t, this->getFullName().data());
        return Event();
    }
}

