#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "StringUtils.hpp"

LocationConfig::~LocationConfig()
{
}

LocationConfig::LocationConfig()
{
}

LocationConfig::LocationConfig(const LocationConfig& src)
{
	*this = src;
}

LocationConfig&	LocationConfig::operator=(const LocationConfig& src)
{
	this->_path = src._path;
	this->_root = src._root;
	this->_limitClientBodySize = src._limitClientBodySize;
	this->_index = src._index;
	this->_autoindex = src._autoindex;
	memcpy(this->_httpMethods, src._httpMethods, sizeof(this->_httpMethods));
	memcpy(this->_cgi, src._cgi, sizeof(this->_cgi));
	return (*this);
}

void	LocationConfig::init(const std::string& data, size_t body, bool autoindex, bool methods[3])
{
	std::vector<std::string>	tokens;
	std::vector<std::string>	lines;

	this->_httpMethods[GET] = methods[GET];
	this->_httpMethods[POST] = methods[POST];
	this->_httpMethods[DELETE] = methods[DELETE];
	this->_limitClientBodySize = body;
	this->_autoindex = autoindex;

	StringUtils::split(data, " \t\n", tokens);
	if (tokens.size() != 3)
		throw std::runtime_error("configfile: location has bad formatting");
	this->_path = tokens[1];
	StringUtils::trimBraces(tokens[2]);
	StringUtils::split(tokens[2], ";", lines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::string identifier;
		StringUtils::matchIdentifier(lines[i], identifier);
		if (!identifier.compare("root"))
			this->setRoot(lines[i]);
		else if (!identifier.compare("client_max_body_size"))
			this->setLimitClientBodySize(lines[i]);
		else if (!identifier.compare("index"))
			this->setIndex(lines[i]);
		else if (!identifier.compare("autoindex"))
			this->setAutoindex(lines[i]);
		else if (!identifier.compare("limit_except"))
			this->setHttpMethods(lines[i]);
		else if (!identifier.compare("cgi"))
			this->setCgi(lines[i]);
		else
			throw std::runtime_error("configfile: unknown identifier in location");
	}
	return;
}

void	LocationConfig::setRoot(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	this->_root = vec[1];
}

void	LocationConfig::setLimitClientBodySize(const std::string& str)
{
	std::vector<std::string>	vec;
	std::stringstream			ss;
	size_t						maxBodySize;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	ss << vec[0];
	ss >> maxBodySize;
	this->_limitClientBodySize = maxBodySize;
}

void	LocationConfig::setIndex(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	this->_index = vec;
}

void	LocationConfig::setAutoindex(const std::string& str)
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

void	LocationConfig::setHttpMethods(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
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

void	LocationConfig::setCgi(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	this->_cgi[EXTENSION] = vec[1];
	this->_cgi[PATH] = vec[2];
}

const std::string&	LocationConfig::getPath() const
{
	return (this->_path);
}

const std::string&	LocationConfig::getRoot() const
{
	return (this->_root);
}

size_t	LocationConfig::getLimitClientBodySize() const
{
	return (this->_limitClientBodySize);
}

const std::vector<std::string>&		LocationConfig::getIndex() const
{
	return (this->_index);
}

bool	LocationConfig::getAutoindex() const
{
	return (this->_autoindex);
}

bool	LocationConfig::getHttpMethods(enum e_httpMethods method) const
{
	return (this->_httpMethods[method]);
}

const std::string&	LocationConfig::getCgi(enum e_cgi arg) const
{
	return (this->_cgi[arg]);
}

std::ostream &operator<<(std::ostream& out, const LocationConfig& loc)
{
	std::cout << "Printing LocationConfig:" << std::endl;
	out << "Path: " << loc.getPath() << std::endl;
	out << "Root: " << loc.getRoot() << std::endl;
	out << "LimitClientBodySize: " << loc.getLimitClientBodySize() << std::endl;
	printVec(loc.getIndex());
	out << "AutoIndex: " << loc.getAutoindex() << std::endl;
	out << "GET: " << loc.getHttpMethods(GET) << std::endl;
	out << "POST: " << loc.getHttpMethods(POST) << std::endl;
	out << "DELETE: " << loc.getHttpMethods(DELETE) << std::endl;
	out << "Cgi ext.: " << loc.getCgi(EXTENSION) << std::endl;
	out << "Cgi path: " << loc.getCgi(PATH) << std::endl;
	return (out);
}
