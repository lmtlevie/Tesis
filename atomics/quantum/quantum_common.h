#if !defined quantum_common_h
#define quantum_common_h

#include <complex>
#include <cmath>
#include <cstring>
#include "sinks/ParameterReader.h"

inline double qtheta(char *fvar) {
	return (strlen(fvar)==0 ? 0.0 : readDefaultParameterValue<double>(fvar));
}

inline void apply2x2(std::complex<double> out[2], const std::complex<double> in[2],
	std::complex<double> m00, std::complex<double> m01,
	std::complex<double> m10, std::complex<double> m11) {
	out[0] = m00 * in[0] + m01 * in[1];
	out[1] = m10 * in[0] + m11 * in[1];
}

inline void tensor_from_qubits(const std::complex<double> q0[2], const std::complex<double> q1[2],
	std::complex<double> psi[4]) {
	psi[0] = q0[0] * q1[0];
	psi[1] = q0[0] * q1[1];
	psi[2] = q0[1] * q1[0];
	psi[3] = q0[1] * q1[1];
}

inline void split_qubits(const std::complex<double> psi[4],
	std::complex<double> q0[2], std::complex<double> q1[2]) {
	q0[0] = psi[0] + psi[1];
	q0[1] = psi[2] + psi[3];
	q1[0] = psi[0] + psi[2];
	q1[1] = psi[1] + psi[3];
}

#endif
