#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "engine/common/engine.h"
#include "engine/common/pdevslib.h"
#include "utils/globals.h"
#include "utils/options.h"

#include "interface.h"
#include "utils.h"

using namespace boost::python;


void run_scilab_script(const py_str &script_str)
{
	std::string script = extract<std::string>(script_str);
	std::stringstream stream;

	stream << "exec(\"" << script << "\")";

	executeVoidScilabJob(
			const_cast<char*>(stream.str().c_str()),
			true);
}

void set_options(const py_dict &py_options)
{
	OptionManager::set_options(to_map(py_options));

	std::string reading_backend =
			OptionManager::get<std::string>("reading_backend");
	std::string logging_backend =
			OptionManager::get<std::string>("logging_backend");

	set_reading_backend(reading_backend);
	set_logging_backend(logging_backend);
}

Py2PDEVSModel *new_model(
		double ti,
		const py_str &name)
{
	// Apply command-line options first.
	OptionManager::apply_options();
	return new Py2PDEVSModel(ti, name);
}
