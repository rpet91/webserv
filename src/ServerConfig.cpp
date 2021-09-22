#include "ServerConfig.hpp"
#include <iostream>

Serverconfig::~Serverconfig()
{
}

Serverconfig::Serverconfig()
{
}

Serverconfig::Serverconfig(std::string data) :
	_rawInput(data)
{
	std::cout << "constr called" << std::endl;
	(void)_portNumber;
	(void)_limitClientBodySize;
}

Serverconfig::Serverconfig(Serverconfig const& src)
{
	(void)src;
	*this = src;
}

Serverconfig const& Serverconfig::operator=(Serverconfig const& src)
{
	(void)src;
	this->_rawInput = src._rawInput;
	return (*this);
}

std::ostream&	operator<<(std::ostream& os, Serverconfig const& sc)
{
	(void)os;
	(void)sc;
	os << sc._rawInput << std::endl;
	return (os);
}
