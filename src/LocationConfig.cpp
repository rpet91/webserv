#include "Config.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "StringUtils.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

LocationConfig::~LocationConfig()
{
	// std::cout << "Destructor called (LocationConfig)" << std::endl;
}

LocationConfig::LocationConfig()
{
	// std::cout << "Constructor called (LocationConfig)" << std::endl;
}

LocationConfig::LocationConfig(const LocationConfig& src)
{
	// std::cout << "Copy Constructor called (LocationConfig)" << std::endl;
	*this = src;
}

LocationConfig::LocationConfig(const ServerConfig& src)
{
	// std::cout << "Copy Constructor called (LocationConfig)" << std::endl;
	*this = src;
}

LocationConfig&	LocationConfig::operator=(const ServerConfig& src)
{
	//	Config variables
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

LocationConfig&	LocationConfig::operator=(const LocationConfig& src)
{
	//	LocationConfig variables
	this->_path = src._path;

	//	Config variables
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

void	LocationConfig::init(const std::string& data)
{
	std::vector<std::string>	tokens;
	std::vector<std::string>	lines;

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
		if (identifier == "root")
			this->setRoot(lines[i]);
		else if (identifier == "client_max_body_size")
			this->setLimitClientBodySize(lines[i]);
		else if (identifier == "index")
			this->setIndex(lines[i]);
		else if (identifier == "autoindex")
			this->setAutoindex(lines[i]);
		else if (identifier == "limit_except")
			this->setHttpMethods(lines[i]);
		else if (identifier == "cgi")
			this->setCgi(lines[i]);
		else if (identifier == "upload")
			this->setUploadDir(lines[i]);
		else
			throw std::runtime_error("configfile: unknown identifier in location");
	}
}

void	LocationConfig::setPath(const std::string& str)
{
	this->_path = str;
}

const std::string&	LocationConfig::getPath() const
{
	return (this->_path);
}

std::ostream &operator<<(std::ostream& out, const LocationConfig& loc)
{
	out << "Printing LocationConfig:" << std::endl;
	out << "LocaPath: " << loc.getPath() << std::endl;
	out << "LocaRoot: " << loc.getRoot() << std::endl;
	out << "BodySize: " << loc.getLimitClientBodySize() << std::endl;
	out << "indexVec: "; for (size_t i = 0; i < loc.getIndex().size(); i++){out << loc.getIndex()[i] << " ";} out << std::endl;
	out << "Autoinde: " << loc.getAutoindex() << std::endl;
	out << "httpGET : " << loc.getHttpMethods(GET) << std::endl;
	out << "httpPOST: " << loc.getHttpMethods(POST) << std::endl;
	out << "httpDEL : " << loc.getHttpMethods(DELETE) << std::endl;
	out << "Upld Dir: " << loc.getUploadDir() << std::endl;
	out << "boolcgi : " << loc.hasCgi() << std::endl;
	out << "Cgi     : " << loc.getCgi(".php") << " | " << loc.getCgi(".py") << std::endl;
	return (out);
}
