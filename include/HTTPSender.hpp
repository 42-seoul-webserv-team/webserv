#ifndef HTTPSENDER_HPP
#define HTTPSENDER_HPP

#include "Response.hpp"
#include <ctime>
#include <sys/socket.h>

class HTTPSender
{
	private:

	public:
		HTTPSender();
		~HTTPSender();

		std::string	getDate(void);
		void		sendMessage(int sockfd, const Response &response);
		std::string	makeMessage(Response response);

		void	sendMessage(int sockfd, std::string location, std::string serverName); // redirection
		std::string	makeMessage(std::string location, std::string serverName);
};
#endif
