#include "Config.hpp"
#include "StringUtils.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

Config::~Config()
{
	// std::cout << "Destructor called (Config)" << std::endl;
}

Config::Config() : _limitClientBodySize(0)
{
	// std::cout << "Constructor called (Config)" << std::endl;
	_httpMethods[GET] = false;
	_httpMethods[POST] = false;
	_httpMethods[DELETE] = false;
	_autoindex = false;
}

Config::Config(const Config& src)
{
	// std::cout << "Copy Constructor called (Config)" << std::endl;
	*this = src;
}

Config&	Config::operator=(const Config& src)
{
	this->_limitClientBodySize = src.getLimitClientBodySize();
	this->_root = src.getRoot();
	this->_index = src.getIndex();
	this->_autoindex = src.getAutoindex();
	this->_errorPage = src.getMapErrorPages();
	this->_httpMethods[GET] = src.getHttpMethods(GET);
	this->_httpMethods[POST] = src.getHttpMethods(POST);
	this->_httpMethods[DELETE] = src.getHttpMethods(DELETE);
	this->_cgi = src.getMapCgi();
	this->_uploadDir = src.getUploadDir();
	this->_amountErrorpages = src.getAmountErrorPages();
	this->_amountCgi = src.getAmountCgi();
	return (*this);
}

size_t  Config::getLimitClientBodySize() const
{
	return (this->_limitClientBodySize);
}

const std::string&	Config::getRoot() const
{
	return (this->_root);
}

const std::vector<std::string>&	    Config::getIndex() const
{
	return (this->_index);
}

bool    Config::getAutoindex() const
{
	return (this->_autoindex);
}

bool	Config::getHttpMethods(enum e_httpMethods method) const
{
	return (this->_httpMethods[method]);
}

const std::string&	Config::getErrorPage(size_t errorNumber) const
{
	std::map<size_t, std::string>::const_iterator	it = this->_errorPage.find(errorNumber);
	return (it->second);
}

bool	Config::hasErrorPage(size_t errorNumber) const
{
	std::map<size_t, std::string>::const_iterator	it = this->_errorPage.find(errorNumber);
	if (it == this->_errorPage.end())
		return (false);
	return (true);
}

bool	Config::hasCgi() const
{
	return (this->_cgi.size());
}

bool	Config::newHasCgi(const std::string& string) const
{
	std::map<std::string, std::string>::const_iterator	it = this->_cgi.begin();
	std::map<std::string, std::string>::const_iterator	itEnd = this->_cgi.end();

	while (it != itEnd)
	{
		std::string tmp = it->first;
		size_t	len = tmp.length();
		if (string.length() < len)
		{
			it++;
			continue;
		}
		size_t	start = string.length() - len;
		std::string substr = string.substr(start, len);
		if (substr == tmp)
			return (true);
		it++;
	}
	return (false);
}

const std::string	Config::newGetCgi(const std::string& string)
{
	for (std::map<std::string, std::string>::const_iterator	it = this->_cgi.begin(); it != this->_cgi.end(); it++)
	{
		std::string tmp = it->first;
		size_t	len = tmp.length();
		if (string.length() < len)
		{
			it++;
			continue;
		}
		size_t	start = string.length() - len;
		std::string substr = string.substr(start, len);
		if (substr == tmp)
			return (it->second);
		it++;
	}
	return ("");
}

const std::string	Config::getCgi(const std::string& ext) const
{
	std::map<std::string, std::string>::const_iterator	it = this->_cgi.find(ext);
	if (it == this->_cgi.end())
		return ("");
	return (it->second);
}

const std::string&  Config::getUploadDir() const
{
	return (this->_uploadDir);
}

size_t	Config::getAmountErrorPages() const
{
	return (this->_amountErrorpages);
}

size_t	Config::getAmountCgi() const
{
	return (this->_amountCgi);
}

const std::map<size_t, std::string>&	Config::getMapErrorPages() const
{
	return (this->_errorPage);
}

const std::map<std::string, std::string>&	Config::getMapCgi() const
{
	return (this->_cgi);
}

void	Config::setLimitClientBodySize(const std::string& str)
{
	std::stringstream			ss;
	size_t						maxBodySize;
	std::vector<std::string>	vec;
	size_t						multiplyBytes = 1;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() == 1)
	{
		this->_limitClientBodySize = 0;
		return;
	}
	else if (vec.size() > 2)
		throw std::runtime_error("configfile: bad formatting client_max_body_size");
	size_t pos = vec[1].find_first_not_of("1234567890", 0);
	if (pos != std::string::npos)
	{
		if (vec[1][pos] == 'k')
			multiplyBytes = 1000;
		else if (vec[1][pos] == 'm')
			multiplyBytes = 1000000;
		else if (vec[1][pos] == 'g')
			multiplyBytes = 1000000000;
		else
			throw std::runtime_error("configfile: client_max_body_size must contain digits only");
		pos++;
		if (vec[1][pos] != '\0')
			throw std::runtime_error("configfile: client_max_body_size must contain digits only");
	}
	ss << vec[1];
	ss >> maxBodySize;
	this->_limitClientBodySize = maxBodySize * multiplyBytes;
	if (this->_limitClientBodySize < maxBodySize)
		throw std::runtime_error("configfile: client_max_body_size is too large");
}

void	Config::setRoot(const std::string& str)
{
	std::vector<std::string> vec;
	StringUtils::split(str, " \t\n", vec);
	if (vec.size() != 2)
		throw std::runtime_error("configfile: root bad formatting");
	this->_root = vec[1];
}

void	Config::setIndex(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() == 1)
	{
		this->_index.reserve(1);
		this->_index[0] = "";
		return;
	}
	else
	{
		vec.erase(vec.begin());
		this->_index = vec;
	}
}

void	Config::setAutoindex(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	for (size_t j = 0; j < vec[0].size(); j++)
		vec[0][j] = std::tolower(vec[0][j]);
	if (!vec[0].compare("on"))
		this->_autoindex = true;
	else if (!vec[0].compare("off"))
		this->_autoindex = false;
	else
		throw std::runtime_error("configfile: bad autoindex");
}

void	Config::setHttpMethods(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() == 1)
		return;
	for (size_t i = 1; i < vec.size(); i++)
	{
		for (size_t j = 0; j < vec[i].size(); j++)
			vec[i][j] = std::toupper(vec[i][j]);
		if (!vec[i].compare("GET"))
			this->_httpMethods[GET] = true;
		else if (!vec[i].compare("POST"))
			this->_httpMethods[POST] = true;
		else if (!vec[i].compare("DELETE"))
			this->_httpMethods[DELETE] = true;
		else
			throw std::runtime_error("configfile: unknown httpmethod");
	}
}

void	Config::setCgi(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() != 3)
		throw std::runtime_error("configfile: cgi bad formatting");
	this->_cgi.insert(std::pair<std::string, std::string>(vec[1], vec[2]));
}

void	Config::setUploadDir(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() != 2)
		throw std::runtime_error("configfile: upload dir bad formatting");
	this->_uploadDir = vec[1];
}

void	Config::setErrorPage(const std::string& str)
{
	std::vector<std::string>	vec;
	std::stringstream			ss;
	size_t						errorNum;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() != 3)
		throw std::runtime_error("configfile: bad formatting errorpage");
	ss << vec[1];
	ss >> errorNum;
	this->_errorPage.insert(std::pair<size_t, std::string>(errorNum, vec[2]));
}
