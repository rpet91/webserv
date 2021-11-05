#include "CGI.hpp"			// CGI

/*
 * Default constructor
 */
CGI::CGI()
{
}

/*
 * Copy constructor
 */
CGI::CGI(CGI const &src)
{
	*this = src;
}

/*
 * Assignment operator
 */
CGI			&CGI::operator=(CGI const &src)
{
	this->fdin[0] = src.fdin[0];
	this->fdin[1] = src.fdin[1];
	this->fdout[0] = src.fdout[0];
	this->fdout[1] = src.fdout[1];
	return *this;
}

/*
 * Destructor
 */
CGI::~CGI()
{
}
