#ifndef __VARS_H__
#define __VARS_H__

#include <stdlib.h>
#include <limits>

#include "types.h"

//#define INF std::numeric_limits<double>::infinity() // does not work in some corner cases (ej: advance_time(y=[0][0][0], INF, 1) makes y=[-nan] because 0*infinity=nan which in this case it should be 0)
#define INF 1e20

extern Time realTi;
extern bool overRun;
extern int exitStatus;
extern double realTiSimulation;

#endif
