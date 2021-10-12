#include "Server.hpp"			// Server
#include "ServerConfig.hpp"		// ServerConfig
#include "LocationConfig.hpp"	// LocationConfig
#include "Task.hpp"				// Task
#include "Client.hpp"			// Client
#include "Request.hpp"			// Request
#include "Response.hpp"			// Response
#include <string>				// std::string
#include <fcntl.h>				// open
#include <stdexcept>			// std::runtime_error
#include <sys/stat.h>			// stat
#include <sstream>				// std::stringstream

#include <iostream>		// DEBUG

/*
 * Copy constructor
 */
Server::Server(Server const &src)
{
	*this = src;
}

/*
 * Destructor
 */
Server::~Server()
{
}

/*
 * Assignment operator
 */
Server		&Server::operator=(Server const &src)
{
	this->_config = src._config;
	return *this;
}

/*
 * Parameterized constructor
 */
Server::Server(ServerConfig const &conf) : _config(conf)
{
}

/*
 * Getter for the ServerConfig
 */
const ServerConfig		&Server::getConfig() const
{
	return this->_config;
}

/*
 * This will handle a connection and checks if an error occurs.
 */
void		Server::handleConnection(Task &task)
{
	std::string				URI = task.client->getRequest().getURI();
	LocationConfig const	location = this->_config.getLocationConfig(URI);
	std::string				root = location.getRoot();
	std::string				pathName = root + URI;

	task.server = this;
	// if already an error
	// {
	// open error path
	// return ;
	// }
	if (task.client->getRequest().getProtocol() != "HTTP/1.1" || URI.find("/../") != std::string::npos)
	{
		std::cout << "alles naar de tering en geef 400 error" << std::endl;
		task.type = Task::FILE_READ;
		this->_checkErrorPath(task);
		task.client->getResponse().setStatusCode(400);
		return ;
	}
	else if (*pathName.rbegin() == '/')
	{
		std::string		currentIndexPage;
		bool			validIndexPage = false;

		for (size_t i = 0; i < location.getIndex().size(); i++)
		{
			currentIndexPage = location.getIndex()[i];
			if (this->_isValidPath(pathName + currentIndexPage) == true)
			{
				pathName.append(currentIndexPage);
				validIndexPage = true;
				break ;
			}
		}
		if (validIndexPage == false)
		{
			if (location.getAutoindex())
			{
				std::cout << "autoindex staat aan bitches" << std::endl;
			}
			else
			{
				std::cout << "geen autoindex, tijd voor een 403" << std::endl;
				task.client->getResponse().setStatusCode(403);
				this->_checkErrorPath(task);
			}
		}
	}
	std::cout << "uiteindelijke pathname: " << pathName << std::endl;

	if (task.client->getRequest().getMethod() == Request::DELETE)
	{
		std::cout << "lekkchr deleteren" << std::endl;
		task.type = Task::FILE_DELETE;
		return ;
	}
	task.type = (task.client->getRequest().getMethod() == Request::GET) ? Task::FILE_READ : Task::FILE_WRITE;
	task.fd = this->_openFile(task, pathName);
	if (task.fd < 0)
	{
		std::cout << "ERROR HANDELEN" << std::endl;
		task.type = Task::FILE_READ;
		this->_checkErrorPath(task);
	}
}

/*
 *	This function will try to open a file and gives a specific error if it didn't work.
 */
int			Server::_openFile(Task &task, std::string &pathName)
{
	Request::RequestMethod	clientRequest = task.client->getRequest().getMethod();
	Response				&response = task.client->getResponse();
	int						fd = 0;

	std::cout << "PATHNAME: " << pathName << std::endl;
	if (this->_isValidPath(pathName) == false)
	{
		std::cout << "404 error incoming baybeee" << std::endl;
		response.setStatusCode(404);
		return -1;
	}
	if (clientRequest == Request::GET)
	{
		if ((fd = open(pathName.c_str(), O_RDONLY)) < 0)
			response.setStatusCode(403);
	}
	else if (clientRequest == Request::POST)
	{
		if (this->_isValidContentLength(task) < 0)
			return -1;
		if ((fd = open(pathName.c_str(), O_CREAT | O_WRONLY | O_TRUNC)) < 0)
			response.setStatusCode(403);
	}
	else if (clientRequest == Request::OTHER)
	{
		response.setStatusCode(405);
		return -1;
	}
	if (fd > 0 && fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Ja kak weer een error");
	return fd;
}

/*
 *	This function checks if the server config file gave us a valid path to an error page.
 *	If this is not the case, our default error string will be set to true.
 */
void		Server::_checkErrorPath(Task &task)
{
	size_t				errorCode = task.client->getResponse().getStatusCode();
	bool				hasErrorPage = this->_config.hasErrorPage(errorCode);
	std::string			errorConfigPath;

	task.fd = 0;
	task.client->getRequest().setMethod("GET");
	if (hasErrorPage == true)
	{
		std::cout << "De error page van code: " << errorCode << " bestaat!" << std::endl;
		errorConfigPath = this->_config.getErrorPage(errorCode);
		std::cout << "ERROR CONFIG PATH: " << errorConfigPath << std::endl;
		task.fd = this->_openFile(task, errorConfigPath);
		if (task.fd == -1)
			hasErrorPage = false;
	}
	if (hasErrorPage == false)
	{
		task.type = Task::CLIENT_RESPONSE;
		task.fd = task.client->fd;
		task.client->getResponse().setDefaultError();
		std::cout << "DEFAULT ERROR STAAT AAN" << std::endl;
	}
}

/*
 *	This function checks if the requested path exists and is a file.
 */
bool		Server::_isValidPath(std::string const &path)
{
	struct stat	stats;
	int			invalidPath = stat(path.c_str(), &stats);

	if (invalidPath < 0)
		return false;
	if (S_ISDIR(stats.st_mode))
		return false;
	return true;
}

/*
 *	This function checks if the given content length overrides the max content body size
 */
int			Server::_isValidContentLength(Task &task)
{
	size_t					bodyContentLength;
	size_t					givenContentLength;
	std::stringstream		conversionStream;
	Request					&request = task.client->getRequest();
	
	bodyContentLength = request.getBody().length();
	conversionStream << request.getHeader("Content-Length");
	conversionStream >> givenContentLength;
	if (bodyContentLength != givenContentLength)
	{
		task.client->getResponse().setStatusCode(400);
		return -1;
	}
	else if (task.server->getConfig().getLimitClientBodySize() < givenContentLength)
	{
		task.client->getResponse().setStatusCode(413);
		return -1;
	}
	return 0;
}
