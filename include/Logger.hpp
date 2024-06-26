#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <fstream>
# include <exception>
# include <ctime>
# include "ft.hpp"

# define DEFAULT_ACCESS_LOG "access.log"
# define DEFAULT_ERROR_LOG "error.log"

enum eOpenFail
{
	NOT,
	ACCESS,
	ERROR,
	BOTH,
};

class Logger
{
	private:
		std::string		mALogFile;
		std::string		mELogFile;
		std::ofstream	mAccess;
		std::ofstream	mError;

		void getTime(std::ofstream & os);

	public:
		Logger(void);
		~Logger(void);

		eOpenFail open(void);
		void close(void);

		void setAccessLogFile(std::string const & file);
		void setErrorLogFile(std::string const & file);

		void putAccess(std::string const & log);
		void putError(std::string const & log);
};

#endif
