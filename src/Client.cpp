#include "Client.hpp"
#include <string>


// debug
#include <iostream>

Client::Client()
{
}

Client::Client(Client const &src)
{
	*this = src;
}

Client		&Client::operator=(Client const &src)
{
	this->fd = src.fd;
	this->_request = src._request;
	this->_response = src._response;
	return *this;
}

Client::~Client()
{
}

Client::Client(int socketFD) : fd(socketFD)
{
}

std::string		&Client::getRequest()
{
	return this->_request;
}

void			Client::setRequest(std::string const &message)
{
	this->_request = message;
}
