#include "Response.hpp"	// Response
#include <sstream>		// std::stringstream

/*
 * Default constructor.
 */
Response::Response() : _body(std::string()), _headers(std::string()), _statusCode(200), _defaultError(false)
{
}

/*
 * Copy constructor.
 */
Response::Response(Response const &src)
{
	*this = src;
}

/*
 * Assignment operator.
 */
Response		&Response::operator=(Response const &src)
{
	this->_body = src._body;
	this->_headers = src._headers;
	this->_statusCode = src._statusCode;
	this->_defaultError = src._defaultError;
	return *this;
}

/*
 * Destructor.
 */
Response::~Response()
{
}

/*
 * This function will reset a response to its basic settings.
 */
void			Response::reset()
{
	this->_body = "";
	this->_headers = "";
	this->_statusCode = 200;
	this->_defaultError = false;
}


/*
 * This function sets the _body variable.
 */
void			Response::setBody(std::string const &body)
{
	this->_body = body;
}

/*
 * This function sets a header for the response.
 */
void			Response::setHeader(std::string const &header)
{
	this->_headers += header + "\r\n";
}

/*
 * This function sets the status code for the response.
 */
void			Response::setStatusCode(int code)
{
	this->_statusCode = code;
}

/*
 * This function sets the _defaultError variable to true.
 */
void			Response::setDefaultError()
{
	this->_defaultError = true;
}

/*
 * This function returns the body of the response.
 */
std::string		Response::getBody()
{
	return this->_body;
}

/*
 * This function builds and returns the headers for the response.
 */
std::string		Response::getHeaders()
{
	return this->_headers;
}

/*
 * This function returns the status code for the response.
 */
int				Response::getStatusCode()
{
	return this->_statusCode;
}

/*
 * this function returns true if the _defaultError variable has been set.
 */
bool			Response::isDefaultError()
{
	return this->_defaultError;
}

/*
 * this function formats and returns the text that will be sent to the client.
 */
std::string		Response::getResponseText(std::string const &statusText)
{
	std::stringstream		result;

	result << "HTTP/1.1 " << this->_statusCode << " " << statusText << "\r\n";
	result << this->_headers << "\r\n";
	result << this->_body;

	return result.str();
}
