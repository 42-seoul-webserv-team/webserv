#include "HTTPSender.hpp"

HTTPSender::HTTPSender()
{
}

HTTPSender::~HTTPSender()
{

}

std::string	HTTPSender::getDate(void)
{
	std::time_t	current = std::time(NULL);
	std::tm		*gmtTime = std::gmtime(&current);

	char	buffer[100];

	strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %X GMT\r\n", gmtTime);
	return (static_cast<std::string>(buffer));
}

std::string	HTTPSender::makeMessage(Response response)
{
	//struct tm	*newtime;
	std::string	message;

	message.reserve(800);
	// status line
	message += "HTTP/1.1 ";
	message += response.getCode() + " ";
	message += response.getStatusMsg() + CRLF;

	// header lines
	message += this->getDate();
	message += "Server: " + response.getServerName() + CRLF;
	message += "Connection: Closed\r\n";

	message += response.getContentLength() + CRLF;
	if (atoi(response.getContentLength().c_str()) != 0)
		message += response.getContentType() + CRLF;

	// Blank line
	message += CRLF;

	message += response.getBody();

	return (message);
}

void	HTTPSender::sendMessage(int sockfd, const Response &response)
{
	std::string	message = this->makeMessage(response);

	send(sockfd, message.c_str(), message.size(), 0);
}

std::string	HTTPSender::makeMessage(std::string location, std::string serverName) // redirection
{
	std::string message;

	message.reserve(800);

	message += "HTTP/1.1 301 Moved Permanently\r\n";
	message += "Location: " + location + CRLF;
	message += "Server: " + serverName + CRLF;

	message += CRLF;
	return (message);
}

void	HTTPSender::sendMessage(int sockfd, std::string location, std::string serverName) // redirection
{
	std::string	message = this->makeMessage(location, serverName);

	send(sockfd, message.c_str(), message.size(), 0);
}