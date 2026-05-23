#ifndef __UTILS_GLOBALS_H__
#define __UTILS_GLOBALS_H__

#include <string>


void set_reading_backend(const std::string&);
std::string get_reading_backend();

void set_logging_backend(const std::string&);
std::string get_logging_backend();

#endif
