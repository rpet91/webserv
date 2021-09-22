#include "Server.hpp" // Server
#include "Task.hpp" // Task
#include "Client.hpp" // Client
#include <string> // std::string
#include <fcntl.h> // open
#include <stdexcept> // std::runtime_error

#include <iostream>		// DEBUG

#include <unistd.h>				// close

/*
 * Default constructor, this sets some hardcoded values for now, should be
 * made private later, because we want to construct with a ServerConfig object.
 */
Server::Server()
{
}

/*
 * Copy constructor, we want to make this private I suppose, since it makes
 * no sense to copy a Server object.
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
 * Assignment operator, make this private? Since it, again, makes no sense
 * to assign one Server to Another.
 */
Server		&Server::operator=(Server const &src)
{
	// Do actual assign stuff :)
	(void)src;
	return *this;
}

/*
 * This will handle a conncetion. Don't know where this goes just yet.
 */
void		Server::handleConnection(Client &client, Task &task)
{
	std::string		method = "GET"; // Hardcoded request method
	std::string		URI = "/index.html"; // Hardcoded uri string
	std::string		root = "web"; // Hardcoded root
	const char		*pathName = (root + URI).c_str();

	(void) client; // Has the correct method for me
	/*
	 * open root + uri.
	 * edit task naar file read (get)
	 * fd in Task.fd zetten
	 *
	*/
	std::cout << "Pathname: " << pathName << std::endl;
	if (method == "GET")
	{
		std::cout << "lekkchr getten" << std::endl;
		this->_handleGetMethod(pathName, task);
	}
	if (method == "POST")
	{
		std::cout << "lekkchr posten" << std::endl;
		this->_handlePostMethod(pathName, task);
	}
	if (method == "DELETE")
	{
		std::cout << "lekkchr deleteren" << std::endl;
		this->_handleDeleteMethod(pathName, task);
	}
}

void	Server::_handleGetMethod(char const *pathName, Task &task)
{
	int	fd = open(pathName, O_RDONLY);

	if (fd < 0)
		throw std::runtime_error("Failed to open the path name");
	task.fd = fd;
	task.type = Task::FILE_READ;
}

void	Server::_handlePostMethod(char const *pathName, Task &task)
{
	int	fd = open(pathName, O_CREAT | O_WRONLY | O_TRUNC);

	if (fd < 0)
		throw std::runtime_error("Failed to open the path name");
	task.fd = fd;
	task.type = Task::FILE_WRITE;
}

void	Server::_handleDeleteMethod(char const *pathName, Task &task)
{
	int	fd = open(pathName, O_WRONLY);

	if (fd < 0)
		throw std::runtime_error("Failed to open the path name");
	task.fd = fd;
	task.type = Task::FILE_WRITE;
}
