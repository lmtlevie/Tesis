#ifndef _PY2PDEVS_UTILS_H_
#define _PY2PDEVS_UTILS_H_

#include <string>
#include <vector>
#include <istream>
#include <unordered_map>

#include "py_types.h"

#include <boost/python.hpp>

#define TO_C_STR(s) (extract<char*>(s))
#define TO_STR(s)   (extract<std::string>(s))

std::vector<std::string> to_vector(const py_list &list);
std::unordered_map<std::string, std::string> to_map(const py_dict &dict);

#endif
