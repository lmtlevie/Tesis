#include "JobQueue.h"

void JobQueue::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar;

	fvar = va_arg(parameters, char*);
	this->maxCapacity = readDefaultParameterValue<int>(fvar);
	if (this->maxCapacity == -1) this->maxCapacity = std::numeric_limits<int>::max();
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: MaxCapacity: %i jobs \n", t, this->getName(), this->maxCapacity);

	this->sigma = std::numeric_limits<double>::max();
	this->discardedJobs = 0;

	for (int i = 0; i < 10; i++) {
		stateOutSignal[i] = 0;
	}

	this->logger->initSignals(std::vector<std::string>{"discards", "qsize"});

	this->mystate = WAITING;
	this->serverstate = SERVERBUSY;

	return;
}

void JobQueue::dint(double t) {

	switch (this->mystate) {
	case WAITING:
		this->serverstate = SERVERWAITING;
		this->mystate = t_NOTIFYING;
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Ready to notify new size \n", t, this->getName());
		this->sigma = 0;
		return;

	case t_NOTIFYING: // Notified length
		this->stateOutSignal[1] = 0; // clear discard flag notification

		if (this->serverstate == SERVERWAITING && this->queuedJobs.size() > 0){ // now emit
			this->mystate = t_READYTOEMIT;
			this->sigma = 0;
		} else { // not ready to send, so wait
			this->mystate = WAITING;
			this->sigma = std::numeric_limits<double>::max();
		}

		return;

	case t_READYTOEMIT:
		// Emitted a packet, now notify length
		if (this->serverstate == SERVERWAITING && this->queuedJobs.size() > 0) {
			this->queuedJobs.pop_front(); // unqueue
			this->serverstate = SERVERBUSY;
			this->mystate = t_NOTIFYING;
			this->sigma = 0;

			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: DEQUEUED SIZE: %u \n", t, this->getName(), this->queuedJobs.size());
		} else {
			// Nothing else to do, go back waiting
			this->mystate = WAITING;
			this->sigma = std::numeric_limits<double>::max();
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Go to wait again \n", t, this->getName());
		}

	};
	return;
}
void JobQueue::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: job arrived to port %u \n", t, this->getName(), x.port);

	if (x.port == 0) { // new job Arrived
		std::shared_ptr<Job> arrivedJob = castEventPointer<Job>(x); // get the job from the incomming event


		if (this->queuedJobs.size() >= this->maxCapacity) { // Queue Full: Rejects
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Incoming job #%i Refused. Job Discarded Not enough capacity for job (currentSize=%i, MaxCapacity=%i) \n", t, this->getName(), arrivedJob->getId(), this->queuedJobs.size(), this->maxCapacity);
 
			this->stateOutSignal[1] = 1; // send a discard on the notifications
			this->logger->logSignal(t, (double) ++this->discardedJobs, "discards");
		}else{ // Otherwise : Accepts
			this->queuedJobs.push_back(arrivedJob);

			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Queued job #%u (queue size: %i jobs)\n", t, this->getName(), arrivedJob->getId(), this->queuedJobs.size());
		}

		this->mystate = t_NOTIFYING;
		this->sigma = 0;
	}

	if (x.port == 1) { // Request from Server Arrived
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Requested by Server... \n", t, this->getName());

		if (!this->queuedJobs.empty()) { // Queue is not empty
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] Queue_ext: Non Empty Queue... \n", t);
			this->serverstate = SERVERWAITING;
			this->mystate = t_READYTOEMIT;
			this->sigma = 0; // Force dequeueing next available element at the head of the queue;
		} else { // Queue is empty
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Failed: Empty Queue... \n", t, this->getName());
			this->serverstate = SERVERWAITING;
			this->mystate = t_READYTOEMIT;
			this->sigma = this->sigma - this->e; // Continues waiting for a new packet
		}
	}

	return;
}
Event JobQueue::lambda(double t) {
	switch (this->mystate) {
	case WAITING: // Forced Period Elapsed
		// Sends Nothing
		return Event();
		break;
	case t_NOTIFYING: // Must notify new queue length
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Notifying QUEUE SIZE: %i jobs in queue\n", t, this->getName(), this->queuedJobs.size());
		this->logger->logSignal(t, (double) this->queuedJobs.size(), "qsize");
		this->stateOutSignal[0] = (double) this->queuedJobs.size();
		return Event(&this->stateOutSignal, 1);
		break;
	case t_READYTOEMIT:
		if (this->serverstate == SERVERWAITING && this->queuedJobs.size() > 0) {
			// Must emit a packet
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Sending packet... NOW QUEUE SIZE: %u bytes \n ", t, this->getName(), this->queuedJobs.size());

			auto jobToSend = this->queuedJobs.front();

			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Dequeueing job #%u (queue size: %i packets)\n", t, this->getName(), jobToSend->getId(), this->queuedJobs.size());
			return Event(jobToSend, 0);
		} else {
			return Event();
		}

		break;
	}

	return Event();
}


