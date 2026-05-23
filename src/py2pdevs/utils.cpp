#include <iostream>
#include <sstream>

#include "utils.h"

using namespace boost::python;


std::vector<std::string> to_vector(const py_list &list)
{
	std::vector<std::string> v;
	for(int i = 0; i < len(list); ++i)
	{
		object value = list[i];
		std::string value_str = extract<std::string>(str(value));
		v.push_back(value_str);
	}
	return v;
}

std::unordered_map<std::string, std::string>
to_map(const py_dict &dict)
{
	py_list keys = dict.keys();
	std::unordered_map<std::string, std::string> map;

	for(int i = 0; i < len(keys); ++i)
	{
		object value = dict[keys[i]];
		std::string key_str = extract<std::string>(keys[i]);
		std::string value_str = extract<std::string>(str(value));

		map[key_str] = value_str;
	}

	return map;
}

