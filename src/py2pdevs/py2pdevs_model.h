#ifndef __PY2PDEVS_MODEL_BUILDER_H__
#define __PY2PDEVS_MODEL_BUILDER_H__

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/shared_ptr.hpp>

#include "engine.h"

#include "py_types.h"
#include "utils.h"

using namespace boost::python;


class Py2PDEVSModel
{
	RootSimulator *simulator;
	RootCoupling *top;
	double ti;

	std::unordered_map<std::string, std::string> params;

	std::unordered_map<Coupling*, std::vector<Simulator*>> children;
	std::unordered_map<Coupling*, std::vector<Connection*>> internal_conns;
	std::unordered_map<Coupling*, std::vector<Connection*>> external_input_conns;
	std::unordered_map<Coupling*, std::vector<Connection*>> external_output_conns;

	void create_top(const py_str&);
	size_t find_index(Simulator*);

	void setup_coupled(Coupling*);
	void close_coupled(Coupling*);
	void close_top(RootCoupling*);

public:
	bool printStatus = false;
	Py2PDEVSModel(double, const py_str&);
	~Py2PDEVSModel();

	boost::shared_ptr<Coupling> create_coupled(const Coupling&, const py_str&);

	RootCoupling *get_top() const;

	void connect_models(Simulator*, size_t, Simulator*, size_t);
	void connect_input_to_model(Simulator*, size_t, size_t);
	void connect_model_to_output(Simulator*, size_t, size_t);
	void run();

	// Atomic models
	template<class T>
	boost::shared_ptr<T> create_atomic(
			const Coupling &parent,
			const py_str &name,
			const py_list &params)
	{
		Coupling *parent_ptr = const_cast<Coupling*>(&parent);

		T *model = new T(TO_C_STR(name));
		model->father = parent_ptr;
		std::vector<std::string> v_params = to_vector(params);
		model->init_from_vector(this->ti, v_params);

		this->children[parent_ptr].push_back(model);

		return boost::shared_ptr<T>(model, [](T*) {return;});
	}
};


#endif
