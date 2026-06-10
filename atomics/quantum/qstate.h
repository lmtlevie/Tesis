#if !defined qstate_h
#define qstate_h

#include <complex>

struct QuantumState {
	int N;
	int dim;
	std::complex<double>* psi;
	int* measurements;
};

#endif
