#include "Request.hpp"		// Request
#include <string>			// std::string
#include <map>				// std::map

Request::Request()
{
}

Request::Request(Request const &src)
{
	*this = src;
}

Request::~Request()
{
}

Request					&Request::operator=(Request const &src)
{
	this->_method = src._method;
	this->_URI = src._URI;
	this->_protocol = src._protocol;
	this->_headers = src._headers;
	this->_body = src._body;
	return *this;
}

Request::RequestMethod	Request::mapStringToRequestMethod(std::string const &str)
{
	if (str == "GET")
		return Request::GET;
	else if (str == "POST")
		return Request::POST;
	else if (str == "DELETE")
		return Request::DELETE;
	else
		return Request::OTHER;
}

Request::RequestMethod	Request::getMethod() const
{
	return this->_method;
}

std::string				Request::getURI() const
{
	return this->_URI;
}

std::string				Request::getProtocol() const
{
	return this->_protocol;
}

std::string				Request::getHeader(std::string const &headerName)
{
	std::string		headerValue;
	if (this->_headers.count(headerName))
		headerValue = this->_headers[headerName];
	else
		headerValue = std::string("");
	return headerValue;
}

std::string				Request::getBody() const
{
	return this->_body;
}

void					Request::setMethod(RequestMethod method)
{
	this->_method = method;
}

void					Request::setURI(std::string const &URI)
{
	this->_URI = URI;
}

void					Request::setProtocol(std::string const &protocol)
{
	this->_protocol = protocol;
}

void					Request::setHeader(std::string const &header)
{
	size_t			colonPosition;
	std::string		headerName;
	std::string		headerValue;

	colonPosition = header.find(":");
	headerName = header.substr(0, colonPosition);
	headerValue = header.substr(colonPosition + 2);
	this->_headers[headerName] = headerValue;
}

void					Request::setBody(std::string const &body)
{
	this->_body = body;
}
