#include <cstdlib>
#include <iostream>
#include <sstream>

#include <boost/python.hpp>
#include <Python.h>

#include "pdevslib.h"
#include "pdevs2py.h"


void run_python_module(
		const std::string &path,
		const std::string &module)
{
	std::string current_pythonpath = std::getenv("PYTHONPATH");
	std::string model_path = getExecutablePath();
	std::string paths = current_pythonpath + ":" + path + ":" + model_path + "/../build/lib";
	setenv("PYTHONPATH", paths.c_str(), 1);

	Py_Initialize();
	try
	{
		boost::python::dict locals;
		boost::python::object main = boost::python::import("__main__");
		boost::python::object globals = main.attr("__dict__");

		std::stringstream stream;
		std::string mod_path = path + "/" + module + ".py";
		stream << "import subprocess\nsubprocess.call(['python', '" << mod_path << "'])";
		boost::python::str module_str(stream.str());

		boost::python::exec(module_str, globals);
	}
	catch(boost::python::error_already_set &)
	{
		std::cerr << "Warning: finalization script failed!" << std::endl ;

		// TODO: this might not work property in some compilers. See https://www.boost.org/doc/libs/1_62_0/libs/python/doc/html/reference/high_level_components/boost_python_errors_hpp.html
		boost::python::handle_exception();

//      NOTE: Previous implementation below
//		        it does catch errors in the script.
//		        but does not catch certain errors (ej: out of memory, Exception OSError)
//

//	    PyObject *ptype, *pvalue, *ptraceback;
//	    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
//
//	    std::string error = boost::python::extract<std::string>(pvalue);
//
//	    std::cerr << "Python error: " << std::endl;
//	    std::cerr << error << ")\n";
	}
}
