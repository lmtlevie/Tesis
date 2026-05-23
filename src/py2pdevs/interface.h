#ifndef _PY2PDEVS_INTERFACE_H_
#define _PY2PDEVS_INTERFACE_H_

#include "py_types.h"
#include <boost/python.hpp>

#include "py2pdevs_model.h"


void run_scilab_script(const py_str&);
void set_options(const py_dict&);
Py2PDEVSModel *new_model(double, const py_str&);

#endif
