#include "globals.h"

std::string global_private_readingBackend = "";
std::string global_private_loggingBackend = "";


void set_reading_backend(const std::string &value)
{
	global_private_readingBackend = value;
}

std::string get_reading_backend()
{
	return global_private_readingBackend;
}

void set_logging_backend(const std::string &value)
{
	global_private_loggingBackend = value;
}

std::string get_logging_backend()
{
	return global_private_loggingBackend;
}
