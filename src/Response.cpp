#include "Response.hpp"
#include <string>

Response::Response() : _body(std::string()), _header(std::string()), _statusCode(0), _defaultError(false)
{
}

Response::Response(Response const &src)
{
	*this = src;
}

Response		&Response::operator=(Response const &src)
{
	this->_body = src._body;
	this->_header = src._header;
	this->_statusCode = src._statusCode;
	this->_defaultError = src._defaultError;
	return *this;
}

Response::~Response()
{
}

void			Response::resetResponse()
{
	this->_body = "";
	this->_header = "";
	this->_statusCode = 0;
	this->_defaultError = false;
}

void			Response::setBody(std::string const &body)
{
	this->_body = body;
}

void			Response::setHeader(std::string const &header)
{
	this->_header = header;
}

void			Response::setStatusCode(int code)
{
	this->_statusCode = code;
}

void			Response::setDefaultError()
{
	this->_defaultError = true;
}

std::string		Response::getBody()
{
	return this->_body;
}

std::string		Response::getHeader()
{
	return this->_header;
}

int				Response::getStatusCode()
{
	return this->_statusCode;
}

bool			Response::isDefaultError()
{
	return this->_defaultError;
}
