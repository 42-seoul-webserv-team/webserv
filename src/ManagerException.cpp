#include "ManagerExcption.hpp"

ManagerException::ManagerException(std::string const errorMsg)
	: std::runtime_error(errorMsg.c_str())
{}

ManagerException::~ManagerException() throw()
{}