#ifndef __UTILS_OPTIONS_H__
#define __UTILS_OPTIONS_H__

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <typeinfo>

#include "types.h"



class OptionManager
{
	static OptionManager *instance;
	static OptionManager *get_instance();

	std::unordered_map<std::string, std::string> options;

public:
	OptionManager() {};
	~OptionManager() {};

	static void set_options(const std::unordered_map<std::string, std::string>&);
	static std::vector<std::string> keys();
	static void apply_options();

	/* checks if the option is defined.
	 * NOTE: does not attempt any conversion, so it is not directly related to method get<T>
	 * */
	static bool has(const std::string&);

	template<class T>
	static T get(const std::string &option)
	{
		OptionManager *instance = OptionManager::get_instance();

		if(instance->options.find(option) == instance->options.end())
		{
			if(check_conversion_to<T>(option))
				return to<T>(option);
			else
				throw std::runtime_error("invalid or unable to find option '" + option + "'. When attempting to read as type: '" + typeid(T).name() + "'");

		}

		std::string value = instance->options[option];
		if(check_conversion_to<T>(value))
			return to<T>(value);
		else
		{
			std::string error =
					"option ' " + option +\
					"' has value '" + value +\
					"', which is invalid!";
			throw std::runtime_error(error);
		}
	}

	static std::string get(const std::string &option)
	{
		OptionManager *instance = OptionManager::get_instance();

		if(instance->options.find(option) == instance->options.end())
			return option;

		return instance->options[option];
	}

	static void clear_instance();
};

#endif
