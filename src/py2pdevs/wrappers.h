#ifndef __PY2PDEVS_WRAPPERS_H__
#define __PY2PDEVS_WRAPPERS_H__

#include <iostream>

#include <boost/python.hpp>

#include "engine.h"
#include "py_types.h"

#include "general/BaseSimulator.h"
#include "vector/VectorialModel.h"
#include "network/packetRED_AQM.h"
#include "network/packetRED_AQM_ECN.h"
#include "network/packetqueue.h"
#include "network/packetqueue_simple.h"


using namespace boost::python;


class SimulatorWrapper : public Simulator, public wrapper<Simulator>
{

	void dint(Time t)
    {
        this->get_override("dint")(t);
    }

	void dext(Event e, Time t)
    {
        this->get_override("dext")(e, t);
    }

	Event lambda(Time t)
    {
        return this->get_override("lambda")(t);
    }

	double ta(Time t)
    {
        return this->get_override("ta")(t);
    }

public:
	void init_from_list_2(Time t, const py_list &params)
    {
        if(override f = this->get_override("init_from_list"))
            f(t, params);
        this->init_from_list(t, params);
    }

    void default_init(Time t, const py_list &params)
    {
    	this->init_from_list(t, params);
    }

	virtual void init_from_list(Time t, const py_list &params)
	{
		std::cout << t << std::endl;
	}

	std::string get_name() const
	{
		return std::string(Simulator::name);
	}
};

class BaseSimulatorWrapper : public BaseSimulator, public wrapper<BaseSimulator>
{
	void dint(Time t)
    {
        this->get_override("dint")(t);
    }

	void dext(Event e, Time t)
    {
        this->get_override("dext")(e, t);
    }

	Event lambda(Time t)
    {
        return this->get_override("lambda")(t);
    }

public:
	BaseSimulatorWrapper(const char *name) : BaseSimulator(name) {};
};

class CouplingWrapper : public Coupling, public wrapper<Coupling>
{
	void dint(Time t)
    {
        this->get_override("dint")(t);
    }

	void dext(Event e, Time t)
    {
        this->get_override("dext")(e, t);
    }

	Event lambda(Time t)
    {
        return this->get_override("lambda")(t);
    }

public:
	CouplingWrapper(const char *name) : Coupling(name) {};
};

class VectorialModel_packetRED_AQM_ECN_Wrapper :
		public VectorialModel<packetRED_AQM_ECN>,
		public wrapper<VectorialModel<packetRED_AQM_ECN>>
{
public:
	VectorialModel_packetRED_AQM_ECN_Wrapper(const char *name) :
		VectorialModel<packetRED_AQM_ECN>(name) {};

	int getParameterCount()
    {
        return this->get_override("getParameterCount")();
    }
};

class VectorialModel_packetRED_AQM_Wrapper :
		public VectorialModel<packetRED_AQM>,
		public wrapper<VectorialModel<packetRED_AQM>>
{
public:
	VectorialModel_packetRED_AQM_Wrapper(const char *name) :
		VectorialModel<packetRED_AQM>(name) {};

	int getParameterCount()
    {
        return this->get_override("getParameterCount")();
    }
};

class VectorialModel_packetqueue_Wrapper :
		public VectorialModel<packetqueue>,
		public wrapper<VectorialModel<packetqueue>>
{
public:
	VectorialModel_packetqueue_Wrapper(const char *name) :
		VectorialModel<packetqueue>(name) {};

	int getParameterCount()
    {
        return this->get_override("getParameterCount")();
    }
};

class VectorialModel_packetqueue_simple_Wrapper :
		public VectorialModel<packetqueue_simple>,
		public wrapper<VectorialModel<packetqueue_simple>>
{
public:
	VectorialModel_packetqueue_simple_Wrapper(const char *name) :
		VectorialModel<packetqueue_simple>(name) {};

	int getParameterCount()
    {
        return this->get_override("getParameterCount")();
    }
};

#endif
