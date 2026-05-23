#include "options.h"
#include "engine/common/pdevslib.h"

OptionManager *OptionManager::instance = nullptr;


OptionManager *OptionManager::get_instance()
 {
	 if(OptionManager::instance == nullptr)
		 OptionManager::instance = new OptionManager();
	 return OptionManager::instance;
 }

 void OptionManager::clear_instance()
 {
	 delete OptionManager::instance;
 }

void OptionManager::set_options(
		const std::unordered_map<std::string, std::string> &options)
{
	OptionManager *instance = OptionManager::get_instance();
	instance->options = options;
}

void OptionManager::apply_options()
{
	/*OptionManager *instance =*/ OptionManager::get_instance();
//	if(instance->has("tf")){
//		setFinalTime(instance->get<double>("tf"));
//	}
}

/* checks if the option is defined.
 * NOTE: does not attempt any conversion, so it is not directly related to method get<T>
 * */
bool OptionManager::has(const std::string &option)
{
	OptionManager *instance = OptionManager::get_instance();
	return instance->options.find(option) != instance->options.end();
}

std::vector<std::string> OptionManager::keys(){
	OptionManager *instance = OptionManager::get_instance();
	std::vector<std::string> ret;
	ret.reserve(instance->options.size());
	for(auto const& elem: instance->options){
	    ret.push_back(elem.first);
	}

	return ret;
}


