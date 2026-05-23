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

#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

//---------------------- agregado 24-01-2018 -----------------------
#define MSG_DATA_SIZE 512

struct SQUEUEMSG{//ver de hacerlo más generico
	//~ int size;//size of data field is must be <=MSG_DATA_SIZE
	//~ void* data;
	int port;
	char data[MSG_DATA_SIZE];
};
typedef struct SQUEUEMSG QUEUEMSG;
//-------------------------- fin agregado --------------------------

typedef double  Time;
typedef int     Port;
typedef void* Value;

#endif
