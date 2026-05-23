#include "qss_saturation.h"
void qss_saturation::init(double t,...) {
va_list parameters;
va_start(parameters,t);

char *fvar= va_arg(parameters,char*); 
xl= readDefaultParameterValue<double>(fvar );
fvar= va_arg(parameters,char*);
xu= readDefaultParameterValue<double>(fvar );

for(int i=0;i<10;i++){
	u[i]=0;
	y[i]=0;
};  

if (xl>0){
	sw=-1; // low saturation
 } else {
	if (xu<0) {
		sw=1; // high saturation
	} else {
		sw=0; // no saturation
	};
};
sigma=0;
}
double qss_saturation::ta(double t) {
	//printLog("[%g] %s::ta sigma=%g ; sw=%d; u=[%g, %g, %g] \n",t, this->getName(), sigma, sw, u[0], u[1], u[2]);     // DEBUG
	return sigma;
}
void qss_saturation::dint(double t) {
if (sigma>0) {
	//change in saturation state
	if (sw==0) {
		//we enter saturation
		if (u[0]>0.5*(xl+xu)){
			//we are in up saturation
			sw=1;
		} else {
			//we are in low saturation
			sw=-1;
		};			
	} else {
		//we enter linear zone
		sw=0;	
	};
} else {
	if (u[0]<=xl) {
		sw=-1;	
   } else if (u[0]>=xu) {
		sw=1;
	} else {
		sw=0;
	};
};

//printLog("[%g] %s::dint set up saturation sw=%d \n",t, this->getName(), sw);     // DEBUG

switch(sw) {
	case -1: // low saturation
		if (u[0]==xl)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]>xl) {
		 //we are outside low saturation
			sigma=0;
		} else {
			u[0]=u[0]-xl;
			sigma=minposroot(u,4);
			u[0]=u[0]+xl;

			if(u[0]==xl && sigma==0){ // in such case we would cycle indefinitely because of numerical errors. TODO: check if other solutions might apply better (ej: at minposroot)
				//TODO (Matias): 1e-20 might not be enough to cross discontinuity due to numerical precision (ej:xu=100, u[0]=100, u[1]=-50 => u[0] will remain 100)
				// workaround: force a bigger step in time (is this enough, is this too much?)
				//printLog("[%g] %s::dint enter workaround for low saturation and setting sigma=1e-3 \n",t, this->getName(), this->getName());     // DEBUG
				sigma=1e-3;
			}
		}
	break;

	case 0: // no saturation
		if (u[0]==xl)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]==xu)advance_time(u,1e-20,-1); //cross the discontinuity
		if ((u[0]<xl) ||(u[0]>xu)){
		 //we are outside linear zone
			sigma=0;
		} else {
			u[0]=u[0]-xl;
			double sigma1=minposroot(u,4); // cross low saturation threshold
			u[0]=u[0]+xl-xu;
			sigma=minposroot(u,4); // cross up saturation threshold
			u[0]=u[0]+xu;
			if (sigma1<sigma)sigma=sigma1; // keep first crossing

//			if(sigma==0){ // in such case we would cycle indefinitely because of numerical errors. TODO: check if other solutions might apply better (ej: at minposroot)
//				//TODO (Matias): 1e-20 might not be enough to cross discontinuity due to numerical precision (ej:xu=100, u[0]=100, u[1]=-50 => u[0] will remain 100)
//				// workaround: force a bigger step in time (is this enough, is this too much?)
//				printLog("[%g] %s::dint enter workaround for no saturation and setting sigma=1e-3 \n",t, this->getName(), this->getName());     // DEBUG
//				sigma=1e-3;
//			}
		}
		break;

	case 1: // up saturation
		if (u[0]==xu)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]<xu) {
		 //we are outside up saturation
			sigma=0;
		} else {
			u[0]=u[0]-xu;
			sigma=minposroot(u,4);
			u[0]=u[0]+xu;

			if(u[0]==xu && sigma==0){ // u is exactly in the saturation limit (although we already advance it 1e-20, so appears not to be moving) but at the same time it will cross the limit in 0s (sigma=0)
				// in such case we would cycle indefinitely because of numerical errors. TODO: check if other solutions might apply better (ej: at minposroot)
				//TODO (Matias): 1e-20 might not be enough to cross discontinuity due to numerical precision (ej:xu=100, u[0]=100, u[1]=-50 => u[0] will remain 100)
				// workaround: force a bigger step in time (is this enough, is this too much?)
				//printLog("[%g] %s::dint enter workaround for up saturation and setting sigma=1e-3 \n",t, this->getName());     // DEBUG
				sigma=1e-3;
			}
		}

}; 

		 
//printLog("Internal: We set sigma=%g \n",sigma);
}
void qss_saturation::dext(Event x, double t) {
double *xv;
xv=(double*)(x.value);

u[0]=xv[0];
u[1]=xv[1];
u[2]=xv[2];
u[3]=xv[3];

//printLog("[%g] %s::dext u=[%g %g %g] \n",t, this->getName(), u[0], u[1], u[2]);     // DEBUG

switch(sw) {
	case -1:
		if (u[0]==xl)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]>xl) {
		 //we are outside low saturation
			sigma=0;
		} else {
			u[0]=u[0]-xl;
			sigma=minposroot(u,4);
			u[0]=u[0]+xl;
		}
	break;

	case 0:

		sigma=0;
	break;

	case 1:
		if (u[0]==xu)advance_time(u,1e-20,-1); //cross the discontinuity
		if (u[0]<xu) {
		 //we are outside up saturation
			sigma=0;
		} else {
			u[0]=u[0]-xu;
			sigma=minposroot(u,4);
			u[0]=u[0]+xu;
		}

};

}
Event qss_saturation::lambda(double t) {
if (sigma>0) {
	//change in saturation state
	advance_time(u,sigma+1e-20,-1);//this should be done at internal transition
	if (sw==0) {
		//we enter saturation
		if (u[0]>0.5*(xl+xu)){
			//we are in up saturation
			//printLog("[%g] %s::lambda output up saturation \n",t, this->getName());     // DEBUG
			y[0]=xu;
			y[1]=0;	
			y[2]=0;	
			y[3]=0;	
		} else {
			//we are in low saturation
			//printLog("[%g] %s::lambda output low saturation \n",t, this->getName());     // DEBUG
			y[0]=xl;
			y[1]=0;	
			y[2]=0;	
			y[3]=0;	
		};			
	} else {
		//we enter linear zone
		//printLog("[%g] %s::lambda no saturation (output=input) \n",t, this->getName());     // DEBUG
		y[0]=u[0];
		y[1]=u[1];
		y[2]=u[2];
		y[3]=u[3];
	};
} else {
	if (u[0]<=xl) {
		y[0]=xl;
		y[1]=0;	
		y[2]=0;	
		y[3]=0;	
   } else if (u[0]>=xu) {
		y[0]=xu;
		y[1]=0;	
		y[2]=0;	
		y[3]=0;	
	} else {
		y[0]=u[0];
		y[1]=u[1];	
		y[2]=u[2];	
		y[3]=u[3];	
	};
};		 
//printLog("[%g] %s::lambda y=[%g %g %g] \n",t, this->getName(), y[0], y[1], y[2]);     // DEBUG
return Event(y,0);
}
void qss_saturation::exit() {

}
