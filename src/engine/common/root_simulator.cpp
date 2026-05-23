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


#include "root_simulator.h"
#include "pdevslib.h"
#include "pdevs2py.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

Time realTi;
bool overRun;
int exitStatus=0;
double realTiSimulation;

bool abort_simulation; // EP: used to abort simulation from atomic

double RootSimulator::FinalTime = -1; // this is the global tf used by the pdevslib.h::get/setFinalTime methods

void RootSimulator::init(){
	t=ti;
	printLog("Simulation Initialized\n");
	mainCoupling->init(t);
	mainCoupling->imessage(0,0,t);
	mainCoupling->ta(t);
	realTi = getTime();
	lastTransition=0;
	stepsCount=0;
  	initLib();
#ifdef RTAIOS
	enterRealTime();
#endif
	abort_simulation = 0; // EP: used to abort simulation from atomic
	absoluteMaxSteps=0; // EP: used to register max number of events in a single time instant
}

bool RootSimulator::step(){
	try{
		t=mainCoupling->tn;
		if (abort_simulation) { // EP
			exit(t);
			return true;
		}
		if (lastTransition==t)
		{
			stepsCount++;
		} else
		{
			if (stepsCount>absoluteMaxSteps) // EP
				absoluteMaxSteps = stepsCount; //EP
			lastTransition=t;
			stepsCount=0;
		}
		if (stepsCount>maxSteps) {
			printf("Illegitimate model (after %d steps). Aborting simulation at time %g\n", stepsCount, t);
			fflush(stdout);
			exit(t);
			return true;
		}
		if (t<=tf) {
 			Event e = mainCoupling->lambdamessage(t);
			if (!e.isInterrupted()) {
				mainCoupling->dintmessage(t);
			}
		    return false;
		}
		else {
			exit(tf);
			return true;
		}
	} catch (std::runtime_error& e) {
		printf("Exception thrown during simulation (at T=%g). Attempting to exit all models \n", t);
		exit(t);
		printf("Models successfully exited. Rethrowing original exception \n");
		throw e;
		return true;
	} catch(...){ // attempt to catch additional exceptions (although not-c++ exceptions like division-by-zero are not caught)
		printf("Unknown exception thrown \n");
		std::exception_ptr eptr = std::current_exception(); // capture
		if (eptr) {
			std::rethrow_exception(eptr);
		}
		return true;
	}
}

void RootSimulator::exit(double t)
{
#ifdef RTAIOS
	leaveRealTime();
#endif
	mainCoupling->exit(t);
	// printLog("Maximum number of events in same instant: %i \n",absoluteMaxSteps); // EP
	printf("Maximum number of events in same instant: %i \n",absoluteMaxSteps); // EP
	// fflush(stdout); // EP
	printLog("Simulation Ended (%.3g sec)\n",getTime()-realTi);
	cleanLib();
}

bool RootSimulator::step(int s) {
	int i;
	Time temp=getRealSimulationTime();
	
	for (i=0;i<s;i++) {
	  if (getRealSimulationTime()-temp>0.5) 
		  return false;
		if (timed) {
			waitFor(t-getRealSimulationTime(),REALTIME);
		}
		if (step()) 
			return true;

	}
	return false;
}

void RootSimulator::run() {
	while (!step(1)) ;
}

// Run finalization tasks.
// The default behavior is running a Python script named like the model.
// The script might be overriden by command-line arguments.
void RootSimulator::finalize(const std::string &finalization_script)
{
	std::string script, module, path;

	if(!finalization_script.empty())
	{
		script = finalization_script;

		std::ifstream stream(script);
		if(!stream.good())
		{
			std::cerr << "Warning: cannot open finalization script!" << std::endl;
			return;
		}

		auto index = script.find_last_of("/");

		if(index == std::string::npos)
			path = ".";
		else
			path = script.substr(0, index);

		module = script.substr(index+1, script.size() - index - 4);
	}
	else
	{
		path = std::string(this->path);
		module = std::string(this->name);
		script = path + "/" + module + ".py";

		// Backwards compatibility: do nothing in case default script does not
		// exist.
		std::ifstream stream(script);
		if(!stream.good())
			return;
	}

	run_python_module(path, module);
}

void RootSimulator::changemytn(Time tnext){
	DEBUG(TA,(Simulator*)this,"Changing tn at RootSimulator %g\n",tnext);
	t=tnext;
	mainCoupling->ta(t);
};
