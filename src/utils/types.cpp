#include <istream>

#include "types.h"


std::istream &operator>>(
		std::istream &is,
		std::vector<std::string> &v)
{
	char c;
	std::string value;
	bool next = false;
	v = std::vector<std::string>();

	if(!(is >> c) || c != '{')
	{
		is.setstate(std::ios::failbit);
		return is;
	}

	while(true)
	{
		std::stringstream ss;

		while(is >> c && c != ',' && c != '}')
		{
			ss << c;
			next = false;
		}

		if(is.fail())
			break;
		else if(c == ',')
		{
			v.push_back(ss.str());
			next = true;
		}
		else if(c == '}' and next)
		{
			is.setstate(std::ios::failbit);
			break;
		}
		else if(c == '}')
		{
			if(ss.str().size() > 0)
				v.push_back(ss.str());
			break;
		}
	}

	return is;
}
