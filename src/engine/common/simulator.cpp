/****************************************************************************
 **
 **  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
 **		       Universidad Nacional de Rosario - Argentina.
 **  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
 **
 **  This file is part of PowerDEVS.
 **
 **  PowerDEVS is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  PowerDEVS is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/

#include <iostream>

#include "simulator.h"
#include "coupling.h"
#include "engine.h"
#include "pdevslib.h"
#include "root_simulator.h"

Simulator::Simulator(const char *name)
{
	Simulator::name = name;

}

Simulator::Simulator()
{

}
Simulator::~Simulator()
{

}

void Simulator::init_from_vector(Time t, const std::vector<std::string> &params)
{
	// TODO: this is a temporal implementation.
	// Should be fixed after updating every atomic model in order
	// to use vectors instead of va_lists to handle parameters.

	switch(params.size())
	{
	case 0:
		this->init(t);
		break;
	case 1:
		this->init(t, params[0].c_str());
		break;
	case 2:
		this->init(t, params[0].c_str(), params[1].c_str());
		break;
	case 3:
		this->init(t, params[0].c_str(), params[1].c_str(), params[2].c_str());
		break;
	case 4:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str());
		break;
	case 5:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str());
		break;
	case 6:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str());
		break;
	case 7:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str());
		break;
	case 8:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str());
		break;
	case 9:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str());
		break;
	case 10:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str());
		break;
	case 11:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str());
		break;
	case 12:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str());
		break;
	case 13:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str());
		break;
	case 14:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str());
		break;
	case 15:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str());
		break;
	case 16:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str());
		break;
	case 17:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str(), params[16].c_str());
		break;
	case 18:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str(), params[16].c_str(), params[17].c_str());
		break;
	case 19:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str(), params[16].c_str(), params[17].c_str(),
				params[18].c_str());
		break;
	case 20:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str(), params[16].c_str(), params[17].c_str(),
				params[18].c_str(), params[19].c_str());
		break;
	case 201:
		this->init(
				t, params[0].c_str(), params[1].c_str(), params[2].c_str(),
				params[3].c_str(), params[4].c_str(), params[5].c_str(),
				params[6].c_str(), params[7].c_str(), params[8].c_str(),
				params[9].c_str(), params[10].c_str(), params[11].c_str(),
				params[12].c_str(), params[13].c_str(), params[14].c_str(),
				params[15].c_str(), params[16].c_str(), params[17].c_str(),
				params[18].c_str(), params[19].c_str(), params[20].c_str());
		break;

	default:
		std::cerr << "Simulator::init_from_vector: Exceeded the maximum number of parameters to an atomic model!\n";
		abort();
	}
}

void Simulator::imessage(Coupling *f, int my, Time t){
	myself=my;
	father=f;
	e=0;
	tl=t;
	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: at imessage, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] " + this->getFullName() + ": after dint, ta returned a not >=0 value. "+std::to_string(timeAdvance));
	}
	DEBUG(INIT,this,"[%s] Execution Init Function at %g\n",name,t);
}


Event Simulator::lambdamessage(Time t){
	Event out=lambda(t);
	DEBUG(LAMBDA,this,"[%s] Execution Output Function at %g\n",name,t);
	int ret = 0;
	if (out.getRealTimeMode()) {
		ret = waitFor(t-getRealSimulationTime(),out.getRealTimeMode());	 
	}

	if (ret == -1) { // In IRQ has happend
		DEBUG(LAMBDA,this,"[%s] IRQ Detected %g\n",name,t);
		out.setNullEvent();
		out.setInterrupted();
	}
	return out;
}

void Simulator::dintmessage(Time t){
	DEBUG(DINT,this,"[%s] Execution Internal Transition at %g\n",name,t);
	e=t-tl;
	dint(t);
	tl=t;

	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: after dint, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] "+ this->getFullName() + ": after dint, ta returned a not >=0 value." +std::to_string(timeAdvance));
	}
}

void Simulator::dextmessage(Event x, Time t){
	DEBUG(DEXT,this,"[%s] Execution External Transition at %g\n",name,t);
	e=t-tl;
	dext(x, t);
	tl=t;

	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: after dext, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] "+ this->getFullName() + ": after dext, ta returned a not >=0 value. ");
	}
}


void Simulator::externalinput(Time t, int irq){
	DEBUG(DEXT,this,"[%s] Execution External Input at %g\n",name,t);
	Event x;
	x.port = -1;
	x.value = (void*)irq;
	e = t - tl;
	dext(x,t);
	tl=t;
	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: after dext from external input, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] "+ this->getFullName() + ": after dint, ta returned a not >=0 value. ");
	}

	father->changemytn(tn,myself);
	DEBUG(DEXT,this,"[%s] Next event is at %g\n",name,tn);
};

//---------------------- agregado 24-01-2018 -----------------------
void Simulator::externalinput(Time t, void* data){
	Event x;
	x.port = -1;
	x.value = data;
	e = t - tl;
	dext(x,t);
	tl=t;
	// tn=t+ta(t);
	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: after dext from external input, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] "+ this->getFullName() + ": after dint, ta returned a not >=0 value. ");
	}
	father->changemytn(tn,myself);
};
void Simulator::externalinput(Time t, void* data, char* typeMsg){
        printLog("[externalinput]\n");
	Event x;
	int longData = strlen((char*) data);
	int longTypeMsg = strlen(typeMsg);
	int size = longData + longTypeMsg+2;
	//int size=100;
	char *msg = new char[size]; //TODO liberarla
	strcat(msg,typeMsg);
	strcat(msg,"=");
	strcat(msg,(char*) data);
	strcat(msg, "\0");
	x.port = -1;
	x.value = data; // msg;
	e = t - tl;
	dext(x,t);
	tl = t;
	// tn = t + ta(t);
	double timeAdvance = ta(t);
	if(timeAdvance >= 0){ // verify ta before proceeding (to save debugging time). NOTE: this might bring problems because sigma-e is sometimes negative (ej: -5.551e-17)
		tn=t+timeAdvance;
	} else { // invalid ta
		printLog("[%g] %s: after dext from external input, ta returned a not >=0 value . Returned value: %g \n", t, this->getFullName().data(), timeAdvance);
		//		throw std::runtime_error("["+ std::to_string(t) + "] "+ this->getFullName() + ": after dint, ta returned a not >=0 value. ");
	}
	father->changemytn(tn,myself);
};
//-------------------------- fin agregado --------------------------

RootCoupling *Simulator::get_top() const
{
	Coupling *parent;
	for(parent = this->father;
			parent != nullptr && parent->father != nullptr;
			parent = parent->father);

	return dynamic_cast<RootCoupling*>(parent);
}

/*
 * Return the full hierarchy name: coupleModelName.atomicModelName
 */
std::string Simulator::getFullName() {
	if(this->fullName == ""){
		this->fullName = std::string(this->getName()); // my name
		for (Coupling* papa = this->father; papa != NULL && papa->father != NULL; papa = papa->father){
			fullName = std::string(papa->getName()) + std::string(".") + fullName;
		}
	}

	return this->fullName;
}

void Simulator::exit(double t){
	this->exit(); // to keep backward compatibility
};

void Simulator::exit(){

};



