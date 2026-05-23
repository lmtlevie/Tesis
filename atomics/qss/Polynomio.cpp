#include "Polynomio.h"

// TODO: assumes coefficients[10]
Polynomio::Polynomio (double* coeffs){
	for(int i=0; i<10; i++){
		this->coefficients[i] = coeffs[i];
		if(coeffs[i]!=0) {
			this->lastNonCeroCoeff = i;
		}
	}
}

Polynomio::Polynomio (std::vector<double> coeffs) : Polynomio(&coeffs[0]){

}

unsigned int Polynomio::order() const{
	return this->lastNonCeroCoeff;
}


double& Polynomio::operator[](unsigned int index) {
	if(index >= 10){
		throw std::runtime_error("Polynomio::operator[] trying to access invalid index");
	}

	return coefficients[index];
}

/*
 * return the time at which the polinomio will be equal to crossingValue
 * (wrapper for minposroot)
 */
double Polynomio::nextCrossing(double crossingValue){
	coefficients[0]=coefficients[0]-crossingValue;
	double crossing1 = minposroot(&coefficients[0], this->order());
	coefficients[0]=coefficients[0]+crossingValue; // back to original value

	//coefficients[0]=coefficients[0]+crossingValue;
	//double crossing2 = minposroot(&coefficients[0], this->order());
	//coefficients[0]=coefficients[0]-crossingValue;// back to original value

	return crossing1;
}

/*
 * advances the polinomio t seconds
 * (wrapper for advance_time)
 */
void Polynomio::advanceTime(double t){
	advance_time(&coefficients[0], t, this->order());
}
