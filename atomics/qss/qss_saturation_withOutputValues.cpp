#include "qss_saturation_withOutputValues.h"
void qss_saturation_withOutputValues::init(double t,...) {
	va_list parameters;
	va_start(parameters,t);

	char *fvar= va_arg(parameters,char*);
	lowLimit= readDefaultParameterValue<double>(fvar );
	fvar= va_arg(parameters,char*);
	upperLimit= readDefaultParameterValue<double>(fvar );


	// TODO: just to test
	noSaturationOutput[0] = 100;
	//saturationOutput = {0};

	if (lowLimit >= 0){
		saturationState=LOW_SATURATION;
	} else {
		if (upperLimit <= 0) {
			saturationState=HIGH_SATURATION;
		} else {
			saturationState=NO_SATURATION;
		}
	}

	sigma=0;
}

double qss_saturation_withOutputValues::ta(double t) {
	return sigma;
}

void qss_saturation_withOutputValues::dint(double t) {
	//printLog("Switch internal transition \n");
	if (sigma>0) {
		//change in saturation state
		if (saturationState==0) {
			//we enter saturation
			if (u[0]>0.5*(lowLimit+upperLimit)){
				//we are in up saturation
				saturationState=HIGH_SATURATION;
			} else {
				//we are in low saturation
				saturationState=LOW_SATURATION;
			};
		} else {
			//we enter linear zone
			saturationState=NO_SATURATION;
		};
	} else {
		if (u[0]<=lowLimit) {
			saturationState=LOW_SATURATION;
		} else if (u[0]>=upperLimit) {
			saturationState=HIGH_SATURATION;
		} else {
			saturationState=NO_SATURATION;
		};
	};

	switch(saturationState) {
	case LOW_SATURATION:
		if (u[0]==lowLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]>lowLimit) {
			//we are outside low saturation
			sigma=0;
		} else {
			u[0]=u[0]-lowLimit;
			sigma=minposroot(u,4);
			u[0]=u[0]+lowLimit;
		}
		break;

	case NO_SATURATION:
		if (u[0]==lowLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]==upperLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if ((u[0]<=lowLimit) ||(u[0]>=upperLimit)){
			//we are outside linear zone
			sigma=0;
		} else {
			u[0]=u[0]-lowLimit;
			double sigma1=minposroot(u,4);
			u[0]=u[0]+lowLimit-upperLimit;
			sigma=minposroot(u,4);
			u[0]=u[0]+upperLimit;
			if (sigma1<sigma)sigma=sigma1;
		}
		break;

	case HIGH_SATURATION:
		if (u[0]==upperLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]<upperLimit) {
			//we are outside up saturation
			sigma=0;
		} else {
			u[0]=u[0]-upperLimit;
			sigma=minposroot(u,4);
			u[0]=u[0]+upperLimit;
		}

	};


	//printLog("Internal: We set sigma=%g \n",sigma);
}
void qss_saturation_withOutputValues::dext(Event x, double t) {
	double *xv;
	xv=(double*)(x.value);

	//printLog("t=%g : We entered delta_ext \n",t);

	u[0]=xv[0];
	u[1]=xv[1];
	u[2]=xv[2];
	u[3]=xv[3];

	switch(saturationState) {
	case LOW_SATURATION:
		if (u[0]==lowLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]>lowLimit) {
			//we are outside low saturation
			sigma=0;
		} else {
			u[0]=u[0]-lowLimit;
			sigma=minposroot(u,4);
			u[0]=u[0]+lowLimit;
		}
		break;

	case NO_SATURATION:

		sigma=0;
		break;

	case HIGH_SATURATION:
		if (u[0]==upperLimit)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]<upperLimit) {
			//we are outside up saturation
			sigma=0;
		} else {
			u[0]=u[0]-upperLimit;
			sigma=minposroot(u,4);
			u[0]=u[0]+upperLimit;
		}

	};



	//printLog("t=%g: We have u[0]=%g , saturationState=%i and we set sigma=%g \n",t,u[0],sw,sigma);
}
Event qss_saturation_withOutputValues::lambda(double t) {
	if (sigma>0) {
		//change in saturation state
		advance_time(u,sigma+1e-20,-1);//this should be done at internal transition
		if (saturationState == NO_SATURATION) {
			//we enter saturation
			if (u[0]>0.5*(lowLimit+upperLimit)){ // TODO: review why this formula. where does 0.5 comes from?
				//we are in up saturation
				y[0]=saturationOutput[0];
				y[1]=saturationOutput[1];
				y[2]=saturationOutput[2];
				y[3]=saturationOutput[3];
			} else {
				//we are in low saturation
				y[0]=saturationOutput[0];
				y[1]=saturationOutput[1];
				y[2]=saturationOutput[2];
				y[3]=saturationOutput[3];
			};
		} else {
			//we enter linear zone
			y[0]=noSaturationOutput[0];
			y[1]=noSaturationOutput[1];
			y[2]=noSaturationOutput[2];
			y[3]=noSaturationOutput[3];
		};
	} else {
		if (u[0]<=lowLimit) {
			y[0]=saturationOutput[0];
			y[1]=saturationOutput[1];
			y[2]=saturationOutput[2];
			y[3]=saturationOutput[3];
		} else if (u[0]>=upperLimit) {
			y[0]=saturationOutput[0];
			y[1]=saturationOutput[1];
			y[2]=saturationOutput[2];
			y[3]=saturationOutput[3];
		} else {
			y[0]=noSaturationOutput[0];
			y[1]=noSaturationOutput[1];
			y[2]=noSaturationOutput[2];
			y[3]=noSaturationOutput[3];
		};
	};
	return Event(y,0);
}

void qss_saturation_withOutputValues::exit() {

}

void qss_saturation_withOutputValues::setOutput(){

}
