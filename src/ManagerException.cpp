#include "ManagerExcption.hpp"
#include <string>
#include <stdexcept>

ManagerException::ManagerException(std::string const errorMsg)
	: std::runtime_error(errorMsg.c_str())
{}

ManagerException::~ManagerException()
{}