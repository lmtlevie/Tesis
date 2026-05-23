#include "qss_signal_filter.h"
void qss_signal_filter::init(double t,...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	dQmin = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: dQmin=%g \n", this->getFullName().data(), dQmin);
	fvar= va_arg(parameters,char*);
	dQrel = readDefaultParameterValue<double>(fvar );
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: dQrel=%g \n", this->getFullName().data(), dQrel);

	for(int i=0;i<10;i++){
		u[i]=0;
		y[i]=0;
		diff[i]=0;
	};

	sigma=std::numeric_limits<double>::infinity();
}
//double qss_signal_filter::ta(double t) {
//return sigma;
//}
void qss_signal_filter::dint(double t) {
	sigma = std::numeric_limits<double>::infinity();
}
void qss_signal_filter::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);
	for (int i=0;i<4;i++) {
		u[i]=xv[i];
	}

	advance_time(y, e,-1); // advance output

	double tol=fabs(y[0])*dQrel;
	if (tol<dQmin) tol=dQmin;

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::dext: u=[%g][%g][%g] ; y=[%g][%g][%g] ; tol=%g \n", t, this->getFullName().data(), u[0], u[1], u[2], y[0], y[1], y[2], tol);
	if ((u[0] >= y[0]+tol)||(u[0]<y[0]-tol)) { // new signal outside error bounds --> forward immediately
		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::dext: Outside bounds. Sending now \n", t, this->getFullName().data());
		sigma=0; // program transition immediately
	} else {
		//we compute next crossing time
		for (int i=0;i<4;i++) {
			diff[i]=u[i]-y[i];
		}
		diff[0] -= tol;
		double sigma1=minposroot(diff,4); // lower bound crossing
		diff[0] += tol+tol;
		sigma=minposroot(diff,4); // upper bound crossing
		if (sigma1<sigma)sigma=sigma1;

		debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::dext: Inside bounds sigma=%g ; diff=[%g][%g][%g] \n", t, this->getFullName().data(), sigma, diff[0], diff[1], diff[2]);
	}
}
Event qss_signal_filter::lambda(double t) {
	advance_time(u, sigma,-1);
	for (int i=0;i<4;i++) {
		y[i]=u[i];
	}

	debugMsg(LOG_LEVEL_DEBUG, "[%g] %s::dext: Sending y=[%g][%g][%g] \n", t, this->getFullName().data(), y[0], y[1], y[2]);
	return Event(y,0);
}
//void qss_signal_filter::exit() {
//
//}
