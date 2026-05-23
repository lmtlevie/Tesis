/***************************************************************************
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

#include "PythonCodeGenerator.h"

QString PythonCodeGenerator::getAdditionalMethods(modelCoupled* coupledModel, int tabs){
	QStringList additionalMethods;

	//child models
	//	if (coupledModel->childs.size() >0)
	//		additionalMethods << QString("%1#### child classes of %2  ####\n").arg(TAB(tabs)).arg(coupledModel->name);

	int index=-1; // to avoid repeating variable names
	for(modelChild* child : coupledModel->childs){
		index++;
		if (child->childType == ATOMIC) continue;

		child->coupled->index = index;
		additionalMethods << generateTopModel(child, tabs);
	}

	//	if (coupledModel->childs.size() >0)
	//		additionalMethods << QString("%1#### END child classes of %2  ####\n").arg(TAB(tabs)).arg(coupledModel->name);

	return  additionalMethods.join("");
}

QString PythonCodeGenerator::getCreateCode(modelCoupled* coupledModel, int tabs){
	QStringList constructor;

	// create a coupled to contain all
	QString modelVar = "self._pd_model";
	constructor << QString("%1%2 = self.interface.create_coupled(self.parent.pd_model(), self.name)\n").arg(TAB(tabs))
													.arg(modelVar);
	constructor << QString("%1\n").arg(TAB(tabs));

	//child models
	constructor << QString("%1# Child models\n").arg(TAB(tabs));
	int index=-1; // to avoid repeating variable names
	for(modelChild* child : coupledModel->childs){
		index++;
		if (child->childType == ATOMIC) {	// (thanks polymorphism :-()
			child->atomic->index = index;
			constructor << getCreateCode(child->atomic, modelVar, tabs);
		} else { //coupled, add them as class methods so that it is easy to override in python (tabs=2)
			child->coupled->index = index;
			QString methodName = getFullName(child->coupled);
			constructor << QString("%1__%2 = %3(self, \"%4\"%5)._create_model() # create an instance of '%2' coupled model \n")
															.arg(TAB(tabs))
															.arg(toVariableName(child->coupled))
															.arg(methodName)
															.arg(child->coupled->name)
															.arg(getInheritedParameterValues(coupledModel, child->coupled));
			constructor << QString("%1self.submodels[\"__%2\"] = __%2\n")
								.arg(TAB(tabs))
								.arg(toVariableName(child->coupled));
		}
	}
	constructor << QString("%1\n").arg(TAB(tabs));

	// connections
	//	printf("%s has %d IC connections \n", QSTR(coupledModel->name), coupledModel->lsIC.size());
	//	printf("%s has %d EIC connections \n", QSTR(coupledModel->name), coupledModel->lsEIC.size());
	//	printf("%s has %d EOC connections \n", QSTR(coupledModel->name), coupledModel->lsEOC.size());

	constructor << QString("%1# Connections\n").arg(TAB(tabs));
	for (modelConnection* connection : coupledModel->lsIC){ // intermodel connections
		QString sourceName = this->getChildName(coupledModel, connection->childSource);
		sourceName = "__" + toVariableName(sourceName, connection->childSource);
		QString sinkName = this->getChildName(coupledModel, connection->childSink);
		sinkName = "__" +  toVariableName(sinkName, connection->childSink);
		constructor << QString("%1self.interface.connect_models(%2, %3, %4, %5)\n").arg(TAB(tabs))
																		.arg(sourceName)
																		.arg(connection->sourcePort)
																		.arg(sinkName)
																		.arg(connection->sinkPort);
	}

	for (auto connection : coupledModel->lsEIC) { // input ports to inner model connections
		QString sinkName = this->getChildName(coupledModel, connection->childSink);
		sinkName = "__" + toVariableName(sinkName, connection->childSink);
		constructor << QString
				("%1self.interface.connect_input_to_model(%2, %3, %4)\n").arg(TAB(tabs))
				.arg(sinkName).arg(connection->sinkPort)
				.arg(connection->sourcePort);
	}

	for (auto connection : coupledModel->lsEOC){
		QString sourceName = this->getChildName(coupledModel, connection->childSource);
		sourceName = "__" + toVariableName(sourceName, connection->childSource);
		constructor << QString(
				"%1self.interface.connect_model_to_output(%2, %3, %4) \n").arg(TAB(tabs))
																				.arg(sourceName).arg(connection->sourcePort)
																				.arg(connection->sinkPort);
	}

	constructor << QString("\n");
	constructor << QString("%1return %2\n").arg(TAB(tabs)).arg(modelVar);
	//	constructor << QString("\n");

	return constructor.join("");
}

QString PythonCodeGenerator::getInheritedParameterValues(modelCoupled* parent, modelCoupled* child){
	QStringList parameters;

	for (auto param : child->params){
		if((param->type == STR || param->type == SCILAB)
				&& param->strValue.startsWith("%")){

			// search for parameter in parent
			bool found = false;
			for(auto parentParam : parent->params){
				if(parentParam->name == param->strValue.mid(1)){
					parameters << QString("%1 = self.%2")
																								.arg(toVariableName(param->name, -1))
																								.arg(toVariableName(parentParam->name, -1));
					found = true;
					break;
				}
			}

			if (!found){
				QString msg = QString("paramenter '%1' in model '%2' is not specified by the parent model '%3'. Either don't use the '%' syntax or add a parameter with name '%1' in coupled model '%3'   \n")
																.arg(param->strValue)
																.arg(child->name)
																.arg(getFullName(parent));
				throw std::runtime_error(QSTR(msg));
			}
		}
	}

	if (parameters.size() ==0)
		return "";

	return ", " + parameters.join(", "); // first " ," is because this is used after other parameters
}

QString PythonCodeGenerator::getCreateCode(modelAtomic* atomicModel, QString parent, int tabs){
	QStringList constructor;
	QString varName;
	QString modelName;
	QString closeMethod;
	if(parent.isEmpty()){ // atomic model is not inside a coupled model
		parent = "self.parent.pd_model()";
		varName = "self._pd_model";
		modelName = "self.name";
		closeMethod = QString("\n%1return self._pd_model\n")
				.arg(TAB(tabs));
	} else {
		varName = "__" + toVariableName(atomicModel);
		modelName =  QString("\"%1\"").arg(atomicModel->name);
	}
	constructor << QString("%1%2 = self.interface.create_%3(%4, %5%6)\n")
							.arg(TAB(tabs))
							.arg(varName)
							.arg(getClassName(atomicModel->path).toLower())
							.arg(parent)
							.arg(modelName)
							.arg(resolveAtomicParams(atomicModel, parent));

	constructor << QString("%1self.submodels[\"%2\"] = %2\n")
							.arg(TAB(tabs))
							.arg(varName);
	constructor << closeMethod;


	return constructor.join("");
}

QString PythonCodeGenerator::toVariableName(QString name, int index=-1){
	// remove bad characters for python variable names
	QString varName = name.simplified().replace(" ", "");
	varName = varName.replace(".", "_"); // dots are not allows, replace them by _ instead of removing them
	varName.remove(QRegExp("[^a-zA-Z0-9_]*"));

	if(varName.isEmpty()){
		QString msg = QString("'%1' could not be transformed into a proper python variable name. Please choose a more friendly name. \n").arg(name);
		throw std::runtime_error(QSTR(msg));
		exit(-1);
	}

	// ~ set a good name ~
	if(std::isdigit(varName[0].toAscii())){
		varName = "_" + varName; 	// add a leading '_' to avoid starting with a number
	}

	QStringList reservedKeywords;
	reservedKeywords << "False" <<"None" << "True" << "and" << "as" << "assert" << "break" << "class" << "continue" << "def" << "del" << "elif" << "else" << "except" << "finally" << "for" << "from" << "global" << "if" << "import" << "in" << "is" << "lambda" << "nonlocal" << "not" << "or" << "pass" << "raise" << "return" << "try" << "while" << "with" << "yield";
	if(reservedKeywords.indexOf(varName) != -1){
		varName = "_" + varName; 	// add a leading '_' to avoid using reserved keywords
	}


	if(index == -1)
		return QString("%1").arg(varName);
	else
		return QString("%1_%2").arg(varName).arg(index); // Use index to avoid repeating variable names
}

QString PythonCodeGenerator::toVariableName(modelCoupled* coupled){
	QString modelName = coupled->name;
	int index = coupled->index;
	return toVariableName(modelName, index);
}

QString PythonCodeGenerator::toVariableName(modelAtomic* atomic){
	QString modelName = atomic->name;
	int index = atomic->index;
	return  toVariableName(modelName, index);
}

QString PythonCodeGenerator::toVariableName(modelChild* child){
	if (child->childType == ATOMIC) {	// (thanks polymorphism :-()
		return toVariableName(child->atomic);
	} else { //coupled
		return toVariableName(child->coupled);
	}
}

QStringList PythonCodeGenerator::generateTopModel(modelChild* model, int tabs){
	// retrieve values according to model type (thanks polymorphism :-()
	QString className;
	QString functionBody ;
	QString additionalMethods ;
	QList<modelParameter*>params;
	if (model->childType == ATOMIC) {	// Atomic
		className = toVariableName(model->atomic);
		params = model->atomic->params;
		functionBody = getCreateCode(model->atomic, ""); // TODO: this can break if there is a child model called 'parent'
		additionalMethods = "";
	} else { //coupled
		className = toVariableName(model->coupled);
		params = model->coupled->params;
		functionBody = getCreateCode(model->coupled, tabs+4); // TODO: this can break if there is a child model called 'parent'
		additionalMethods = getAdditionalMethods(model->coupled, tabs+2);
	}

	// class body according to model
	QStringList constructor;
	constructor << QString("\n");
	constructor << QString("%1class %2(BaseModel):\n").arg(TAB(tabs)).arg(className); // class

	// __init__ method
	if(params.size() == 0){
		constructor << QString("%1def __init__(self, parent, name):\n").arg(TAB(tabs+2));
	} else {
		constructor << QString("%1def __init__(self, parent, name, %2):\n").arg(TAB(tabs+2))
														.arg(getParametersForFunctionArgument(model));
	}
	constructor << QString("%1%1BaseModel.__init__(self, parent, name, [%2])\n").arg(TAB(tabs+2))
													.arg(getParameterNames(params));
	for (auto param : params){
		constructor << QString("%1%1self.%2 = %2\n").arg(TAB(tabs+2)).arg(toVariableName(param->name));
	}
	constructor << QString("\n");

	// _do_create_model method
	constructor << QString("%1def _do_create_model(self):\n").arg(TAB(tabs+2));
	constructor << QString("%1\n").arg(functionBody);

	// additional methods (submodels are subclasses)
	constructor << additionalMethods;

	return constructor;
}

void PythonCodeGenerator::generatePythonLibrary(){
	QStringList constructor;
	constructor << QString("########################################\n");
	constructor << QString("## This file was automatically generated by PowerDEVS pdppt tool. \n");
	constructor << QString("## Changes to this file will get overwritten in next compilation. To modify models in this library, extend the classes defined here and override the necessary methods.\n");
	constructor << QString("########################################\n \n");
	constructor << QString("from py2pdevs.network import log_vars \n");
	constructor << QString("from collections import OrderedDict \n");
	constructor << QString(" \n");
	constructor << QString("from .base_model import BaseModel \n");
	constructor << QString("\n");

	// process each model in the library
	for (auto child : this->libraryModel->childs){
		constructor << this->generateTopModel(child);
	}

	// write model to file
	QString path = QCoreApplication::applicationDirPath();
	path = path + QString("/../build/lib/py2pdevs/library/"+this->outputFileName);
	QFile *fd = new QFile(path );
	fd->open(QIODevice::WriteOnly | QIODevice::Text);
	printf("Writing python code in file: %s \n", path.toUtf8().constData() );

	for (auto i = constructor.begin(); i != constructor.end(); i++) {
		fd->write(QSTR(*i), (*i).size());
	}

	//close file
	fd->close();
	delete fd;

	// write to python module to include the new file (useful for testing the python syntax of all libraries)
//	QString init_path = QCoreApplication::applicationDirPath();
//	init_path = init_path + QString("/../build/lib/py2pdevs/library/__init__.py");
//	QFile *fd_init = new QFile(init_path);
//	fd_init->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
//	//printf("Writing __init__.py code in file: %s \n", QSTR(init_path));
//
//	QString init_include = QString("from %1 import *\n").arg(QFileInfo( this->outputFileName).baseName()); //.toLower()
//	fd_init->write(QSTR(init_include), init_include.size());
//	fd_init->close();
//	delete fd_init;
}

QString PythonCodeGenerator::getChildName(modelCoupled* coupledModel, int childIndex){
	QString name;
	auto source= coupledModel->childs[childIndex];

	if (source->childType == ATOMIC) {	// (thanks polymorphism :-()
		name = source->atomic->name;
	} else { //coupled
		name = source->coupled->name;
	}

	return name;
}


QString PythonCodeGenerator::getParameterNames (QList<modelParameter*> params, QString prefix){
	QStringList strParams;
	for(auto param : params){
		strParams << QString("%1%2").arg(prefix).arg(toVariableName(param->name));
	}

	return strParams.join(", ");
}

QString PythonCodeGenerator::getParameterValues (QList<modelParameter*> params){
	QStringList strParams;
	for(auto param : params){
		strParams << QString("\"%1\"").arg(param->strValue);
	}

	return strParams.join(", ");
}

QString PythonCodeGenerator::getParametersForFunctionArgument(modelChild* child){
	QStringList strParams;
	for(auto param : child->getParams()){
			strParams << QString("%1 = %2")
																.arg(toVariableName(param->name))
																.arg(this->getParameterString(param, child, false));

	}

	return strParams.join(", ");
}

QString PythonCodeGenerator::resolveAtomicParams(modelAtomic* atomicModel, QString parent){
	QString params;

	if(QString::compare(parent, "self.parent.pd_model()", Qt::CaseSensitive) == 0){ // this is a 1st level model (parent=top library)
		params = QString(", [%1]").arg(getParameterNames(atomicModel->params, "self."));
	} else { // the atomic model is inside some coupled model: parameters need resolving
		// logic taken from codegenerator::540 used for c++ generation
		if (atomicModel->paramsString.trimmed() != "") {
			params = QString(atomicModel->paramsString );
		} else if (atomicModel->params.size() != 0) {
			QStringList builder;
			for (auto param : atomicModel->params) {
				auto tmp = new modelChild();
				tmp->childType = ATOMIC; tmp->atomic = atomicModel;
				builder << this->getParameterString(param, tmp, true);
			}
			params = builder.join(", ");
		} else {
			params = QString(""); // no params
		}

		params = QString(", [") + params + QString("]"); // paramsString resolves parameters inherited from coupled models (the ones that start with '%')
	}

	return params;
}

/*
 * Extracted from codegenerator.cpp:366
 * and modified to use parent model parameter values as passed in arguments instead of the one defined in GUI
 */
QString PythonCodeGenerator::getParameterString(modelParameter* param, modelChild* model, bool usePythonFather){
	//printf("Evaluating parameter %s of atomic %s\n", QSTR(param->strValue),QSTR(model->name));
	QString ret;
	switch (param->type) {
	case STR:
	case SCILAB:
		if (param->strValue == "%Inputs") {
			ret += QString("\"%1\"").arg((double)model->getInPorts(), 0, 'e');
		} else if (param->strValue == "%Outputs") {
			ret += QString("\"%1\"").arg((double)model->getOutPorts(), 0, 'e');
		} else if (param->strValue.startsWith("%")) {
			if (model->getFather() == NULL){
				QString msg = QString("paramenter '%1' in model '%2'. Can not use the '%' syntax in top models'   \n")
																				.arg(param->strValue)
																				.arg(model->getName());
				throw std::runtime_error(QSTR(msg));
			}

			auto resolvedParam = model->getFather()->getParameter(param->strValue.mid(1));
			if(!resolvedParam){
				QString msg = QString("paramenter '%1' in model '%2' is not specified by the parent model '%3'. Either don't use the '%' syntax or add a parameter with name '%1' in coupled model '%3'   \n")
																				.arg(param->strValue)
																				.arg(model->getName())
																				.arg(getFullName(model->getFather()));
				throw std::runtime_error(QSTR(msg));
			}

			if(usePythonFather){
//				&& resolvedParam->father != nullptr && 	(resolvedParam->father ->father == NULL || resolvedParam->father ->father->father == NULL)){ // it is a 1st level model in the library, so use the python parameter
				ret += QString("self.%1").arg(toVariableName(resolvedParam->name)); // 'self' is the top model which should have the parameter defined
			} else {
				switch (resolvedParam->type) {
				case STR:
				case SCILAB:
					ret += "\"" + resolvedParam->strValue + "\"";
					break;
				case VAL:
					ret += QString("\"%1\"").arg(resolvedParam->dValue, 0, 'e');
					break;
				case LST:
					ret += "\"" + (resolvedParam->lsValues.  at(resolvedParam->lsValue - 1)) + "\"";
					break;
				}
			}
		} else {
			ret += "\"" + param->strValue + "\"";
		}

		break; // case SCILAB
	case VAL:
		ret += QString("%1").arg(param->dValue, 0, 'e');
		break;
	case LST:
		//printf("Valor de lista %d %d\n",param->lsValues.size(), param->lsValue);
		ret +=	"\"" + param->lsValues.at(param->lsValue - 1) + "\"";
		break;
	}

	//printf("Returning '%s'\n",QSTR(ret));
	return ret;
}

/**
 * Based on codegenerator.cpp:343
 */
modelParameter* PythonCodeGenerator::searchParameter(QString param, modelCoupled * c){
	QList<modelParameter*>::iterator j;
	//qDebug("Searching arg '%s' in %s\n",QSTR(par),QSTR(c->name));
	for (j = c->params.begin(); j != c->params.end(); j++) {
		if ((*j)->name.trimmed() == param.trimmed()) {
			if ((*j)->type == STR && (*j)->strValue.startsWith("%")) { // father also has a % parameter
				return searchParameter((*j)->strValue.mid(1),   c->father);
			}

			// parameter found
			return (*j);
		}
	}

	if (c->father!=NULL){
		return searchParameter(param, c->father);
	}

	printf("Parameter '%s' not found in model. An atomic model is using it as '%%' parameter. \n", QSTR(param));
	char buff[1024];
	sprintf (buff,"Parameter '%s' not found in model. An atomic model is using it as '%%' parameter.\n", QSTR(param));
	QMessageBox::critical(NULL,"PowerDEVS",buff);
	exit(-1);
}

QString PythonCodeGenerator::getFullName(modelCoupled * coupled){
	if (coupled->father == NULL || coupled->father->father == NULL){ // it is a 1st level model in the library, so use the python parameter
		return toVariableName(coupled);
	}

	return getFullName(coupled->father) + "." + toVariableName(coupled);
}

QString PythonCodeGenerator::getFullName(modelAtomic * atomic){
	if (atomic->father == NULL || atomic->father->father == NULL){ // it is a 1st level model in the library, so use the python parameter
		return toVariableName(atomic);
	}

	return getFullName(atomic->father) + "." + toVariableName(atomic);
}

QString PythonCodeGenerator::getFullName(modelChild * child){
	return child->childType==COUPLED? getFullName(child->coupled): getFullName(child->atomic);
}
