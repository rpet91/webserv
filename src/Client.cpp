#include "Client.hpp"			// Client
#include "Request.hpp"			// Request
#include "Response.hpp"			// Response


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
	return *this;
}

Client::~Client()
{
}

Client::Client(int socketFD) : fd(socketFD), _request(Request()), _response(Response())
{
}

Request			&Client::getRequest()
{
	return this->_request;
}

Response		&Client::getResponse()
{
	return this->_response;
}
