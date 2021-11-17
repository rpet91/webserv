#include "Request.hpp"		// Request
#include <string>			// std::string
#include <map>				// std::map


#include <iostream> //debug

/*
 * Default constructor.
 */
Request::Request() : _bodyLength(0)
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
	this->_queryString = src._queryString;
	this->_protocol = src._protocol;
	this->_headers = src._headers;
	this->_body = src._body;
	this->_matchedServerName = src._matchedServerName;
	this->_bodyLength = src._bodyLength;
	return *this;
}

/*
 * This function will reset a request to its basic settings.
 */
void				Request::reset()
{
	this->_method.clear();
	this->_URI.clear();
	this->_queryString.clear();
	this->_protocol.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_matchedServerName.clear();
	this->_bodyLength = 0;
}

/*
 * This function returns the method as a RequestMethod enum.
 */
std::string				Request::getMethod() const
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
 * This function returns the query string of the request.
 */
std::string				Request::getQueryString() const
{
	return this->_queryString;
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
	std::string		lowerCaseHeaderName;

	for (std::string::size_type i = 0; i < headerName.size(); i++)
		lowerCaseHeaderName += std::tolower(headerName[i]);
	if (this->_headers.count(lowerCaseHeaderName))
		headerValue = this->_headers[lowerCaseHeaderName];
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
 * This funtion returns the matched server name of the request.
 */
std::string				Request::getMatchedServerName() const
{
	return this->_matchedServerName;
}

/*
 * This functions returns the port of the request.
 */
std::string				Request::getPort()
{
	std::string		port = getHeader("Host");
	size_t			colonPosition = port.find(":");

	if (colonPosition == std::string::npos)
		return "80";
	port = port.substr(colonPosition + 1);
	return port;
}

/*
 * This function returns the length of the requested body.
 */
size_t					Request::getBodyLength() const
{
	return this->_bodyLength;
}

/*
 * This function sets the method of the request.
 */
void					Request::setMethod(std::string const &method)
{
	this->_method = method;
}

/*
 * This function sets the URI of the request.
 */
void					Request::setURI(std::string const &URI)
{
	size_t			questionMarkPosition;

	questionMarkPosition = URI.find("?");
	if (questionMarkPosition != std::string::npos)
	{
		std::cout << "Ik vond een vraagtekel." << std::endl;
		this->setQueryString(URI.substr(questionMarkPosition + 1));
	}
	this->_URI = URI.substr(0, questionMarkPosition);
	std::cout << ">>>    The URI is now [" << this->_URI << "]" << std::endl;
}

/*
 * This function sets the query string of the request.
 */
void					Request::setQueryString(std::string const &queryString)
{
	this->_queryString = queryString;
	std::cout << ">>>    The query is now [" << this->_queryString << "]" << std::endl;
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
	std::string		lowerCaseHeaderName;
	std::string		headerValue;

	colonPosition = header.find(":");
	if (colonPosition == std::string::npos)
		return ;
	headerName = header.substr(0, colonPosition);
	for (std::string::size_type i = 0; i < headerName.size(); i++)
		lowerCaseHeaderName += std::tolower(headerName[i]);
	headerValue = header.substr(colonPosition + 2);
	this->_headers[lowerCaseHeaderName] = headerValue;
}

/*
 * This function sets the body of the request.
 */
void					Request::setBody(std::string const &body)
{
	this->_body = body;
}

/*
 * This function sets the matched server name of the request.
 */
void					Request::setMatchedServerName(std::string const &matchedServerName)
{
	this->_matchedServerName = matchedServerName;
}

/*
 * This function keeps track of the length of the requested body of the client.
 */
void					Request::modifyBodyLength(int length)
{
	this->_bodyLength += length;
}
