#include "Logger.hpp"

Logger::Logger(void)
{
	this->mALogFile = DEFAULT_ACCESS_LOG;
	this->mELogFile = DEFAULT_ERROR_LOG;
}

Logger::~Logger(void)
{
	this->close();
}

eOpenFail Logger::open(void)
{	
	if (!this->mAccess.is_open())
		this->mAccess.open(this->mALogFile, std::fstream::app);
	if (!this->mError.is_open())
		this->mError.open(this->mELogFile, std::fstream::app);

	bool access = this->mAccess.is_open();
	bool error = this->mError.is_open();

	if (!access && !error)
		return BOTH;
	else if (!access)
		return ACCESS;
	else if (!error)
		return ERROR;
	return NOT;
}

void Logger::close(void)
{
	if (this->mAccess.is_open())
		this->mAccess.close();
	if (this->mError.is_open())
		this->mError.close();
}

void Logger::setAccessLogFile(std::string const & file)
{
	this->mALogFile = file;
}

void Logger::setErrorLogFile(std::string const & file)
{
	this->mELogFile = file;
}

void Logger::getTime(std::ofstream & os)
{
	time_t		t = std::time(NULL);
	struct tm 	*time = std::localtime(&t);

	os << "[" << ft::toString(time->tm_year + 1900, 10);
	os << "/" << std::setfill('0') << std::setw(2) << ft::toString(time->tm_mon + 1, 10);
	os << "/" << std::setfill('0') << std::setw(2) << ft::toString(time->tm_mday, 10);
	os << ":" << std::setfill('0') << std::setw(2) << ft::toString(time->tm_hour, 10);
	os << ":" << std::setfill('0') << std::setw(2) << ft::toString(time->tm_min, 10);
	os << ":" << std::setfill('0') << std::setw(2) << ft::toString(time->tm_sec, 10) << "]";
}

void Logger::putAccess(std::string const & log)
{
	if (!this->mAccess.is_open())
		return;
	this->getTime(this->mAccess);
	this->mAccess << " " << log << "\n";

}

void Logger::putError(std::string const & log)
{
	if (!this->mError.is_open())
		return;
	this->getTime(this->mError);
	this->mError << " " << log << "\n";
}
