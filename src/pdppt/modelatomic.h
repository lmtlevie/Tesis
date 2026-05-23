/****************************************************************************
 **
 **  Copyright (C) 2009 Facultad de Ciencia Exactas Ingenieria y Agrimensura
 **										 Universidad Nacional de Rosario - Argentina.
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

#ifndef MODELATOMIC_H
#define MODELATOMIC_H

#include <QString>
#include <QList>
#include "modelparameter.h"
#include "modelcoupled.h"

class modelAtomic {
public:
	int inPorts;
	int outPorts;
	QString name;
	int index=-1; // todo: set it when parsing, now only used from Python generation
	QString path;
	QString desc;
	QString paramsString;
	QList < modelParameter * >params;
	QList < QString > extra;
	class modelCoupled *father = NULL;

	modelParameter* getParameter(QString paramName){
		for(auto param : this->params){
			if(paramName == param->name) return param;
		}

		return nullptr;
	}
};
#endif
