#pragma once
#ifndef __MANAGER_EXCEPTION_H__
#define __MANAGER_EXCEPTION_H__

#include <string>
#include <stdexcept>

class ManagerException : public std::runtime_error
{
public:
	ManagerException(std::string const errorMsg);
	~ManagerException() throw();
};

#endif