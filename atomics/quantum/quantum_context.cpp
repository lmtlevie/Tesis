#include "quantum_context.h"
#include "quantum_common.h"
#include "simulator.h"
#include "coupling.h"
#include <cmath>

static std::map<std::string, QuantumContext> quantum_contexts;

static std::string context_key_for(Simulator* model) {
	if (!model || !model->father) return "__quantum_root__";
	return model->father->getFullName();
}

QuantumContext::QuantumContext() {
	nqubits = 0;
	locked = false;
	next_auto_qubit = 0;
	state_data.N = 0;
	state_data.dim = 0;
	state_data.psi = NULL;
	state_data.measurements = NULL;
}

void QuantumContext::configure(int n, const std::string& initial_state) {
	if (n < 1) n = 1;
	nqubits = n;
	locked = false;
	next_auto_qubit = 0;
	initial_bits.assign(nqubits, 0);
	int len = (int)initial_state.size();
	for (int i = 0; i < nqubits && i < len; i++) {
		char c = initial_state[len - 1 - i];
		initial_bits[i] = (c == '1') ? 1 : 0;
	}
	psi_storage.assign(1 << nqubits, std::complex<double>(0,0));
	meas_storage.assign(nqubits, -1);
	state_data.N = nqubits;
	state_data.dim = 1 << nqubits;
	state_data.psi = psi_storage.data();
	state_data.measurements = meas_storage.data();
	rebuild_basis_state();
}

int QuantumContext::size() const {
	return nqubits;
}

int QuantumContext::allocate_qubit() {
	if (nqubits < 1) configure(1, "0");
	if (next_auto_qubit >= nqubits) return -1;
	return next_auto_qubit++;
}

QuantumState* QuantumContext::state() {
	return &state_data;
}

void QuantumContext::rebuild_basis_state() {
	if (nqubits < 1) configure(1, "0");
	for (int i = 0; i < state_data.dim; i++) psi_storage[i] = std::complex<double>(0,0);
	int idx = 0;
	for (int q = 0; q < nqubits; q++) {
		if (initial_bits[q]) idx |= (1 << q);
	}
	psi_storage[idx] = std::complex<double>(1,0);
	for (int q = 0; q < nqubits; q++) meas_storage[q] = -1;
	state_data.psi = psi_storage.data();
	state_data.measurements = meas_storage.data();
}

void QuantumContext::set_initial_qubit(int q, int value) {
	if (q < 0) return;
	if (nqubits < 1) configure(q + 1, "0");
	if (q >= nqubits || locked) return;
	initial_bits[q] = value ? 1 : 0;
	rebuild_basis_state();
}

void QuantumContext::apply_one(int q, const std::complex<double> U[4]) {
	if (q < 0 || q >= nqubits) return;
	apply1(&state_data, q, U);
	locked = true;
}

void QuantumContext::apply_cnot(int control, int target) {
	if (control < 0 || target < 0 || control >= nqubits || target >= nqubits || control == target) return;
	std::complex<double> U[16];
	for (int k = 0; k < 16; k++) U[k] = std::complex<double>(0,0);
	U[0*4+0] = std::complex<double>(1,0);
	U[2*4+2] = std::complex<double>(1,0);
	U[3*4+1] = std::complex<double>(1,0);
	U[1*4+3] = std::complex<double>(1,0);
	apply2(&state_data, control, target, U);
	locked = true;
}

int QuantumContext::measure(int q, std::mt19937& rng) {
	if (q < 0 || q >= nqubits) return -1;
	double p0 = 0.0;
	for (int i = 0; i < state_data.dim; i++)
		if (((i >> q) & 1) == 0) p0 += std::norm(psi_storage[i]);
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	int outcome = (dist(rng) < p0) ? 0 : 1;
	double pkeep = (outcome == 0) ? p0 : (1.0 - p0);
	double norm = (pkeep > 1e-15) ? std::sqrt(pkeep) : 1.0;
	for (int i = 0; i < state_data.dim; i++) {
		if (((i >> q) & 1) != outcome) psi_storage[i] = std::complex<double>(0,0);
		else psi_storage[i] /= norm;
	}
	meas_storage[q] = outcome;
	locked = true;
	return outcome;
}

QuantumContext* quantum_context_for(Simulator* model, int min_qubits) {
	std::string key = context_key_for(model);
	QuantumContext& ctx = quantum_contexts[key];
	if (ctx.size() < 1) {
		if (min_qubits < 1) min_qubits = 1;
		ctx.configure(min_qubits, "0");
	}
	return &ctx;
}

void clear_quantum_context_for(Simulator* model) {
	quantum_contexts.erase(context_key_for(model));
}
