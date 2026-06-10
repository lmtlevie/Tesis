//CPP:quantum/quantum_context.cpp
#if !defined quantum_context_h
#define quantum_context_h

#include <complex>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "qstate.h"

class Simulator;

class QuantumContext {
int nqubits;
bool locked;
int next_auto_qubit;
std::vector<int> initial_bits;
std::vector< std::complex<double> > psi_storage;
std::vector<int> meas_storage;
QuantumState state_data;

void rebuild_basis_state();

public:
	QuantumContext();
	void configure(int n, const std::string& initial_state);
	int size() const;
	int allocate_qubit();
	QuantumState* state();
	void set_initial_qubit(int q, int value);
	void apply_one(int q, const std::complex<double> U[4]);
	void apply_cnot(int control, int target);
	int measure(int q, std::mt19937& rng);
};

struct QuantumWire {
	QuantumContext* context;
	int qubit;
};

QuantumContext* quantum_context_for(Simulator* model, int min_qubits = 1);
void clear_quantum_context_for(Simulator* model);

#endif
