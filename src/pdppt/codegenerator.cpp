/***************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingenieria y Agrimensura
**		  Universidad Nacional de Rosario - Argentina.
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

#include <QMessageBox>
#include <QDebug>
#include "codegenerator.h"
#include "modelcoupled.h"

#define TAB(N) QSTR(QString("").fill(' ',2*N)) // Defines a macro TAB(N) that returns a QString filled with 2 * N spaces.
#define S_TAB(N) std::string(2 * (N), ' ') // TODO: make this the default tab macro
#define CHILDNAME(chld) ((chld)->childType==ATOMIC ? chld->atomic->name.trimmed() : chld->coupled->name.trimmed())

QSet < QString > includes;
QStringList includes_dirs;
QStringList extra_libs;
QStringList extra_flags;
QSet < QString > objects;
QStringList constructor;
QStringList state;
int currentCoupled;
QString currentCoupledName;
QFile *fdPds; // TODO: this should not be a global variable
bool showMsgBoxForWarning = true; // TODO: this should not be a global variable

/**************************************************************************
 * getRelativePath
 *
 * @param
 **************************************************************************/
QString getRelativePath(QString p)
{
	return p.trimmed().replace("\\", "/");
}

/**************************************************************************
 * getBaseFilename
 *
 * @param
 **************************************************************************/
QString getBaseFilename(QString s)
{
	return getRelativePath(s.left(s.lastIndexOf(".")));
}

/**************************************************************************
 * getClassName
 *
 * @param
 **************************************************************************/
QString getClassName(QString p)
{
	QString l = p.left(p.lastIndexOf("."));
	int slash = p.lastIndexOf("/") + 1;
	int bslash = p.lastIndexOf("\\") + 1;
	int m = (slash > bslash ? slash : bslash);
	return l.mid(m);
}

/***************************************************************************
 * printConnection
 *
 * @param
 * @param
 ***************************************************************************/
void printConnection(modelLine * i, modelCoupled * c)
{
	printf("Connection from ");
	if ((i)->sourceType == TOKCMP) {
		printf("%s(%d) (%s:%d) to ",
		       QSTR(CHILDNAME(c->childs.at((i)->sources.first() - 1))),
		       (i)->sources.first(),
		       ((i)->sources.at(2) != 0 ? "INPUT" : "OUTPUT"),
		       (i)->sources.at(1) - 1);
	}
	if ((i)->sourceType == TOKPRT) {
		printf("%s (%s:%d) to ", "Port",
		       (c->lsPorts.at(i->sources.at(0) - 1)->type ==
			INPORT ? "INPUT" : "OUTPUT"),
		       c->lsPorts.at(i->sources.at(0) - 1)->coupledPort - 1);
	}
	if ((i)->sourceType == TOKPNT) {
		printf("Point %d to ", (i)->sources.at(0));
	}

	if ((i)->sinkType == TOKCMP) {
		printf("%s(%d) (%s:%d) to ",
		       QSTR(CHILDNAME(c->childs.at((i)->sinks.first() - 1))),
		       (i)->sinks.first(),
		       ((i)->sinks.at(2) != 0 ? "INPUT" : "OUTPUT"),
		       (i)->sinks.at(1) - 1);
	}
	if ((i)->sinkType == TOKPRT) {
		printf("%s (%s:%d) to ", "Port",
		       (c->lsPorts.at(i->sinks.at(0) - 1)->type ==
			INPORT ? "INPUT" : "OUTPUT"),
		       c->lsPorts.at(i->sinks.at(0) - 1)->coupledPort - 1);

	}
	if ((i)->sinkType == TOKPNT) {
		printf("Point %d", (i)->sinks.at(0));
	}
	printf("\n");
}

/****************************************************************************
 * computeConnections
 *
 * @param
 ****************************************************************************/
void computeConnections(modelCoupled * c)
{
	QList < modelLine * >::iterator i, o, d;
	modelPoint *p;
	modelPoint *n;
	QList < int >::iterator j, k;
	modelLine *lk, *lj;
	QList < modelLine * >l;
	int pointNumber = 1;
	while (c->points.size() != 0) {
		p = c->points.first();
		for (j = p->lines.begin(); j != p->lines.end(); j++) {
			for (k = j + 1; k != p->lines.end(); k++) {
				lj = c->lines.at(*j - 1);
				lk = c->lines.at(*k - 1);
				if (lj->sourceType == TOKPNT
				    && lj->sources.first() == pointNumber) {
					if (lk->sinkType == TOKPNT
					    && lk->sinks.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sinkType;
						nl->sources = lj->sinks;
						nl->sinkType = lk->sourceType;
						nl->sinks = lk->sources;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest > pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
					}
				}
				if (lj->sourceType == TOKPNT
				    && lj->sources.first() == pointNumber) {
					if (lk->sourceType == TOKPNT
					    && lk->sources.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sinkType;
						nl->sources = lj->sinks;
						nl->sinkType = lk->sinkType;
						nl->sinks = lk->sinks;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest > pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size());
							}
						}
					}
				}
				if (lj->sinkType == TOKPNT
				    && lj->sinks.first() == pointNumber) {
					if (lk->sourceType == TOKPNT
					    && lk->sources.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sourceType;
						nl->sources = lj->sources;
						nl->sinkType = lk->sinkType;
						nl->sinks = lk->sinks;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size ());
							}
						}
					}
				}
				if (lj->sinkType == TOKPNT
				    && lj->sinks.first() == pointNumber) {
					if (lk->sinkType == TOKPNT
					    && lk->sinks.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sourceType;
						nl->sources = lj->sources;
						nl->sinkType = lk->sourceType;
						nl->sinks = lk->sources;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest > pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
					}
				}
			}
		}
		pointNumber++;
		c->points.removeFirst();
	}
	QList < modelLine * >removedLines;
	for (i = c->lines.begin(); i != c->lines.end(); i++) {	// Remove repeated lines and check for feedbacks
		bool repeated = false;
		if (((*i)->sourceType == TOKCMP) &&
		    ((*i)->sinkType == TOKCMP) &&
		    ((*i)->sources.first() == (*i)->sinks.first())) {
			int in = (*i)->sources.at(2);
			int out = (*i)->sinks.at(2);
			if (in != out) {
				//((*i)->sources.at(2) != (*i)->sinks.at(2))  // Theres feedback
				int child = (*i)->sources.first() - 1;
				modelChild *mc = c->childs.at(child);
        char buff[1024];
				printf ("There is a direct feedback in the component '%s'\n", QSTR(mc->childType == ATOMIC ? mc->atomic->name : mc-> coupled->name));
				sprintf (buff,"There is a direct feedback in the component '%s'\n", QSTR(mc->childType == ATOMIC ? mc->atomic->name : mc-> coupled->name));
        QMessageBox::critical(NULL,"PowerDEVS",buff);
				exit(-1);
			}
		}

		for (d = i + 1; d != c->lines.end(); d++) {
			if (((*i)->sourceType == (*d)->sourceType) &&
			    ((*i)->sources == (*d)->sources) &&
			    ((*i)->sinkType == (*d)->sinkType) &&
			    ((*i)->sinks == (*d)->sinks)) {
				repeated = true;
			}
		}
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKCMP
		    && ((*i)->sources.at(2) == (*i)->sinks.at(2))) {
			continue;
		}
		if ((*i)->sinkType == TOKPRT &&
		    (*i)->sourceType == TOKCMP &&
		    ((*i)->sources.at(2) != 0) &&
		    (c->lsPorts.at((*i)->sinks.first() - 1)->type != INPORT)) {
			continue;
		}
		if ((*i)->sourceType == TOKPRT &&
		    (*i)->sinkType == TOKCMP &&
		    ((*i)->sinks.at(2) != 0) &&
		    (c->lsPorts.at((*i)->sources.first() - 1)->type !=
		     INPORT)) {
			continue;
		}
		if ((*i)->sinkType == TOKPNT || (*i)->sourceType == TOKPNT) {
			continue;
		}
		if (!repeated)
			removedLines << (*i);
	}
	c->lines = removedLines;

	for (i = c->lines.begin(); i != c->lines.end(); i++) {
		if (((*i)->sourceType == TOKCMP && (*i)->sources.at(2) != 0) || ((*i)->sourceType == TOKPRT && c->lsPorts.at((*i)->sources.first() - 1)->type != INPORT)) {	// Swtich order
			QString t;
			QList < int >tt;
			t = (*i)->sourceType;
			tt = (*i)->sources;
			(*i)->sourceType = (*i)->sinkType;
			(*i)->sources = (*i)->sinks;
			(*i)->sinkType = t;
			(*i)->sinks = tt;
		}
	}
}

/****************************************************************************
 * processConnections
 *
 * @param
 ****************************************************************************/
void processConnections(modelCoupled * c)
{
	QList < modelLine * >::iterator i;
	for (i = c->lines.begin(); i != c->lines.end(); i++) {
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKCMP)	// IC Connection
		{
			if ((*i)->sources.at(2) == (*i)->sinks.at(2)) {
				continue;
			}
			modelConnection *con = new modelConnection();
			con->childSource = (*i)->sources.first() - 1;
			con->sourcePort = (*i)->sources.at(1) - 1;
			con->childSink = (*i)->sinks.first() - 1;
			con->sinkPort = (*i)->sinks.at(1) - 1;
			c->lsIC.append(con);
		}
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKPRT)	// EIC
		{
			modelConnection *con = new modelConnection();
			con->childSource = 0;
			con->sourcePort =
			    c->lsPorts.at((*i)->sources.first() -
					  1)->coupledPort - 1;
			con->childSink = (*i)->sinks.first() - 1;
			con->sinkPort = (*i)->sinks.at(1) - 1;
			c->lsEIC.append(con);
		}
		if ((*i)->sinkType == TOKPRT && (*i)->sourceType == TOKCMP) {
			modelConnection *con = new modelConnection();
			con->childSource = 0;
			con->childSource = (*i)->sources.first() - 1;
			con->sourcePort = (*i)->sources.at(1) - 1;
			con->childSink = 0;
			con->sinkPort =
			    c->lsPorts.at((*i)->sinks.first() -
					  1)->coupledPort - 1;
			c->lsEOC.append(con);
		}
	}
}

/*********************************************************************
 * getParameter
 *
 * @param
 * @param
 *********************************************************************/
modelParameter *getParameter(QString par, modelCoupled * c)
{
	QList < modelParameter * >::iterator j;
	//qDebug("Searching arg '%s' in %s\n",QSTR(par),QSTR(c->name));
	for (j = c->params.begin(); j != c->params.end(); j++) {
		//      qDebug("Comparing against '%s'\n",QSTR((*j)->name));
		if ((*j)->name.trimmed() == par.trimmed()) {
			if ((*j)->type == STR && (*j)->strValue.startsWith("%")) {
				return getParameter((*j)->strValue.mid(1),
						    c->father);
			}
			return (*j);
		}
	}
  if (c->father!=NULL)
    return getParameter(par, c->father);
	printf("Parameter %s not found in model. An atomic model uses it as a '%%' parameter.\n", QSTR(par));
  char buff[1024];
  sprintf (buff,"Parameter %s not found in model. An atomic model uses it as a '%%' parameter.\n", QSTR(par));
  QMessageBox::critical(NULL,"PowerDEVS",buff);
	exit(-1);
}

/**************************************************************************
 * getParameterString
 *
 * @param
 **************************************************************************/
QString getParameterString(QList < modelParameter * >params, modelAtomic * a)
{

	QString ret;
	QList < modelParameter * >::iterator i;
	for (i = params.begin(); i != params.end(); ++i) {

		//printf("Evaluating parameter %s of atomic %s\n", QSTR((*i)->strValue),QSTR(a->name));
		switch ((*i)->type) {
		case STR:
		case SCILAB:
			if ((*i)->strValue == "%Inputs") {
				ret += QString("\"%1\"").arg((double)a->inPorts, 0, 'e');
			} else if ((*i)->strValue == "%Outputs") {
				ret += QString("\"%1\"").arg((double)a->outPorts, 0, 'e');
			} else {
				if ((*i)->strValue.startsWith("%")) {
					QString par = (*i)->strValue.mid(1);
					modelParameter *j = getParameter(par, a->father);
					switch (j->type) {
					case STR:
					case SCILAB:
						ret += "\"" + j->strValue + "\"";
						break;
					case VAL:
						ret += QString("\"%1\"").arg(j->dValue, 0, 'e');
						break;
					case LST:
						ret += "\"" + (j->lsValues.  at(j->lsValue - 1)) + "\"";
						break;
					}
				} else {
					ret += "\"" + (*i)->strValue + "\"";
				}
			}
			break;
		case VAL:
			ret += QString("%1").arg((*i)->dValue, 0, 'e');
			break;
		case LST:
			//printf("Valor de lista %d %d\n",(*i)->lsValues.size(), (*i)->lsValue);
			ret +=
			    "\"" + (*i)->lsValues.at((*i)->lsValue - 1) + "\"";
			break;
		}
		if (i + 1 != params.end())
			ret += ",";
	}
	//printf("Returning '%s'\n",QSTR(ret));
	return ret;
}

/***************************************************************
 * getExtraDep
 *
 * @param
 ***************************************************************/
QString getExtraDep(QString at)
{
	QString ret = "";
	QString path = QCoreApplication::applicationDirPath();
	QString file = getRelativePath(path + QString("/../atomics/") + at + ".h");
  qDebug() << "Getting extra deps for "<< file;
  char buff[1024];

	QFile fd(file);
	if (fd.open(QIODevice::ReadOnly | QIODevice::Text)) {
    while (fd.readLine(buff,1024)>0) {
      QString line(buff);
      if (line.startsWith("//cpp:",Qt::CaseInsensitive)) {
        //qDebug() << "Line " << line << " " << getClassName(at);
        if (!line.contains(getClassName(at) + ".cpp"))
          ret +=  " $(ATOMICS)/" + getBaseFilename(line.mid(6)) + ".cpp";
      } else if (line.startsWith("//lib:",Qt::CaseInsensitive)) {
          extra_libs <<  line.mid(6).trimmed();
      } else if (line.startsWith("//flags:",Qt::CaseInsensitive)) {
          extra_flags<<  line.mid(8).trimmed();
      } else if (line.startsWith("//headersdir:",Qt::CaseInsensitive)) {
          includes_dirs << "-I" +  line.mid(13).trimmed();
      }
      if (line.startsWith("#include",Qt::CaseInsensitive)) {
        QString header = line.mid(8).trimmed().mid(1);
        header = header.left(header.length()-1);
	      QFile hfile(getRelativePath(path + QString("/../atomics/") + header));
        if (hfile.exists())
        {
          ret +=  " $(ATOMICS)/" + header;
          //qDebug() << "Header->" << header;
        }
      }
		}
		fd.close();
	}

	return ret;

}

/*************************************************************
 * getExtraObjs
 *
 * @param
 *************************************************************/
QSet < QString > getExtraObjs(QString at)
{

	QSet < QString > ret;
	QString path = QCoreApplication::applicationDirPath();
	QString file = getRelativePath(path + QString("/../atomics/") + at);

	QFile fd(file);
	if (fd.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString line = QString(fd.readLine());
    //qDebug() << line;
		while (!line.startsWith("#")) {
			line = getRelativePath(line);
			if (line.startsWith("//cpp:",Qt::CaseInsensitive) && !line.contains(getClassName(at) + ".cpp")) {
				objects.insert(getBaseFilename(line.mid(6)) + ".o");
        qDebug() << "Adding extra obj " << getBaseFilename(line.mid(6)) + ".o";
			}
			line = QString(fd.readLine());
		}
		fd.close();
	}

	return ret;
}


class FlexibleErrorReporter {
public:
	static void report(const QString &message) {
		// Non-GUI context
		qCritical() << message;

		if (showMsgBoxForWarning && qobject_cast<QApplication *>(QCoreApplication::instance())) {
			// GUI context
			QMessageBox msgBox;
			msgBox.setText(message);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle("PowerDEVS pdppt");
			msgBox.exec();
		}
	}
};

/*********************************************************
 * processChilds
 *
 * @param
 * @param
 * @param
 *********************************************************/
void processChilds(QList < modelChild * >childs, int depth, QString user_path) {

	int idCoupled = currentCoupled;
	QString nameCoupled = currentCoupledName;

	QList < modelChild * >::iterator i;
	int currentChild = 0;
	//! go through all children, either coupled or atomic
	for (i = childs.begin(); i != childs.end(); ++i, currentChild++) {
		if ((*i)->childType == ATOMIC) { // Atomic
			if ((*i)->atomic->path.isEmpty()) {
				FlexibleErrorReporter::report(QString("Error: The atomic %1 has no implementation associated.").arg((*i)->atomic->name));
				// qDebug() << "WARNING: "<< QString("Error: The atomic %1 has no implementation associated.").arg((*i)->atomic->name);

				// if(showMsgBoxForWarning){
				//	QMessageBox msgBox;
				//	msgBox.setText (QString("Error: The atomic %1 has no implementation associated.").arg((*i)->atomic->name));
				//	msgBox.setIcon(QMessageBox::Critical);
				//	msgBox.setWindowTitle("PowerDEVS");
				//	msgBox.exec();
				// }
				exit(-1);
			} else { //! if atomic's path is not empty
				QString path = QCoreApplication::applicationDirPath();
				QString file = getRelativePath(path + QString("/../atomics/") + (*i)->atomic->path);
				QString file2 = getRelativePath(user_path + QString("/atomics/") + (*i)->atomic->path);
				QFile f(file);
				QFile f2(file2);
				qDebug() << file;
				qDebug() << file2;
				if (!f.exists() && !f2.exists()) {
					//! try it in lower case
					QFile file_lower(getRelativePath(path + QString("/../atomics/") + (*i)->atomic->path.toLower()));
					QFile file_lower2(getRelativePath(user_path + QString("/atomics/") + (*i)->atomic->path.toLower()));
					if (!file_lower.exists() && !file_lower2.exists()) {
						//! atomic doesn't exist in path
						 FlexibleErrorReporter::report(QString("Error: The implementation of atomic %1 does not exist (File missing %2 or %3).").arg((*i)->atomic->name).arg(file).arg(file2));
	        	  			// qDebug() << "WARNING: "<< QString("Error: The implementation of atomic %1 does not exists (File missing %2 or %3).").arg((*i)->atomic->name).arg(file).arg(file2);

						// if(showMsgBoxForWarning) {
						// 	QMessageBox msgBox;
						// 	msgBox.setText (QString("Error: The implementation of atomic %1 does not exists (File missing %2 or %3).").arg((*i)->atomic->name).arg(file).arg(file2));
						//	msgBox.setIcon(QMessageBox::Critical);
						//	msgBox.setWindowTitle("PowerDEVS");
						//	msgBox.exec();
						// }
						exit(-1);
					} else {
            					// Convert it to lower
						(*i)->atomic->path = (*i)->atomic->path.toLower();
					}
				} 
			}

			//QString path = getRelativePath((*i)->atomic->path);
			//int index = path.lastIndexOf("atomics/");
			//if (index == -1) { // atomic in powerdevs path
				includes.insert(getRelativePath( (*i)->atomic->path) );
				objects.insert(getBaseFilename(  (*i)->atomic->path) + ".o" );
			//} else { // atomic in my folder
			//	includes.insert(getRelativePath (path.mid(index+8)));
			//	objects.insert(getBaseFilename(path.mid(index+8)) + ".o");
			//}

			getExtraObjs((*i)->atomic->path);
			constructor << QString("%1//D%2[%3] is  %4\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg((*i)->atomic->name.trimmed());
			constructor << QString("%1D%2[%3] = new %4(\"%5\");\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg(getClassName((*i)->atomic->path)).arg((*i)->
								      		atomic->
								      		name.
								      		trimmed
								      		());

			// set the father
			constructor << QString("%1((Simulator*)D%2[%3])->father = %4;\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg(nameCoupled);

			if ((*i)->atomic->paramsString.trimmed() != "") {
				constructor << QString("%1D%2[%3]->init(0.0,%4);\n")
						.arg(TAB(depth))
						.arg(idCoupled)
						.arg(currentChild)
						.arg((*i)->atomic->paramsString);
			} else if ((*i)->atomic->params.size() != 0) {
				constructor << QString("%1D%2[%3]->init(0.0,%4);\n")
						.arg(TAB(depth))
						.arg(idCoupled)
						.arg(currentChild)
						.arg(getParameterString
							  ((*i)->atomic->params,
							   (*i)->atomic));
			} else {
				constructor << QString("%1D%2[%3]->init(0.0);\n")
						.arg(TAB(depth))
						.arg(idCoupled)
						.arg(currentChild);
			}
		} else { // Coupled
			int childCoupled = ++currentCoupled;
			constructor << QString("%1//D%2[%3] is %4\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg((*i)->coupled->name);
			//constructor<< QString("%1D%2[%3] = new Coupling();\n").arg(TAB(depth)).arg(currentCoupled).arg(currentChild);
			constructor << QString("%1D%2[%3] = new Coupling(\"%4\");\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg((*i)->coupled->name);

			// set the current coupled (will be used in the next recursion of processChilds)
			currentCoupledName = QString("(Coupling*)D%1[%2]")
						.arg(idCoupled)
						.arg(currentChild);

			// set the father
			constructor << QString("%1((Simulator*)D%2[%3])->father = %4;\n")
					.arg(TAB(depth))
					.arg(idCoupled)
					.arg(currentChild)
					.arg(nameCoupled);

			constructor << QString("%1Simulator **D%2 = new Simulator* [%3];\n")
					.arg(TAB(depth + 1))
					.arg(childCoupled)
					.arg((*i)->coupled->
						      childs.
						      size());

			constructor << QString("%1Connection **EIC%2 = new Connection* [%3];\n")
					.arg(TAB(depth + 1))
					.arg(childCoupled)
					.arg((*i)->
					      coupled->
					      lsEIC.
					      size());
			constructor << QString("%1Connection **EOC%2 = new Connection* [%3];\n")
					.arg(TAB(depth + 1))
					.arg(childCoupled)
					.arg((*i)->
						coupled->
						lsEOC.
						size());
			constructor << QString("%1Connection **IC%2 = new Connection* [%3];\n")
					.arg(TAB(depth + 1))
					.arg(childCoupled)
					.arg((*i)->
					      coupled->
					      lsIC.
					      size());
			processChilds((*i)->coupled->childs, depth + 1, user_path);
			QList < modelConnection * >::iterator l;
			int count = 0;
			for (l = (*i)->coupled->lsEIC.begin();
			     l != (*i)->coupled->lsEIC.end(); l++, count++) {
				constructor <<
				    QString
				    ("%1EIC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1EIC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			};
			count = 0;
			for (l = (*i)->coupled->lsEOC.begin();
			     l != (*i)->coupled->lsEOC.end(); l++, count++) {
				constructor <<
				    QString
				    ("%1EOC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1EOC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			};
			count = 0;
			for (l = (*i)->coupled->lsIC.begin();
			     l != (*i)->coupled->lsIC.end(); l++, count++) {
				constructor <<
				    QString("%1IC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1IC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			}
			constructor <<
			    QString
			    ("%1((Coupling*)D%2[%3])->setup(D%4,%5,IC%6,%7,EIC%8,%9,EOC%10,%11);\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).arg
			    (childCoupled).arg((*i)->coupled->childs.
					       size()).arg(childCoupled).arg((*i)->coupled->lsIC.size()).arg(childCoupled).arg((*i)->coupled->lsEIC.size()).arg(childCoupled).arg((*i)->coupled->lsEOC.size());
			constructor <<
			    QString("%1((Coupling*)D%2[%3])->init(ti);\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild);
		} // end Coupled
	} // end for loop
}

/************************************************************************
 * generatePDS
 *
 * This function generates the PDS (Presumably some kind of structured representation or specification)
 * of a given modelCoupled, which represents a coupled simulation model.
 *
 * @param
 ************************************************************************/
void generatePDS(modelCoupled * c, int depth) {

	// Compute the connections for the provided model
	// qDebug("Generating Coupled %s",QSTR(c->name));
	computeConnections(c);

	// Process the computed connections to make them ready for PDS generation
	processConnections(c);
	// qDebug("Connections ready %s",QSTR(c->name));

	char buff[100000]; // Buffer to store formatted strings before writing to file
	
	// If the model type is the root, write "Root-Coordinator"
	// Otherwise, based on the depth, write "Coordinator" with appropriate indentation
	if (c->type.trimmed() == TOKROOT) {
		sprintf(buff, "Root-Coordinator\n");
		fdPds->write(buff, strlen(buff));
	} else {
		sprintf(buff, "%sCoordinator\n", TAB(depth));
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s{\n", TAB(depth + 1));
	fdPds->write(buff, strlen(buff));

	// Iterate through each child of the model
	QList < modelChild * >::iterator i;
	for (i = c->childs.begin(); i != c->childs.end(); i++) {
		// If the child is an atomic model, write its details to the file
		if ((*i)->childType == ATOMIC) {
			sprintf(buff, "%sSimulator\n", TAB(depth + 2));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%s{\n", TAB(depth + 3));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%sPath = %s\n", TAB(depth + 4),QSTR((*i)->atomic->path));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%sParameters = %s\n", TAB(depth + 4),
				QSTR(getParameterString((*i)->atomic->params, (*i)->atomic)));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%s}\n", TAB(depth + 3));
			fdPds->write(buff, strlen(buff));
		} else {
			// If the child is another coupled model, recursively generate its PDS
			generatePDS((*i)->coupled, depth + 2);
		}
	}

	QList < modelConnection * >::iterator con;
	// --- External Input Connections (EIC) section starts here ---
	// Writing the EIC section tag with appropriate indentation
	sprintf(buff, "%sEIC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	// Opening the EIC section with a '{'
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	// Iterating through each External Input Connection (EIC) in the list of EICs for the model
	for (con = c->lsEIC.begin(); con != c->lsEIC.end(); con++) {
		// Writing each connection's details in the format "(source, sourcePort);(sink, sinkPort)"
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	// Closing the EIC section with a '}'
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	// --- External Output Connections (EOC) section starts here ---
	// Similar structure to the EIC section above
	sprintf(buff, "%sEOC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));
	for (con = c->lsEOC.begin(); con != c->lsEOC.end(); con++) {
		// Again, writing each connection's details
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	// --- Internal Connections (IC) section starts here ---
	// Again, follows a similar structure as the EIC and EOC sections
	sprintf(buff, "%sIC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));
	for (con = c->lsIC.begin(); con != c->lsIC.end(); con++) {
		// Writing each internal connection's details
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	// Closing the overall block for this Coordinator's PDS representation
	sprintf(buff, "%s}\n", TAB(depth + 1));
	fdPds->write(buff, strlen(buff));

	// Debug logging to indicate the completion of this section of the model's PDS generation
	// (uncomment if needed for debugging purposes)
	//qDebug("Coupled %s finished",QSTR(c->name));
}

/*********************************************************************
 * generateModel
 * 
 * This function is designed to generate C++ code representing a simulation model.
 * The generated code is written to a file named `model.h`.
 *
 * @param
 *********************************************************************/
void generateModel(modelCoupled * c, QString user_path)
{
	// Initial setup for the generated class definition
	currentCoupled = 0;
	currentCoupledName = "mainCoupling";

	// Begin the construction of the "Model" class that inherits from "RootSimulator"
	constructor << QString("class Model: public RootSimulator {\n");
	constructor << QString("%1public:\n").arg(TAB(1));
	constructor << QString("%1Model (Time ti): RootSimulator((char*)\"%2\", (char*)\"%3\") {\n")
			.arg(TAB(2))
			.arg(c->name)
			.arg(c->path);

	// Initialize the main coupling for the model
	constructor << QString("%1mainCoupling = new RootCoupling((char*)\"%2\");\n")
			.arg(TAB(3))
			.arg(c->name);

	// Allocate memory for simulators and connections based on the model's details
	constructor << QString("%1Simulator **D%2 = new Simulator* [%3];\n")
			.arg(TAB(3))
			.arg(currentCoupled)
			.arg(c->childs.size());
	constructor << QString("%1Connection **IC%2 = new Connection* [%3];\n")
			.arg(TAB(3))
			.arg(currentCoupled)
			.arg(c->lsIC.size());

	// Process child models of the given model
	processChilds(c->childs, 3, user_path);

	// Setup interconnections (connections between models)
	QList < modelConnection * >::iterator l;
	int count = 0;
	for (l = c->lsIC.begin(); l != c->lsIC.end(); l++, count++) {
		constructor << QString("%1IC%2[%3] = new Connection();\n")
				.arg(TAB(3))
				.arg(0)
				.arg(count);
		constructor << QString("%1IC%2[%3]->setup(%4,%5,%6,%7);\n")
				.arg(TAB(3))
				.arg(0)
				.arg(count)
				.arg((*l)->childSource)
				.arg((*l)->sourcePort)
				.arg((*l)->childSink)
				.arg((*l)->sinkPort);
	}

	// Finalize setup for the main coupling
	constructor << QString("%1mainCoupling->setup(D%2,%3,IC%4,%5);\n")
			.arg(TAB(3))
			.arg(0)
			.arg(c->childs.size())
			.arg(0)
			.arg(c->lsIC.size());

	//constructor<< QString("%1mainCoupling->init(ti);\n").arg(TAB(3));

	// Link the main coupling with the root simulator
	constructor <<
	    QString("%1((RootCoupling*)mainCoupling)->rootSim = this;\n").
	    arg(TAB(3));

	// Close off the constructor and class definition
	constructor << QString("%1}\n").arg(TAB(2));
	constructor << QString("%1};\n").arg(TAB(1));

	// Prepare the includes for the generated code
	QStringList::iterator i;
	QSet<QString>::iterator j;
	includes << "engine.h";

	// Create the output file in the build directory relative to the application path
	QString path = QCoreApplication::applicationDirPath();
	QFile *fd; 
	if (user_path=="") {
		fd = new QFile(path + QString("/../build/model.h"));
	} else {
		fd = new QFile(user_path + QString("/build/model.h"));
	}
	//QFile *fd = new QFile("./model.h");
	
	// Open the file and write the generated code to it
	fd->open(QIODevice::WriteOnly | QIODevice::Text);

	// Write includes to the file
	for (j = includes.begin(); j != includes.end(); j++) {
		QString w = QSTR(QString("#include <%1>\n").arg(*j));
		fd->write(QSTR(w), w.size());
	}

	// Write the constructed class definition to the file
	for (i = constructor.begin(); i != constructor.end(); i++) {
		fd->write(QSTR(*i), (*i).size());
	}

	// Clean up: close the file and delete the file descriptor
	fd->close();

	delete fd;
}

/****************************************************************************
 * generateCode
 *
 * @param
 * @param
 * @param
 * @param
 * @param
 * @param
 ****************************************************************************/
int generateCode(modelCoupled * c, QString pdsName, bool generateCCode,
		 bool genPDS, bool silent, QString user_path)
{
	showMsgBoxForWarning = !silent;

	if (genPDS) {
		fdPds = new QFile(pdsName); // TODO: this should not be a global variable
		fdPds-> open(QIODevice::WriteOnly | QIODevice::Text | QIODevice:: Truncate);
		generatePDS(c, 0);
		fdPds->close();
	}
	if (generateCCode) {
		generateModel(c,user_path);
	}
	return 0;
}
