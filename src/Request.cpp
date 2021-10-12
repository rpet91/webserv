#include "Request.hpp"		// Request
#include <string>			// std::string
#include <map>				// std::map

/*
 * Default constructor.
 */
Request::Request()
{
}

/*
 * Copy constructor.
 */
Request::Request(Request const &src)
{
	*this = src;
}

/*
 * Destructor.
 */
Request::~Request()
{
}

/*
 * Assignment operator.
 */
Request					&Request::operator=(Request const &src)
{
	this->_method = src._method;
	this->_URI = src._URI;
	this->_protocol = src._protocol;
	this->_headers = src._headers;
	this->_body = src._body;
	return *this;
}

/*
 * This function returns the method as a RequestMethod enum.
 */
Request::RequestMethod	Request::getMethod() const
{
	return this->_method;
}

/*
 * This function returns the URI of the request.
 */
std::string				Request::getURI() const
{
	return this->_URI;
}

/*
 * This function returns the protocol of the request.
 */
std::string				Request::getProtocol() const
{
	return this->_protocol;
}

/*
 * This function returns the value belonging to the requested header. Will be
 * an empty string if the header was not set.
 */
std::string				Request::getHeader(std::string const &headerName)
{
	std::string		headerValue;
	if (this->_headers.count(headerName))
		headerValue = this->_headers[headerName];
	else
		headerValue = std::string("");
	return headerValue;
}

/*
 * This function returns the body of the request.
 */
std::string				Request::getBody() const
{
	return this->_body;
}

/*
 * This function sets the method of the request.
 */
void					Request::setMethod(std::string const &method)
{
	this->_method = this->_mapStringToRequestMethod(method);
}

/*
 * This function sets the URI of the request.
 */
void					Request::setURI(std::string const &URI)
{
	this->_URI = URI;
}

/*
 * This function sets the protocol of the request.
 */
void					Request::setProtocol(std::string const &protocol)
{
	this->_protocol = protocol;
}

/*
 * This function sets a header in the request.
 */
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

/*
 * This function sets the body of the request.
 */
void					Request::setBody(std::string const &body)
{
	this->_body = body;
}

/*
 * This function maps a string to the RequestMethod enum that goes represents it.
 */
Request::RequestMethod	Request::_mapStringToRequestMethod(std::string const &str)
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
