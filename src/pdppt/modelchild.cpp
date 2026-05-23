#include "modelchild.h"

modelParameter* modelChild::getParameter(QString paramName){
	return childType==COUPLED? coupled->getParameter(paramName) : atomic->getParameter(paramName);
}

QString modelChild::getName(){
	return childType==COUPLED? coupled->name : atomic->name;
}

int modelChild::getInPorts(){
	return childType==COUPLED? coupled->inPorts : atomic->inPorts;
}

int modelChild::getOutPorts(){
	return childType==COUPLED? coupled->outPorts : atomic->outPorts;
}

int modelChild::getIndex(){
	return childType==COUPLED? coupled->index : atomic->index;
}

QString modelChild::getPath(){
	return childType==COUPLED? coupled->path : atomic->path;
}

QList<modelParameter*> modelChild::getParams(){
	return childType==COUPLED? coupled->params : atomic->params;
}

modelCoupled* modelChild::getFather(){
	return childType==COUPLED? coupled->father : atomic->father;
}


