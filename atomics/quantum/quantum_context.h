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
int shot_count;
int current_shot;
double shot_period_value;
std::vector<int> initial_bits;
std::vector< std::complex<double> > psi_storage;
std::vector<int> meas_storage;
QuantumState state_data;

void rebuild_basis_state();

public:
	QuantumContext();
	void configure(int n, const std::string& initial_state);
	void configure_shots(int shots, double period);
	int size() const;
	int shots() const;
	double shot_period() const;
	int allocate_qubit();
	QuantumState* state();
	void set_initial_qubit(int q, int value);
	void begin_shot(int shot);
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
