/****************************************************************************
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

#ifndef PYTHON_CODEGENERATOR_H
#define PYTHON_CODEGENERATOR_H

#include <QMessageBox>
#include <QDebug>
#include <QSet>
#include <QString>
#include <QList>
#include <QCoreApplication>
#include <QApplication>
#include <QFileInfo>


#include "modelcoupled.h"
#include "modelatomic.h"
#include "modelparameter.h"
#include "parser.h"

#include "codegenerator.h" // reuse helper functions

class PythonCodeGenerator{

#define TAB(N) QSTR(QString("").fill(' ',2*(N)))
#define CHILDNAME(chld) ((chld)->childType==ATOMIC ? chld->atomic->name.trimmed() : chld->coupled->name.trimmed())

public:
	modelCoupled * libraryModel;
	QString outputFileName;

	PythonCodeGenerator(modelCoupled * model, QString outputFileName) :
		libraryModel(model), outputFileName(outputFileName){

	}

	void generatePythonLibrary();
private:
	QStringList generateTopModel(modelChild* model, int tabs=0);
	QString getCreateCode(modelCoupled* coupledModel, int tabs=4);
	QString getCreateCode(modelAtomic* atomicModel, QString parent, int tabs=4);
	QString getAdditionalMethods(modelCoupled* coupledModel, int tabs);

	QString getChildName(modelCoupled* coupledModel, int childIndex);
	QString toVariableName(QString paramName, int index);
	QString toVariableName(modelChild* child);
	QString toVariableName(modelAtomic* atomic);
	QString toVariableName(modelCoupled* coupled);
	QString getParameterValue (modelParameter* param);
	QString getParametersForFunctionArgument(modelChild* child);
	QString getParameterNames (QList<modelParameter*> params, QString prefix="");
	QString getParameterValues (QList<modelParameter*> params);
	QString getParameterString(modelParameter* params, modelChild * child, bool usePythonFather);
	QString getInheritedParameterValues(modelCoupled* parent, modelCoupled* child);
	modelParameter* searchParameter(QString param, modelCoupled * c);
	QString resolveAtomicParams(modelAtomic* atomicModel, QString parent);
	QString getFullName(modelCoupled * c);
	QString getFullName(modelAtomic * atomic);
	QString getFullName(modelChild * child);
};



#endif
