#include "implicitDelayQSS.h"
void implicitDelayQSS::init(double t, ...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters, t);

	this->sigma = std::numeric_limits<double>::infinity(); // there is no output until we get the first y(t)
	this->delay = std::make_shared<Polynomio>(std::vector<double> { 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0 }); // assume at 0 delay at T=0

	// signal logger
	this->logger->initSignals(std::map<std::string, int>{
		{"in_y", LOG_LEVEL_DEBUG},
		{"in_tau", LOG_LEVEL_DEBUG},
		{"out", LOG_LEVEL_FULL_LOGGING}});
}

void implicitDelayQSS::dint(double t) {
	// advance signals
	advanceSignals(this->e);

	if (!this->programmedOutputs.empty()) {
		this->programmedOutputs.pop_front(); // remove last output that was just sent
	}

	this->sigma = this->nextSigma(t);
}

void implicitDelayQSS::dext(Event x, double t) {
	double *xv = (double*) (x.value);
	Polynomio arrivedSignal = Polynomio(xv); // copy arrived signal into polyniomio

	// advance signals
	//	advanceSignals(this->e);

	// store arrived signal and time
	switch (x.port) {
	case 0: // arrival of new tau(t)
		this->logger->logSignal(t, xv[0], "in_tau");
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: new tau arrived Tau=[%g, %g, %g, %g]\n",t, this->getFullName().data(), xv[0], xv[1], xv[2], xv[3]);

		//verify derivative > -1
		if (arrivedSignal[1] < -1) {
			//std::string error = boost::str(boost::format("[%g] %s[dext]: ERROR -  Wrong delay received. Implicit dQSS does not allow delays with derivative < -1. arrivedSignal=[%g, %g, %g, %g] \n") % t % this->getFullName() % arrivedSignal[0] % arrivedSignal[1] % arrivedSignal[2] % arrivedSignal[3]);
			//			debugMsg(LOG_LEVEL_ERROR, error.data());
			//throw std::runtime_error(error);
		}

		this->delay = std::make_shared<Polynomio>(arrivedSignal);
		if (this->input != nullptr) {
			this->input->advanceTime(this->e);
		}

		break;
	case 1: // arrival of new y(t)
		this->logger->logSignal(t, xv[0], "in_y");
		//debugMsg(LOG_LEVEL_DEBUG, "[%g] %s[dext]: new input arrived Y=[%g, %g, %g, %g]\n",t, this->getFullName().data(), xv[0], xv[1], xv[2], xv[3]);

		this->input = std::make_shared<Polynomio>(arrivedSignal);
		this->delay->advanceTime(this->e);

		break;
	default:
		debugMsg(LOG_LEVEL_ERROR,
				"[%g] %s[dext]: ERROR  in implicit dQSS. wrong port %d\n", t,
				this->getFullName().data(), x.port);
		throw std::runtime_error(
				"ERROR - implicitDelayQSS::dext: wrong port /n");
		break;
	}

	// program delayed transition
	if (this->input != nullptr) {
		double transitionTime = (*this->delay)[0] + t; // transition will occur at t+r(t)
		std::shared_ptr<Polynomio> delayedSignal = this->calculateDelaySignal();

		// verify we are not programming things twice (derivative > -1 should guarantee this, but just in case numerical errors)
		if (!this->programmedOutputs.empty()
				&& this->programmedOutputs.back().first > transitionTime) {
			//debugMsg(LOG_LEVEL_ERROR, "[%g] %s[dext]: Warning in implicit dQSS. Trying to program transition at a T which is smaller to the last programmed transition (error difference:%g). re-programming signal\n", t, this->getFullName().data(), this->programmedOutputs.back().first - transitionTime);

			transitionTime = this->programmedOutputs.back().first;

			//throw std::runtime_error("Error in implicit dQSS. Trying to program transition at a T which is smaller to the last programmed transition. (re-programming signal) " + this->getFullName());
		}

		this->programmedOutputs.push_back( { transitionTime, delayedSignal }); // program transition in delay[0] seconds (d(t+tau)) to output current input (=y(t))
	}

	// program next transition
	this->sigma = this->nextSigma(t);
}

Event implicitDelayQSS::lambda(double t) {
	if (this->programmedOutputs.empty()) {
		return Event();
	}

	// current delay & input
	Polynomio delayedSignal = *this->programmedOutputs.front().second;

	// set all values
	for (int i = 0; i < 10; i++) {
		outputSignal[i] = delayedSignal[i];
	}

	this->logger->logSignal(t, outputSignal[0], "out");
	debugMsg(LOG_LEVEL_IMPORTANT, "[%g] %s[lambda]: sending outsignal=[%g, %g, %g, %g]\n",t, this->getFullName().data(), outputSignal[0], outputSignal[1], outputSignal[2], outputSignal[3]);
	return Event(outputSignal, 0);
}

void implicitDelayQSS::advanceSignals(double dt) {
	//	advance_time(this->outputSignal, dt, -1);

	if (this->input != nullptr) {
		this->input->advanceTime(dt);
	}

	this->delay->advanceTime(dt);
}

std::shared_ptr<Polynomio> implicitDelayQSS::calculateDelaySignal() {
	// rename and get pointer to simplify code
	Polynomio y = *this->input;
	Polynomio tau = *this->delay;
	Polynomio delayedSignal = Polynomio();

	// QSS1
	delayedSignal[0] = y[0]; // a_d = a_y

	// QSS2
	if (y.order() >= 1 && tau[1] != -1) {
		delayedSignal[1] = y[1] / (tau[1] + 1); // b_d = b_y / (b_r + 1) // NOTE: b_r>-1 already verified which avoids division by 0 here
	}


	//	if(y.order() >=2 || tau.order() >=2){
	//		debugMsg(LOG_LEVEL_ERROR, "[??] %s[calculateDelaySignal]: Error in implicit dQSS. Model not implemented for orders >= 2\n", this->getFullName().data());
	//		throw std::runtime_error("implicitDelayQSS::calculateDelaySignal: Error in implicit dQSS. Model not implemented for orders >= 2 /n");
	//	}

	return std::make_shared<Polynomio>(delayedSignal);
}

double implicitDelayQSS::nextSigma(double t) {
	//double sigma = std::numeric_limits<double>::infinity();
	if (!this->programmedOutputs.empty()) {
		return std::max(0.0, this->programmedOutputs[0].first - t);
	}

	return std::numeric_limits<double>::infinity();
}
