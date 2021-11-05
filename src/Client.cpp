#include "Client.hpp"			// Client
#include "Request.hpp"			// Request
#include "Response.hpp"			// Response
#include <strings.h>			// bzero
#include <arpa/inet.h>			// inet
#include <string>				// std::string

// debug
#include <iostream>

Client::Client() : _request(Request()), _response(Response())
{
}

Client::Client(Client const &src)
{
	*this = src;
}

Client			&Client::operator=(Client const &src)
{
	this->fd = src.fd;
	this->_request = src._request;
	this->_response = src._response;
	this->_address = src._address;
	this->_incomingMessage = src._incomingMessage;
	return *this;
}

Client::~Client()
{
}

Client::Client(int socketFD, struct sockaddr *addressInfo) : fd(socketFD), _request(Request()), _response(Response())
{
	char	addressBuffer[1024];

	bzero(addressBuffer, 1024);
	inet_ntop(AF_INET, &addressInfo, addressBuffer, 1024);
	this->_address.append(addressBuffer);
}

Request			&Client::getRequest()
{
	return this->_request;
}

Response		&Client::getResponse()
{
	return this->_response;
}

std::string		&Client::getAddress()
{
	return this->_address;
}

std::string const	&Client::getIncomingMessage()
{
	return this->_incomingMessage;
}

void			Client::setIncomingMessage(std::string const &message)
{
	this->_incomingMessage.append(message);
}
