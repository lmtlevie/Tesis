#ifndef __UTILS_TYPES_H__
#define __UTILS_TYPES_H__

#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <typeinfo>

template<class T>
T to(const std::string &str)
{
	std::string trimmed = str;
	trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), ' '), trimmed.end());
	std::stringstream sstr(trimmed);
	T value;

	sstr >> value;

	if(sstr.fail()){
		throw std::runtime_error("types.h::to<t>(std::string) - Unable to convert std:string '" + str + "'. When attempting to read as type: '" + typeid(T).name() + "'");
	}

	return value;
}

template<class T>
bool check_conversion_to(const std::string &str)
{
	std::string trimmed = str;
	trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), ' '), trimmed.end());
	std::stringstream sstr(trimmed);
	T value;

	sstr >> value;

	return !sstr.fail();
}

template<class T>
std::istream &operator>>(
		std::istream &is,
		std::vector<T> &v){
	char c;
	T value;
	bool next = false;

	c = is.peek();
	if(!c) {
		is.setstate(std::ios::failbit);
		return is;
	}

	if (c != '{' &&  c != '[')	{ // base case, where there is a single element and no '{' and '}' are used
		// parse value directly and add it vector
		is >> value;
		v.push_back(value);

		// verify we are at the end of the stream
		if(!is.eof()){
			is.setstate(std::ios::failbit);
		}

		return is;
	}

	// first character (c) is '{', we can advance the stream and parse until end
	is.get();
	while(is)	{
		c = is.peek();

		// check finish conditions
		if((c == '}' or c == ']') and !next){
			break;
		} else if((c == '}' or c == ']')and next)		{
			is.setstate(std::ios::failbit);
			break;
		}

		// parse value and add it to the vector
		is >> value;
		v.push_back(value);

		// continue with next value if necessary
		c = is.peek();
		if(c == ',')		{
			next = true;
			is.get();
		}
		else next = false;
	}

	return is;
}

std::istream &operator>>(
		std::istream&,
		std::vector<std::string>&);

#endif
