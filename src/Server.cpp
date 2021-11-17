#include "Server.hpp"			// Server
#include "ServerConfig.hpp"		// ServerConfig
#include "LocationConfig.hpp"	// LocationConfig
#include "Task.hpp"				// Task
#include "Client.hpp"			// Client
#include "Request.hpp"			// Request
#include "Response.hpp"			// Response
#include "CGI.hpp"				// CGI
#include <string>				// std::string
#include <fcntl.h>				// open
#include <unistd.h>				// fork, dup2, execve, getcwd
#include <cstdlib>				// exit
#include <stdexcept>			// std::runtime_error
#include <sys/stat.h>			// stat
#include <sstream>				// std::stringstream
#include <dirent.h>				// opendir

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
	LocationConfig			location = this->_config.getLocationConfig(URI);
	std::string				root = location.getRoot();
	std::string				pathName = root + URI;
	std::string				method = task.client->getRequest().getMethod();
//	std::string				redirect = this->_config.getRedirection(); // A function which will return a string to the redirection path. It returns an empty string if no redirection was found. 
	std::string				redirect = "";

	//hardcoded redirect so it will redirect "/redirect/" to "/". In the configfile it should be something like "return 301 /;"
	if (URI == "/redirect/")
		redirect = "/";

	task.server = this;
	if (this->_checkForInitialErrors(task))
	{
		this->_checkErrorPath(task);
		return ;
	}

	// Checks if the server has a redirection.
	if (redirect != "")
	{
		task.client->getRequest().setURI(redirect);
		URI = redirect;
		pathName = root + URI;
		task.client->getResponse().setStatusCode(301);
		location = this->_config.getLocationConfig(URI);
	}

	// Checks of the requested URI end with a /
	if (*pathName.rbegin() == '/')
	{
		std::string		currentIndexPage;
		bool			validIndexPage = false;

		for (size_t i = 0; i < location.getIndex().size(); i++)
		{
			currentIndexPage = location.getIndex()[i];
			if (this->_pathType(pathName + currentIndexPage) == DOCUMENT)
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
				this->_generateDirectoryListing(task, URI);
			}
			else
			{
				std::cout << "geen autoindex, tijd voor een 403" << std::endl;
				task.client->getResponse().setStatusCode(403);
				this->_checkErrorPath(task);
			}
			return ;
		}
	}
	std::cout << "uiteindelijke pathname: " << pathName << std::endl;
	if (method == "DELETE")
	{
		this->_deleteFile(task, pathName);
		return ;
	}

	bool	get = (method == "GET");
	if (this->_config.hasCGI(URI) == true)
	{
		std::cout << "We have a CGI for: " << URI << std::endl;
		std::string		CGIPath = this->_config.getCGI(URI);
		this->_launchCGI(task, CGIPath);
		if (task.client->getResponse().isError())
			this->_checkErrorPath(task);
		else
		{
			if (get == true)
			{
				task.type = Task::CGI_READ;
				task.fd = task.cgi->fdout[0];
			}
			else
			{
				task.type = Task::CGI_WRITE;
				task.fd = task.cgi->fdin[1];
			}
		}
		std::cout << "fd na cgi dingen: " << task.fd << std::endl;
	}
	else
	{
		std::cout << "We DO NOT have a CGI for: " << URI << std::endl;
		task.type = (get == true) ? Task::FILE_READ : Task::FILE_WRITE;
		task.fd = this->_openFile(task, pathName);
		if (task.fd < 0)
			this->_checkErrorPath(task);
	}
}

/*
 *	This function will try to open a file and gives a specific error if it didn't work.
 */
int			Server::_openFile(Task &task, std::string &pathName)
{
	std::string		clientRequest = task.client->getRequest().getMethod();
	Response		&response = task.client->getResponse();
	int				fd = 0;

	std::cout << "PATHNAME: " << pathName << std::endl;
	if (clientRequest != "POST" && this->_pathType(pathName) != DOCUMENT)
	{
		std::cout << "404 error incoming baybeee" << std::endl;
		response.setStatusCode(404);
		return -1;
	}
	else if (clientRequest == "GET")
	{
		if ((fd = open(pathName.c_str(), O_RDONLY)) < 0)
			response.setStatusCode(403);
	}
	else if (clientRequest == "POST")
	{
		response.setStatusCode(201);
		if (this->_isValidContentLength(task) < 0)
			return -1;
		if ((fd = open(pathName.c_str(), O_CREAT | O_WRONLY | O_TRUNC)) < 0)
			response.setStatusCode(403);
	}
	else if (clientRequest == "OTHER")
	{
		std::cout << "error 405 is nu echt nodig" << std::endl;
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

	std::cout << "ERROR HANDELEN" << std::endl;
	task.fd = 0;
	task.client->getRequest().setMethod("GET");
	if (hasErrorPage == true)
	{
		task.type = Task::FILE_READ;
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
Server::PathType	Server::_pathType(std::string const &path)
{
	struct stat	stats;
	int			validPath = stat(path.c_str(), &stats);

	if (validPath < 0)
		return INVALID;
	if (S_ISDIR(stats.st_mode))
		return DIRECTORY;
	return DOCUMENT;
}

/*
 *	This function checks if the given content length overrides the max content body size
 */
int			Server::_isValidContentLength(Task &task)
{
	size_t					givenContentLength;
	std::stringstream		conversionStream;
	Request					&request = task.client->getRequest();
	
	conversionStream << request.getHeader("Content-Length");
	conversionStream >> givenContentLength;
	if (task.client->getRequest().getBodyLength() != givenContentLength)
	{
		task.client->getResponse().setStatusCode(400);
		return -1;
	}
	else if (task.server->getConfig().getLimitClientBodySize() < givenContentLength)
	{
		std::cout << "wtf?" << std::endl;
		task.client->getResponse().setStatusCode(413);
		return -1;
	}
	return 0;
}

/*
 *	This funtion generates our autoindex page and will be set in the body to return.
 *	It also sorts directories from files.
 */
void		Server::_generateDirectoryListing(Task &task, std::string const &URI)
{
	std::string		autoindexList = "<html>\r\n<head>\r\n<title>Index of " + URI + "</title>\r\n</head>\r\n<body>\r\n<h1>Index of " + URI + "</h1>\r\n<hr>\r\n<pre>\r\n";
	std::string		endBody = "\r\n</pre>\r\n<hr>\r\n<p>&copy; Spinnenwebservetjes</p></body>\r\n</html>\r\n";
	DIR				*dir;
	struct dirent	*entry;
	std::string		pathName = this->_config.getLocationConfig(URI).getRoot() + URI;
	std::string		name;
	std::string		link;
	std::string		directoryList;
	std::string		fileList;

	dir = opendir(pathName.c_str());
	if (!dir)
	{
		task.client->getResponse().setStatusCode(404);
		this->_checkErrorPath(task);
		return ;
	}
	entry = readdir(dir); // The first entry "." should not be visible in the autoindex
	while ((entry = readdir(dir)) != NULL)
	{
		name = (entry->d_type == DT_DIR) ? std::string(entry->d_name).append("/") : entry->d_name;
		link = "<a href='" + name + "'>" + name + "</a>\r\n";
		if (entry->d_type == DT_DIR)
			directoryList += link;
		else
			fileList += link;
	}
	autoindexList += directoryList + fileList + endBody;
	closedir(dir);
	task.client->getResponse().setBody(autoindexList);
	task.type = Task::CLIENT_RESPONSE;
	task.fd = task.client->fd;
}

/*
 * This function attempts to launch the CGI application and makes sure we can
 * communicate back and forth to it.
 */
void		Server::_launchCGI(Task &task, std::string const &CGIPath)
{
	CGI			*cgi;
	pid_t		pid;
	bool		post = (task.client->getRequest().getMethod() == "POST") ? true : false;

	// Checks if the content length doesn't override the accepted limit
	if (post && this->_isValidContentLength(task) < 0)
	{
		std::cout << "CGI content length is invalid " << std::endl;
		return ;
	}
	// Checks if the path to CGI executable is valid.
	if (this->_pathType(CGIPath) != DOCUMENT)
	{
		task.client->getResponse().setStatusCode(404);
		return ;
	}
	cgi = new CGI();
	// Create the pipe that the CGI will use to write its output back to us.
	if (pipe(cgi->fdout) < 0)
	{
		task.client->getResponse().setStatusCode(500);
		return ;
	}
	// Create the pipe that we will use to write input to the CGI, if we have POST
	// info.
	if (post && pipe(cgi->fdin) < 0)
	{
		this->_closePipesAndError(task, cgi);
		return ;
	}

	pid = fork();
	if (pid < 0)
	{
		this->_closePipesAndError(task, cgi);
		return ;
	}
	if (pid == 0)
	{
		// If we are the child process, set all the necessary environment
		// variables.
		this->_setEnvironmentVars(task);
		
		// Close the read end of the output pipe.
		close(cgi->fdout[0]);

		// Clone the write end of the output pipe to the standard output,
		// then close the pipe.
		dup2(cgi->fdout[1], STDOUT_FILENO);
		close(cgi->fdout[1]);
		if (post)
		{
			// If we have opened the fdin pipe, close the write end of it,
			// clone the read end to the standard input and close that pipe as well.
			close(cgi->fdin[1]);
			dup2(cgi->fdin[0], STDIN_FILENO);
			close(cgi->fdin[0]);
		}
		char	*execArgs[2] = {const_cast<char*>(CGIPath.c_str()), NULL};
		std::cerr << "boeiend" << std::endl;
		execv(CGIPath.c_str(), execArgs);
		std::cerr << "Couldn't find the CGI executable." << std::endl;
		exit(EXIT_FAILURE);
	}
	close(cgi->fdout[1]);
	if (post)
		close(cgi->fdin[0]);
	task.cgi = cgi;
}

/*
 * The list with environment variables needed for the CGI executable.
 */
void	Server::_setEnvironmentVars(Task &task)
{
	std::cout << "CGI vars zetten in child process" << std::endl;
	Request					request = task.client->getRequest();
	std::string				URI = task.client->getRequest().getURI();
	LocationConfig const	location = this->_config.getLocationConfig(URI);
	std::string				root = location.getRoot();
	std::string				cwd = std::string(getcwd(0, 0)) + '/';

	this->_trySetVar("CONTENT_TYPE", request.getHeader("Content-Type"));
	this->_trySetVar("CONTENT_LENGTH", request.getHeader("Content-Length"));
	this->_trySetVar("GATEWAY_INTERFACE", "CGI/1.1");
	this->_trySetVar("HTTP_ACCEPT", request.getHeader("Accept"));
	this->_trySetVar("HTTP_ACCEPT_CHARSET", request.getHeader("Accept-Charset"));
	this->_trySetVar("HTTP_ACCEPT_ENCODING", request.getHeader("Accept-Encoding"));
	this->_trySetVar("HTTP_ACCEPT_LANGUAGE", request.getHeader("Accept-Language"));
	this->_trySetVar("HTTP_CONNECTION", request.getHeader("Connection"));
	this->_trySetVar("HTTP_HOST", request.getHeader("Host"));
	this->_trySetVar("HTTP_USER_AGENT", request.getHeader("User-Agent"));
	this->_trySetVar("PATH_INFO", root + URI);
	this->_trySetVar("QUERY_STRING", request.getQueryString());
	this->_trySetVar("REDIRECT_STATUS", "true");
	this->_trySetVar("REMOTE_ADDR", task.client->getAddress());
	this->_trySetVar("REQUEST_METHOD", request.getMethod());
	this->_trySetVar("SCRIPT_FILENAME", cwd + root + URI);
	this->_trySetVar("SCRIPT_NAME", URI);
	this->_trySetVar("SERVER_NAME", request.getMatchedServerName());
	this->_trySetVar("SERVER_PORT", request.getPort());
	this->_trySetVar("SERVER_PROTOCOL", "HTTP/1.1");
	this->_trySetVar("SERVER_SOFTWARE", "Spinnenwebservetjes v1.337");
}

/*
 * This function will create every environment variable needed for CGI.
 */
void	Server::_trySetVar(const char *name, std::string const &value)
{
	// If setting the environment var is not possible, the child process should exit.
	if (setenv(name, value.c_str(), 1) < 0)
		exit(EXIT_FAILURE);
}

/*
 * This function will only be called when something goes wrong before
 * the child process setup.
 */
void	Server::_closePipesAndError(Task &task, CGI *cgi)
{
	close(cgi->fdin[0]);
	close(cgi->fdin[1]);
	close(cgi->fdout[0]);
	close(cgi->fdout[1]);
	task.client->getResponse().setStatusCode(500);
}

/*
 * This function will check for initial errors before we start handling the connection.
 */
int		Server::_checkForInitialErrors(Task &task)
{
	Response		&clientResponse = task.client->getResponse();
	std::string		method = task.client->getRequest().getMethod();
	std::string		protocol = task.client->getRequest().getProtocol();
	std::string		URI = task.client->getRequest().getURI();

	// Checks if we already had an error before we entered handle connection.
	if (clientResponse.isError())
		return 1;
	// Checks if the request of the client is accepted by the config file.
	else if (task.server->getConfig().isValidHttpMethod(method) == false)
	{
		std::cout << "invalid method" << std::endl;
		clientResponse.setStatusCode(405);
		return 1; 
	}
	// Checks if the request of the client has a valid URI.
	else if (protocol != "HTTP/1.1" || URI.find("/../") != std::string::npos)
	{
		std::cout << "alles naar de tering en geef 400 error" << std::endl;
		clientResponse.setStatusCode(400);
		return 1;
	}
	return 0;
}

/*
 * This function executes the delete request.
 */
void	Server::_deleteFile(Task &task, std::string const &path)
{
	int ret = remove(path.c_str());
	std::cout << "tijd om iets te duhlietun. ret: " << ret << std::endl;
	if (ret < 0)
	{
		if (ret == EACCES)
			task.client->getResponse().setStatusCode(403);
		else
			task.client->getResponse().setStatusCode(404);
	}
	else
		task.client->getResponse().setStatusCode(204);
	task.fd = task.client->fd;
	task.type = Task::CLIENT_RESPONSE;
}
