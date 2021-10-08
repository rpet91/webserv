#include <iostream>
#include <sstream>
#include <map>
#include "ServerConfig.hpp"
#include "StringUtils.hpp"

ServerConfig::~ServerConfig()
{
}

ServerConfig::ServerConfig()
{
	bzero(&this->_limitClientBodySize, sizeof(this->_limitClientBodySize));
	bzero(this->_httpMethods, sizeof(this->_httpMethods));
	bzero(this->_cgi, sizeof(this->_cgi));
}

ServerConfig::ServerConfig(ServerConfig const& src)
{
	*this = src;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& src)
{
	this->_defaultServerName = src._defaultServerName;
	this->_portNumber = src._portNumber;
	this->_serverNames = src._serverNames;
	this->_errorPage = src._errorPage;
	this->_limitClientBodySize = src._limitClientBodySize;
	this->_root = src._root;
	this->_index = src._index;
	this->_autoindex = src._autoindex;
	memcpy(this->_httpMethods, src._httpMethods, sizeof(this->_httpMethods));
	memcpy(this->_cgi, src._cgi, sizeof(this->_cgi));
	this->_locations = src._locations;
	return (*this);
}
void	ServerConfig::init(std::string& str)
{
	std::vector<std::string>	lines;

	StringUtils::trimBraces(str);
	StringUtils::split(str, ";", lines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::string identifier;
		StringUtils::matchIdentifier(lines[i], identifier);
		if (!identifier.compare("location"))
			this->setLocationConfig(lines[i]);
		else if (!identifier.compare("error_page"))
			this->setErrorPage(lines[i]);
		else if (!identifier.compare("listen"))
			this->setPort(lines[i]);
		else if (!identifier.compare("server_name"))
			this->setServerName(lines[i]);
		else if (!identifier.compare("client_max_body_size"))
			this->setLimitClientBodySize(lines[i]);
		else if (!identifier.compare("root"))
			this->setRoot(lines[i]);
		else if (!identifier.compare("index"))
			this->setIndex(lines[i]);
		else if (!identifier.compare("autoindex"))
			this->setAutoindex(lines[i]);
		else if (!identifier.compare("limit_except"))
			this->setHttpMethods(lines[i]);
		else if (!identifier.compare("cgi"))
			this->setCgi(lines[i]);
		else
			throw std::runtime_error("configfile: unknown identifier");
	}
	this->setDefaultServerName();
}

void	ServerConfig::setDefaultServerName()
{
	if (this->getServerNames()[0].empty())
		throw std::runtime_error("no servername found");
	this->_defaultServerName = getServerNames()[0];
}

void	ServerConfig::setPort(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::stringstream	ss;
		size_t				num;
		ss << vec[i];
		ss >> num;
		this->_portNumber.push_back(num);
	}
}

void	ServerConfig::setServerName(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	this->_serverNames = vec;
}

void	ServerConfig::setErrorPage(const std::string& str)
{
	std::vector<std::string>	vec;
	std::stringstream			ss;
	size_t						errorNum;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	ss << vec[0];
	ss >> errorNum;
	this->_errorPage.insert(std::pair<size_t, std::string>(errorNum, vec[1]));
}

void	ServerConfig::setLimitClientBodySize(const std::string& str)
{
	std::stringstream			ss;
	size_t						maxBodySize;
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	ss << vec[0];
	ss >> maxBodySize;
	this->_limitClientBodySize = maxBodySize;
}

void	ServerConfig::setRoot(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	this->_root = vec[0];
}

void	ServerConfig::setIndex(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	this->_index = vec;
}

void	ServerConfig::setAutoindex(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	if (!vec[0].compare("on"))
		this->_autoindex = true;
	else if (!vec[0].compare("off"))
		this->_autoindex = false;
	else
		throw std::runtime_error("configfile: bad autoindex");
}

void	ServerConfig::setHttpMethods(const std::string& str)
{
	std::vector<std::string>	vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	for (size_t i = 0; i < vec.size(); i++)
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

void	ServerConfig::setCgi(const std::string& str)
{
	std::vector<std::string> vec;

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	this->_cgi[EXTENSION] = vec[0];
	this->_cgi[PATH] = vec[1];
}

void	ServerConfig::setLocationConfig(const std::string& str)
{
	std::vector<std::string>	vec;
	LocationConfig				locationConfig;
	bool						methods[3] = {this->getHttpMethods(GET), this->getHttpMethods(POST), this->getHttpMethods(DELETE)};

	StringUtils::split(str, " \t\n", vec);
	vec.erase(vec.begin());
	locationConfig.init(str, this->getLimitClientBodySize(), this->getAutoindex(), methods);
	this->_locations.insert(std::pair<std::string, LocationConfig>(vec[0], locationConfig));
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

const std::string&	ServerConfig::getErrorPage(size_t errorNumber) const
{
	std::map<size_t, std::string>::const_iterator	it = this->_errorPage.find(errorNumber);
	return (it->second);
}

bool	ServerConfig::hasErrorPage(size_t errorNumber) const
{
	std::map<size_t, std::string>::const_iterator	it = this->_errorPage.find(errorNumber);
	if (it == this->_errorPage.end())
		return (false);
	return (true);
}

size_t	ServerConfig::getLimitClientBodySize() const
{
	return (this->_limitClientBodySize);
}

const std::string&	ServerConfig::getRoot() const
{
	return (this->_root);
}

const std::vector<std::string>&	ServerConfig::getIndex() const
{
	return (this->_index);
}

bool	ServerConfig::getAutoindex() const
{
	return (this->_autoindex);
}

bool	ServerConfig::getHttpMethods(enum e_httpMethods method) const
{
	return (this->_httpMethods[method]);
}

const std::string&	ServerConfig::getCgi(enum e_cgi arg) const
{
	return (this->_cgi[arg]);
}

const LocationConfig&	ServerConfig::getLocationConfig(std::string path) const
{
	std::map<std::string, LocationConfig>::const_iterator	it = this->_locations.find(path);
	if (it == this->_locations.end())
		return (this->_locations.begin()->second);
	return (it->second);
}

std::ostream &operator<<(std::ostream& out, ServerConfig const& sc)
{
	std::cout << "Printing ServerConfig:" << std::endl;

	out << "Default: " << sc.getDefaultServerName() << std::endl;

	std::cout << "ports:" << std::endl;
	printVec(sc.getPort());

	std::cout << "servernames:" << std::endl;
	printVec(sc.getServerNames());
	
	out << "LimitClientBodySize: " << sc.getLimitClientBodySize() << std::endl;
	
	out << "Root: " << sc.getRoot() << std::endl;

	std::cout << "index:" << std::endl;
	printVec(sc.getIndex());

	out << "AutoIndex: " << sc.getAutoindex() << std::endl;

	out << "GET: " << sc.getHttpMethods(GET) << std::endl;
	out << "POST: " << sc.getHttpMethods(POST) << std::endl;
	out << "DELETE: " << sc.getHttpMethods(DELETE) << std::endl;

	out << "Cgi ext.: " << sc.getCgi(EXTENSION) << std::endl;
	out << "Cgi path: " << sc.getCgi(PATH) << std::endl;
	return (out);
}
