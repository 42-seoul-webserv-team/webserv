#include "HTTPSender.hpp"

HTTPSender::HTTPSender()
{}

HTTPSender::~HTTPSender()
{}

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
	std::string	message;

	message.reserve(800);
	message += "HTTP/1.1 ";
	message += response.getCode() + " ";
	message += response.getStatusMsg() + CRLF;

	message += this->getDate();
	message += "Server: " + response.getServerName() + CRLF;
	message += "Connection: Closed\r\n";

	message += "Content-Length: " + response.getContentLength() + CRLF;
	message += "Content-Type: " + response.getContentType() + CRLF;

	message += CRLF;

	message += response.getBody();

	return (message);
}

std::string HTTPSender::sendMessage(int sockfd, std::string const & message)
{
	int length = send(sockfd, message.c_str(), message.size(), 0);
	if (length <= 0)
		return message;
	
	if (length < static_cast<int>(message.size()))
		return message.substr(length);
	else
		return "";
}

std::string	HTTPSender::sendMessage(int sockfd, const Response &response)
{
	std::string	message = this->makeMessage(response);

	return this->sendMessage(sockfd, message);
}

std::string	HTTPSender::makeMessage(std::string location, std::string serverName)
{
	std::string message;

	message.reserve(800);

	message += "HTTP/1.1 301 Moved Permanently\r\n";
	message += "Server: " + serverName + CRLF;
	message += this->getDate();
	message += "Location: " + location + CRLF;
	message += CRLF;

	return (message);
}

std::string	HTTPSender::sendMessage(int sockfd, std::string location, std::string serverName)
{
	std::string	message = this->makeMessage(location, serverName);

	return this->sendMessage(sockfd, message);
}
