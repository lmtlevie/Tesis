#if !defined quantum_common_h
#define quantum_common_h

#include <complex>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "qstate.h"

inline double qparse_d(const char *fvar) {
	return (fvar && *fvar) ? std::atof(fvar) : 0.0;
}

inline int qparse_i(const char *fvar) {
	return (fvar && *fvar) ? std::atoi(fvar) : 0;
}

inline void apply1(QuantumState* s, int q, const std::complex<double> U[4]) {
	if (!s || q < 0 || q >= s->N) return;
	int m = 1 << q;
	int dim = s->dim;
	for (int i = 0; i < dim; i++) {
		if (i & m) continue;
		int j = i | m;
		std::complex<double> a = s->psi[i], b = s->psi[j];
		s->psi[i] = U[0]*a + U[1]*b;
		s->psi[j] = U[2]*a + U[3]*b;
	}
}

inline void apply2(QuantumState* s, int q0, int q1, const std::complex<double> U[16]) {
	if (!s || q0 < 0 || q1 < 0 || q0 >= s->N || q1 >= s->N || q0 == q1) return;
	int m0 = 1 << q0, m1 = 1 << q1;
	int dim = s->dim;
	for (int i = 0; i < dim; i++) {
		if ((i & m0) || (i & m1)) continue;
		int idx[4];
		idx[0] = i;
		idx[1] = i | m0;
		idx[2] = i | m1;
		idx[3] = i | m0 | m1;
		std::complex<double> v[4] = {s->psi[idx[0]], s->psi[idx[1]], s->psi[idx[2]], s->psi[idx[3]]};
		for (int r = 0; r < 4; r++) {
			std::complex<double> sum(0,0);
			for (int c = 0; c < 4; c++) sum += U[r*4+c] * v[c];
			s->psi[idx[r]] = sum;
		}
	}
}

#endif
