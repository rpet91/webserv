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
#include <sys/errno.h>			// errno

#include <iostream>		// DEBUG

#include <unistd.h>				// close


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

	std::string		root = location.getRoot();
	std::string		pathName = root + URI;
	struct stat		stats;

	//task.client->getRequest().setProtocol("kaas?"); // protocol is kapot  want dat is leuk
	std::cout << "HTTP PROTOCOL: " << task.client->getRequest().getProtocol() << std::endl;
	std::cout << "ROOT: " << root << std::endl;
	std::cout << "URI: " << URI << std::endl;
	std::cout << "PATHNAME: " << pathName << std::endl;
	if (task.client->getRequest().getProtocol() != "HTTP/1.1")
	{
		std::cout << "alles naar de tering en geef 400 error" << std::endl;
		task.type = Task::FILE_READ;
		this->_checkErrorPath(task);
		task.client->getResponse().setStatusCode(400);
		return ;
	}
	else if (pathName.back() == '/')
	{
		// loopje over alle mogelijke indexen
		std::string		currentIndexPage;

		std::cout << "Size index vector: " << location.getIndex().size() << std::endl;
		for (size_t i = 0; i < location.getIndex().size(); i++)
		{
			currentIndexPage = location.getIndex()[i];
			std::cout << "CURRENT INDEX: " << currentIndexPage << std::endl;
			std::string	tmp = pathName + currentIndexPage;
			std::cout << "TMP: " << tmp << std::endl;
			if (!(stat(tmp.c_str(), &stats) < 0))
			{
				pathName.append(currentIndexPage);
				break ;
			}
		}
		if (location.getAutoindex())
		{
			std::cout << "autoindex staat aan bitches" << std::endl;
		}
		else
		{
			std::cout << "geen autoindex, tijd voor een 403" << std::endl;
		//	task.client->getResponse().setStatusCode(403);
		//	this->_checkErrorPath(task);
		//	task.client->getResponse().setStatusCode(403);
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

int			Server::_openFile(Task &task, std::string &pathName)
{
	int			fd = 0;
	struct stat	stats;

	std::cout << "PATHNAME: " << pathName << std::endl;
	if (stat(pathName.c_str(), &stats) < 0)
	{
		std::cout << "404 ERROR INCOMINGGGGGG" << std::endl;
		task.client->getResponse().setStatusCode(404);
		return -1;
	}
	if (task.client->getRequest().getMethod() == Request::GET)
	{
		std::cout << "GETTEREN" << std::endl;
		if ((fd = open(pathName.c_str(), O_RDONLY)) < 0)
			task.client->getResponse().setStatusCode(403);
	}
	else if (task.client->getRequest().getMethod() == Request::POST)
	{
		std::cout << "POSTEREN" << std::endl;
		if ((fd = open(pathName.c_str(), O_CREAT | O_WRONLY | O_TRUNC)) < 0)
			task.client->getResponse().setStatusCode(403);
	}
	else if (task.client->getRequest().getMethod() == Request::OTHER)
	{
		std::cout << "METHOD NOT ALLOWED" << std::endl;
		task.client->getResponse().setStatusCode(405);
		return -1;
	}
	if (fd > 0 && fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Ja kak weer een error");
	return fd;
}

/*
 *	This function checks if the server config file gave us a valid path to an error page.
 *	If this is not the case, our default error string will be returned.
 */
void		Server::_checkErrorPath(Task &task)
{
//	std::string		errorConfigPath = "default_error.html";
	size_t				errorCode = task.client->getResponse().getStatusCode();
	bool				hasErrorPage = this->_config.hasErrorPage(errorCode);
	std::string			errorConfigPath;
	
	if (hasErrorPage == true)
	{
		errorConfigPath = this->_config.getErrorPage(errorCode);
		task.fd = this->_openFile(task, errorConfigPath);
	}
	if (hasErrorPage == false || task.fd < 0)
	{
		task.type = Task::CLIENT_RESPONSE;
		task.fd = task.client->fd;
		task.client->getResponse().setDefaultError();
	}
}
