#include "ScalarAssignment2ProcessorVector.h"

void ScalarAssignment2ProcessorVector::init(double t,...) {
	 BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma = std::numeric_limits<double>::infinity();  // wait for ever;
}

void ScalarAssignment2ProcessorVector::dint(double t) {
	sigma = std::numeric_limits<double>::infinity();
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ScalarAssignment2ProcessorVector::dext(Event x, double t) {
	// Request arrived
	auto arrivedAssignment = castEventPointer<JobAssignment>(x);

	// Set the value and index
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedAssignment->getJob();
	vec->index = arrivedAssignment->getProcessorId();

	outputVectorialEvents.push(vec);

//	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u from ROS=%i directed to DCM=%i . Sending it to TcpSender=%i \n", t, this->getName(), arrivedAssignment->ID, rosId, dcmMachineId, vec->index);

	sigma=0; // Program transition right now
}

Event ScalarAssignment2ProcessorVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	//debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}


