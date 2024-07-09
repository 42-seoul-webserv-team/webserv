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
		void		sendMessage(int sockfd, const Response &response);
		std::string	makeMessage(Response response);

		void	sendMessage(int sockfd, std::string location, std::string serverName);
		std::string	makeMessage(std::string location, std::string serverName);
};
#endif
