#include "Location.hpp"
#include <iostream>

Location::Location(void)
{
	this->mAutoindex = false;
	this->mMethod.push_back(GET);
	this->mMethod.push_back(POST);
	this->mMethod.push_back(DELETE);
	this->mMethod.push_back(HEAD);
}

Location::~Location(void)
{
	this->mMethod.clear();
	this->mCgi.clear();
}

std::vector<std::string> Location::getPath(void)
{
	return this->mPath;
}

std::string Location::getIndex(void)
{
	return this->mIndex;
}

std::string Location::getRoot(void)
{
	return this->mRoot;
}

std::string Location::getRedirect(void)
{
	return this->mRedirect;
}

std::string Location::getUpload(void)
{
	return this->mUpload;
}

std::string Location::getCGI(std::string const & url)
{
	if (url.empty())
		return "";

	size_t pos = url.rfind('.');
	if (pos == std::string::npos || pos == 0)
		return "";

	std::string extension = url.substr(pos + 1);
	return this->mCgi[extension];
}

bool Location::checkMethod(eMethod reqMethod)
{
	for (std::vector<eMethod>::iterator it = this->mMethod.begin();
			it != this->mMethod.end(); it++)
	{
		if (*it == reqMethod)
			return true;
	}
	return false;
}

bool Location::checkAutoindex(void)
{
	return this->mAutoindex;
}

void Location::setPath(std::vector<std::string> & path)
{
	this->mPath = path;
}

void Location::setIndex(std::string const & indexfile)
{
	this->mIndex = indexfile;
}

void Location::setRoot(std::string const & rootpath)
{
	this->mRoot = rootpath;
}

void Location::addMethod(eMethod method)
{
	for (std::vector<eMethod>::iterator it = this->mMethod.begin();
			it != this->mMethod.end(); it++)
	{
		if (*it == method)
			return ;
	}
	this->mMethod.push_back(method);
}

void Location::setRedirect(std::string const & redirection)
{
	this->mRedirect = redirection;
}

void Location::addCgi(std::string const & extension, std::string const & cgi)
{
	if (this->mCgi.end() == this->mCgi.find(extension))
		this->mCgi[extension] = cgi;
}

void Location::setAutoindex(bool option)
{
	this->mAutoindex = option;
}

void Location::setUpload(std::string const & dir)
{
	this->mUpload = dir;
}

void Location::clearMethod(void)
{
	this->mMethod.clear();
}

bool Location::checkIndexFile(std::vector<std::string> & url)
{
	if (this->mIndex.empty() || this->mPath.size() != url.size())
		return false;

	for (size_t i = 0; i < this->mPath.size(); i++)
	{
		if (this->mPath[i] != url[i])
			return false;
	}
	return true;
}

bool Location::checkCGI(std::string const & url)
{
	if (url.empty())
		return false;

	if (this->getCGI(url).empty())
		return false;

	return true;
}

# include <iostream>

void Location::printAll(void)
{
	std::cout << "\t* Print Location" << std::endl;
	std::cout << "\t\tPath: ";
	if (this->mPath.size() == 1)
		std::cout << "/" << this->mPath.front() << std::endl;
	else
	{
		for (size_t i = 1; i < this->mPath.size(); i++)
			std::cout << "/" << this->mPath[i];
		std::cout << std::endl;
	}
	std::cout << "\t\tIndex: " << this->mIndex << std::endl;
	std::cout << "\t\tRoot: " << this->mRoot << std::endl;
	std::cout << "\t\tAllow Method: ";
	for (size_t i = 0; i < this->mMethod.size(); i++)
	{
		switch (this->mMethod[i]) {
			case GET:
				std::cout << "GET ";
				break ;
			case POST:
				std::cout << "POST ";
				break ;
			case DELETE:
				std::cout << "DELETE ";
				break ;
			case HEAD:
				std::cout << "HEAD ";
				break ;
			default:
				break ;
		}
	}
	std::cout << std::endl;
	std::cout << "\t\tRedirect: " << this->mRedirect << std::endl;
	std::cout << "\t\tUpload path: " << this->mUpload << std::endl;
	std::cout << "\t\tCGI: {" << std::endl;
	for (std::map<std::string, std::string>::iterator it = this->mCgi.begin(); it != this->mCgi.end(); it++)
		std::cout << "\t\t\t" << it->first << ": " << it->second << std::endl;
	std::cout << "\t\t}" << std::endl;
	std::cout << "\t\tAutoindex: ";
	if (this->mAutoindex)
		std::cout << "On";
	else
		std::cout << "Off";
	std::cout << std::endl;
}
