#include "ServerConfig.hpp"
#include "Config.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

ServerConfig::~ServerConfig()
{
	// std::cout << "Destructor called (ServerConfig)" << std::endl;
}

ServerConfig::ServerConfig()
{
	// std::cout << "Constructor called (ServerConfig)" << std::endl;
}

ServerConfig::ServerConfig(ServerConfig const& src) : Config::Config(src)
{
	// std::cout << "Copy Constructor called (ServerConfig)" << std::endl;
	*this = src;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& src)
{
	//	ServerConfig variables
	this->_amountLocations = src.getAmountLocations();
	this->_defaultServerName = src.getDefaultServerName();
	this->_portNumber = src.getPort();
	this->_serverNames = src.getServerNames();
	this->_locations = src._locations;

	//	Config variables
	this->_limitClientBodySize = src.getLimitClientBodySize();
	this->_root = src.getRoot();
	this->_index = src.getIndex();
	this->_autoindex = src.getAutoindex();
	this->_errorPage = src.getMapErrorPages();
	this->_httpMethods[GET] = src.getHttpMethods(GET);
	this->_httpMethods[POST] = src.getHttpMethods(POST);
	this->_httpMethods[DELETE] = src.getHttpMethods(DELETE);
	this->_CGI = src.getMapCGI();
	this->_uploadDir = src.getUploadDir();
	this->_amountErrorpages = src.getAmountErrorPages();
	this->_amountCGI = src.getAmountCGI();
	return (*this);
}

void	ServerConfig::init(std::string& str)
{
	std::vector<std::string>	lines;
	std::string					identifier;

	StringUtils::trimBraces(str);
	StringUtils::split(str, ";", lines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		StringUtils::matchIdentifier(lines[i], identifier);
		StringUtils::lowerCase(identifier);
		if (!identifier.length())
			continue;
		if (identifier == "location")
			continue;
		else if (identifier == "error_page")
			this->setErrorPage(lines[i]);
		else if (identifier == "listen")
			this->setPort(lines[i]);
		else if (identifier == "server_name")
			this->setServerName(lines[i]);
		else if (identifier == "client_max_body_size")
			this->setLimitClientBodySize(lines[i]);
		else if (identifier == "root")
			this->setRoot(lines[i]);
		else if (identifier == "index")
			this->setIndex(lines[i]);
		else if (identifier == "autoindex")
			this->setAutoindex(lines[i]);
		else if (identifier == "limit_except")
			this->setHttpMethods(lines[i]);
		else if (identifier == "cgi")
			this->setCGI(lines[i]);
		else if (identifier == "upload")
			this->setUploadDir(lines[i]);
		else
			throw std::runtime_error("configfile: unknown identifier");
	}
	for (size_t i = 0; i < lines.size(); i++)
	{
		StringUtils::matchIdentifier(lines[i], identifier);
		if (identifier == "location")
		{
			this->setLocationConfig(lines[i]);
		}
	}
	this->setDefaultServerName();
	this->_amountLocations = _locations.size();
	this->_amountErrorpages = _errorPage.size();
	this->_amountCGI = _CGI.size();
}

void	ServerConfig::setDefaultServerName()
{
	if (getServerNames().size() == 0)	//deze moet nooit throwen
		throw std::runtime_error("configfile: no default servername found");
	this->_defaultServerName = getServerNames()[0];
}

void	ServerConfig::setPort(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	for (size_t i = 1; i < vec.size(); i++)
	{
		std::stringstream	ss;
		size_t				num;
		size_t 				pos = vec[i].find_first_not_of("1234567890", 0);

		if (pos != std::string::npos)
			throw std::runtime_error("configfile: port must contain digits only");
		ss << vec[i];
		ss >> num;
		if (num > 65535)
			throw std::runtime_error("configfile: port input greater than 65535");
		this->_portNumber.push_back(num);
	}
}

void	ServerConfig::setServerName(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() <= 1)
		vec.push_back("");
	vec.erase(vec.begin());
	this->_serverNames = vec;
}

void	ServerConfig::setLocationConfig(const std::string& str)
{
	std::vector<std::string>	vec;
	LocationConfig				locationConfig;

	StringUtils::split(str, " \t\n", vec);
	if (vec.size() != 3)
		throw std::runtime_error("configfile: location bad formatting");
	locationConfig.setPath(vec[1]);
	locationConfig = *this;
	locationConfig.init(str);
	this->_locations.insert(std::pair<std::string, LocationConfig>(vec[1], locationConfig));
}

const std::string&	ServerConfig::getDefaultServerName() const
{
	return (this->_defaultServerName);
}

const std::vector<size_t>&	ServerConfig::getPort() const
{
	return (this->_portNumber);
}

const std::vector<std::string>&	ServerConfig::getServerNames() const
{
	return (this->_serverNames);
}

const LocationConfig&	ServerConfig::getLocationConfig(std::string path) const
{
	std::map<std::string, LocationConfig>::const_iterator	it = this->_locations.find(path);
	if (it == this->_locations.end())
		return (this->_locations.begin()->second);
	return (it->second);
}

size_t	ServerConfig::getAmountLocations() const
{
	return (this->_amountLocations);
}

std::ostream &operator<<(std::ostream& out, const ServerConfig& loc)
{
	std::cout << "Printing ServerConfig:" << std::endl;
	out << "AmntLocs: " << loc.getAmountLocations() << std::endl;
	out << "DfServer: " << loc.getDefaultServerName() << std::endl;
	out << "Port nrs: "; for (size_t i = 0; i < loc.getPort().size(); i++){out << loc.getPort()[i] << " ";} out << std::endl;
	out << "ServNmes: "; for (size_t i = 0; i < loc.getServerNames().size(); i++){out << loc.getServerNames()[i] << " ";} out << std::endl;


	out << "BodySize: " << loc.getLimitClientBodySize() << std::endl;
	out << "LocaRoot: " << loc.getRoot() << std::endl;
	out << "indexVec: "; for (size_t i = 0; i < loc.getIndex().size(); i++){out << loc.getIndex()[i] << " ";} out << std::endl;
	out << "Autoindx: " << loc.getAutoindex() << std::endl;
	//errorpages
	out << "httpGET : " << loc.getHttpMethods(GET) << std::endl;
	out << "httpPOST: " << loc.getHttpMethods(POST) << std::endl;
	out << "httpDEL : " << loc.getHttpMethods(DELETE) << std::endl;
	//CGI std::endl;
	//CGI
	out << "Upld Dir: " << loc.getUploadDir() << std::endl;
	out << "AmntErrs: " << loc.getAmountErrorPages() << std::endl;
	out << "AmntCGIs: " << loc.getAmountCGI() << std::endl;
	out << "CGI pyth: " << loc.getCGI(".py") << std::endl;
	out << "CGI .php: " << loc.getCGI(".php") << std::endl;
	out << "CGI rand: " << loc.getCGI(".sfsdfdsf") << std::endl;
	return (out);
}
