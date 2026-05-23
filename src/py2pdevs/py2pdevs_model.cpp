#include <iostream>
#include <sstream>
#include <stdexcept>

#include "utils/options.h"

#include "py2pdevs_model.h"
#include "utils.h"


Py2PDEVSModel::Py2PDEVSModel(double ti, const py_str &name)
{
	char *name_str = TO_C_STR(name);
	this->simulator = new RootSimulator(name_str, ".");
	this->top = nullptr;
	this->ti = ti;

	this->create_top(name);
}

Py2PDEVSModel::~Py2PDEVSModel()
{
	delete this->simulator;
	delete this->top;
}

RootCoupling *Py2PDEVSModel::get_top() const
{
	return this->top;
}

void Py2PDEVSModel::create_top(const py_str &name_str)
{
	this->top = new RootCoupling(TO_C_STR(name_str));
	this->top->rootSim = this->simulator;
	this->simulator->mainCoupling = this->top;

	this->children[this->top] = std::vector<Simulator*>();
	this->internal_conns[this->top] = std::vector<Connection*>();
	this->external_input_conns[this->top] = std::vector<Connection*>();
	this->external_output_conns[this->top] = std::vector<Connection*>();
}

boost::shared_ptr<Coupling> Py2PDEVSModel::create_coupled(
		const Coupling &parent,
		const py_str &name_str)
{
	Coupling *coupled = new Coupling(TO_C_STR(name_str));

	coupled->father = const_cast<Coupling*>(&parent);
	this->children[coupled->father].push_back(coupled);

	// TODO: refactor
	this->children[coupled] = std::vector<Simulator*>();
	this->internal_conns[coupled] = std::vector<Connection*>();
	this->external_input_conns[coupled] = std::vector<Connection*>();
	this->external_output_conns[coupled] = std::vector<Connection*>();

	return boost::shared_ptr<Coupling>(coupled, [](Coupling*) {return;});
}

void Py2PDEVSModel::connect_models(
		Simulator *model1, size_t port1,
		Simulator *model2, size_t port2)
{
	if(model1->father != model2->father)
	{
		std::stringstream ss;
		ss
			<< "Wrong connection! Models "
			<< model1->getName() << " and "
			<< model2->getName() << " have different parents!";
		throw std::runtime_error(ss.str());
	}

	size_t index1 = this->find_index(model1);
	size_t index2 = this->find_index(model2);

	Connection *connection = new Connection();
	connection->setup(index1, port1, index2, port2);

	this->internal_conns[model1->father].push_back(connection);
}

void Py2PDEVSModel::connect_input_to_model(
		Simulator *model, size_t port,
		size_t inport)
{
	size_t index = this->find_index(model);

	Connection *connection = new Connection();
	connection->setup(0, inport, index, port);

	this->external_input_conns[model->father].push_back(connection);
}

void Py2PDEVSModel::connect_model_to_output(
		Simulator *model, size_t port,
		size_t outport)
{
	size_t index = this->find_index(model);

	Connection *connection = new Connection();
	connection->setup(index, port, 0, outport);

	this->external_output_conns[model->father].push_back(connection);
}

void Py2PDEVSModel::run()
{
	std::cout << "Creating python <-> PowerDEVS model ..." << std::endl;
	this->close_top(this->top);

	double max_steps = OptionManager::get<double>("max_steps");
	double tf = OptionManager::get<double>("tf");
	std::string finalization_script = OptionManager::get("finalization_script");

	this->simulator->setInitialTime(this->ti);
	this->simulator->setBreakCount(max_steps);
	this->simulator->init();
	this->simulator->setFinalTime(tf);

	std::cout << "Model Created, start running ..." << std::endl;
	this->simulator->run();
	std::cout << "Model completed running with code: " << this->simulator->getExitStatus() << std::endl;

	// Assuming finalization in py2pdevs is done directly inside the main
	// script.
	if(!finalization_script.empty())
		this->simulator->finalize(finalization_script);

//	int status = this->simulator->getExitStatus();
//	std::stringstream msg;
//	msg << "Simulation failed! (exit status: " << status << ")";
//	if(status != 0)
//		throw std::runtime_error(msg.str());
}

void Py2PDEVSModel::setup_coupled(Coupling *coupled)
{
	size_t n_children = this->children[coupled].size();
	size_t n_internal_conns = this->internal_conns[coupled].size();
	size_t n_external_input_conns = this->external_input_conns[coupled].size();
	size_t n_external_output_conns = this->external_output_conns[coupled].size();

	// Recursively close children first (otherwise PDEVS is not properly
	// initialized).
	for(size_t i = 0; i < n_children; ++i)
	{
		Simulator *model = this->children[coupled][i];
		if(model->isCoupled())
			this->close_coupled(dynamic_cast<Coupling*>(model));
	}

	Simulator **children = nullptr;
	if(n_children > 0)
		children = new Simulator*[n_children];
	for(size_t i = 0; i < n_children; ++i)
		children[i] = this->children[coupled][i];

	Connection **internal_conns = nullptr;
	if(n_internal_conns > 0)
		internal_conns = new Connection*[n_internal_conns];
	for(size_t i = 0; i < n_internal_conns; ++i)
		internal_conns[i] = this->internal_conns[coupled][i];

	Connection **external_input_conns = nullptr;
	if(n_external_input_conns > 0)
		external_input_conns = new Connection*[n_external_input_conns];
	for(size_t i = 0; i < n_external_input_conns; ++i)
		external_input_conns[i] = this->external_input_conns[coupled][i];

	Connection **external_output_conns = nullptr;
	if(n_external_output_conns > 0)
		external_output_conns = new Connection*[n_external_output_conns];
	for(size_t i = 0; i < n_external_output_conns; ++i)
		external_output_conns[i] = this->external_output_conns[coupled][i];

	coupled->setup(
			children, n_children,
			internal_conns, n_internal_conns,
			external_input_conns, n_external_input_conns,
			external_output_conns, n_external_output_conns);
}

void Py2PDEVSModel::close_coupled(Coupling *coupled)
{
	this->setup_coupled(coupled);
	coupled->init(this->ti);
}

void Py2PDEVSModel::close_top(RootCoupling *coupled)
{
	this->setup_coupled(coupled);
}

size_t Py2PDEVSModel::find_index(Simulator *model)
{
	Coupling *parent = model->father;
	size_t index = 0;

	for(auto child : this->children[parent])
	{
		if(child == model)
			break;
		index++;
	}

	return index;
}
