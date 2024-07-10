#ifndef HTTPSENDER_HPP
#define HTTPSENDER_HPP

#include <sys/socket.h>

#include <ctime>

#include "value.hpp"
#include "Response.hpp"

class HTTPSender
{
	private:

	public:
		HTTPSender();
		~HTTPSender();

		std::string	getDate(void);
		std::string	sendMessage(int sockfd, const Response &response);
		std::string	makeMessage(Response response);

		std::string	sendMessage(int sockfd, std::string const & message);
		std::string	sendMessage(int sockfd, std::string location, std::string serverName);
		std::string	makeMessage(std::string location, std::string serverName);
};
#endif
