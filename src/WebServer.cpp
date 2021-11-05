#include "WebServer.hpp"

#include <iostream>	// DEBUG

#include <sys/socket.h>		// socket, bind, listen, AF_INET, SOCK_STREAM, recv
#include <sys/select.h>		// select
#include <netinet/in.h>		// struct sockaddr_in, INADDR_ANY
#include <stdexcept>		// std::runtime_error
#include <vector>			// std::vector
#include <string>			// std::string
#include <strings.h>		// bzero
#include <unistd.h>			// read
#include <fcntl.h>			// fcntl, F_SETFL, O_NONBLOCK
#include <cstdlib>			// exit
#include <sstream>			// stringstream, istringstream
#include "Server.hpp"		// Server
#include "ServerConfig.hpp"	// ServerConfig
#include "Task.hpp"			// Task
#include "Request.hpp"		// Request
#include "Response.hpp"		// Response
#include "CGI.hpp"			// CGI

/*
 * The constructor is the only starting point for the WebServer. We
 * create the sockets and Server objects, and then run the main loop
 * indefinitely.
 */
WebServer::WebServer()
{
}

/*
 * Destructor
 */
WebServer::~WebServer()
{
}

/*
 * This function is the main loop of the program, it will keep running
 * forever. It waits for incoming connections or other work to be ready,
 * and then handles it.
 */
void		WebServer::run()
{
	std::cout << "Now entering main loop." << std::endl;

	fd_set								read_tmp;
	fd_set								write_tmp;
	std::map<int, Task>::iterator		it;
	Task								currentTask;

	while (true)
	{
		this->_readFDToRemove.clear();
		this->_writeFDToRemove.clear();
		read_tmp = this->_readable;
		write_tmp = this->_writeable;
		this->_debugPrintFDsInSet(read_tmp, 32);
		this->_debugPrintFDsInSet(write_tmp, 32);
		std::cout << "we gaan nu select in" << std::endl;
		std::cout << "Maps are length: " << this->_readTaskMap.size() << " and ";
		std::cout << this->_writeTaskMap.size() << std::endl;
		if (select(FD_SETSIZE, &read_tmp, &write_tmp, NULL, NULL) < 0)
		{
			std::cout << "HUILIE ER GING IETS OFUT" << std::endl;
			perror("ofut");
			// In the future we won't exit, this is for testing purposes.
			exit(EXIT_FAILURE);
		}
		for (it = this->_readTaskMap.begin(); it != this->_readTaskMap.end(); it++)
		{
			currentTask = it->second;
			this->_debugCheckTaskFD(currentTask, "reading"); // debug
			if (FD_ISSET(it->first, &read_tmp))
			{
				std::cout << "Fd " << it->first << " is set for reading" << std::endl;
				if (currentTask.type == Task::WAIT_FOR_CONNECTION)
				{
					std::cout << "This was a server socket" << std::endl;
					this->_acceptConnection(it->first);
				}
				else if (currentTask.type == Task::CLIENT_READ)
				{
					std::cout << "This was a client socket from which we will read" << std::endl;
					this->_readRequest(this->_clientMap[it->first]);
				}
				else if (currentTask.type == Task::FILE_READ)
				{
					std::cout << "Lekkchr lezen" << std::endl;
					this->_readFile(currentTask);

					std::cout << "Lekkchr gelezen" << std::endl;
				}
				else
				{
					std::cout << "Something went wrong: unknown read task type" << std::endl;
					currentTask.printType();
					std::cout << std::endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		for (it = this->_writeTaskMap.begin(); it != this->_writeTaskMap.end(); it++)
		{
			currentTask = it->second;
			this->_debugCheckTaskFD(currentTask, "writing"); // debug
			if (FD_ISSET(it->first, &write_tmp))
			{
				// write stuff
				std::cout << "Fd " << it->first << " is set for writing" << std::endl;

				if (currentTask.type == Task::CLIENT_RESPONSE)
				{
					this->_sendResponse(currentTask);
					this->_markFDForRemoval(it->first, this->_writeable, WRITE);
					FD_CLR(it->first, &this->_writeable);
				}
				else if (currentTask.type == Task::FILE_WRITE)
				{
					std::cout << "Lekkchr raiteh" << std::endl;
					this->_writeFile(currentTask);
					this->_markFDForRemoval(it->first, this->_readable, READ);
					std::cout << "Lekkchr gerait" << std::endl;
				}
				else
				{
					std::cout << "Something went wrong: unknown write task type" << std::endl;
					currentTask.printType();
					exit(EXIT_FAILURE);
				}
			}
		}
		for (std::vector<int>::iterator it = this->_readFDToRemove.begin(); it != this->_readFDToRemove.end(); it++)
		{
			this->_readTaskMap.erase(*it);
			std::cout << "Removed fd [" << *it << "] from the read taskset." << std::endl;
		}
		for (std::vector<int>::iterator it = this->_writeFDToRemove.begin(); it != this->_writeFDToRemove.end(); it++)
		{
			this->_writeTaskMap.erase(*it);
			std::cout << "Removed fd [" << *it << "] from the write taskset." << std::endl;
		}
	}
}

/*
 * This function creates sockets, binds them to ports and starts listening to
 * them.
 */
void		WebServer::setup(std::vector<ServerConfig> const &serverConfigs)
{
	std::cout << "Opening sockets I guess" << std::endl;

	// Iterate over the ServerConfigs we've received from the parser.
	// Create a Server instance with this config for every port it wants
	// to listen on.
	std::vector<ServerConfig>::const_iterator		it;
	std::vector<size_t>								ports;
	std::vector<size_t>::const_iterator				portIt;
	ServerConfig									current;
	std::map<int, std::vector<Server> >::iterator	serverIt;
	for (it = serverConfigs.begin(); it != serverConfigs.end(); it++)
	{
		current = *it;
		ports = current.getPort();
		for (portIt = ports.begin(); portIt != ports.end(); portIt++)
		{
			std::cout << "PORT: " << *portIt << std::endl;
			this->_servers[*portIt].push_back(Server(current));
		}
	}
	std::cout << "Er zijn " << this->_servers.size() << " ports open ??" << std::endl;
	for (serverIt = this->_servers.begin(); serverIt != this->_servers.end(); serverIt++)
	{
		std::cout << "Port nr " << serverIt->first << " heeft " << serverIt->second.size() << " servers" << std::endl;
		this->_sockets.push_back(this->_openSocket(serverIt->first));
	}

	// Zero out the fd sets.
	FD_ZERO(&this->_readable);
	FD_ZERO(&this->_writeable);

	// Put the server sockets into the readable fd set.
	std::vector<int>::iterator		socketIt;
	Task							tmpTask;
	for (socketIt = this->_sockets.begin(); socketIt != this->_sockets.end(); socketIt++)
	{
		std::cout << "Setting fd " << *socketIt << " to listen" << std::endl;
		tmpTask.type = Task::WAIT_FOR_CONNECTION;
		tmpTask.fd = *socketIt;
		this->_addTask(tmpTask);
	}
	this->_responsesSetup();
}

/*
 * This function opens a socket and returns the file descriptor.
 */
int			WebServer::_openSocket(int port)
{
	struct sockaddr_in		serverAddress;
	int						ret;
	int						socketFD;

	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
		throw std::runtime_error("Failed to create socket.");
	int options = 1;
	ret = setsockopt(socketFD, SOL_SOCKET, SO_REUSEPORT, &options, sizeof(options));
	if (socketFD < 0)
		throw std::runtime_error("idklmao");

	// Initialize the address struct that bind will use.
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Bind the socket to a port.
	ret = bind(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (ret < 0)
	{
		perror("le socket");
		throw std::runtime_error("Failed to bind socket to port.");
	}

	// Set the socket to listen for incoming connections.
	ret = listen(socketFD, 10);
	if (ret < 0)
		throw std::runtime_error("Failed to make socket listen to incoming connections.");

	return socketFD;
}

/*
 * This function accepts a connection from a socket, and returns the
 * client's socket file descriptor.
 */
void		WebServer::_acceptConnection(int socketFD)
{
	struct sockaddr_in	clientAddress;
	int					addressSize = sizeof(struct sockaddr_in);
	int					clientSocketFD;

	std::cout << "Accepting connection from socket " << socketFD << std::endl;
	clientSocketFD = accept(socketFD, (struct sockaddr*)&clientAddress, (socklen_t*)&addressSize);
	fcntl(clientSocketFD, F_SETFL, O_NONBLOCK);
	std::cout << "New fd is [" << clientSocketFD << "]" << std::endl;
	Client				client(clientSocketFD, (struct sockaddr*)&clientAddress);
	if (client.fd < 0)
	{
		perror("halp");
		throw std::runtime_error("Failed to accept connection.");
	}
	this->_clientMap[client.fd] = client;
	Task		task(Task::CLIENT_READ, client.fd, 0, &this->_clientMap[client.fd]);
	this->_addTask(task);
}

/*
 * This function reads the incoming HTTPRequest.
 */
void		WebServer::_readRequest(Client &client)
{
	char	buffer[RECVBUFFERSIZE + 1];
	int		bytesRead;

	// Read from the socket in a loop until we find '\r\n\r\n', signifying the
	// end of the incoming message. If we read 0 bytes, the connection was
	// closed by the client.
	bzero(buffer, RECVBUFFERSIZE + 1);
	bytesRead = recv(client.fd, buffer, RECVBUFFERSIZE, 0);
	std::cout << "BYTES READ: AAAAAAAAAAAAAH: " << bytesRead << std::endl;
	if (bytesRead < 0)
	{
		perror("bytes read < 0: ");
		throw std::runtime_error("recv error");
	}
	else if (bytesRead == 0)
	{
		// Connection was closed on the client's side.
		std::cout << "Closing client fd: [" << client.fd << "]" << std::endl;
		close(client.fd);
		this->_markFDForRemoval(client.fd, this->_readable, READ);
		return ;
	}
	client.setIncomingMessage(buffer);
	if (client.getIncomingMessage().find("\r\n\r\n") == std::string::npos)
		return ;
	std::cout << std::endl;
	std::cout << "[[[[[[[[[" << std::endl;
	std::cout << client.getIncomingMessage();
	std::cout << "]]]]]]]]]" << std::endl;
	this->_processRequest(client, client.getIncomingMessage());
}

/*
 * This function processes the message that was sent by the client into the
 * component parts.
 */
void		WebServer::_processRequest(Client &client, std::string requestMessage)
{
	size_t				lineStart;
	size_t				prevStart;
	std::string			header;
	std::string			body;

	// Process the first line into the separate pieces of information.
	lineStart = 0;
	client.getRequest().setMethod(this->_getString(&lineStart, requestMessage, " "));
	client.getRequest().setURI(this->_getString(&lineStart, requestMessage, " "));
	client.getRequest().setProtocol(this->_getString(&lineStart, requestMessage, "\r\n"));

	// Process the headers.
	while (true)
	{
		prevStart = lineStart;
		header = this->_getString(&lineStart, requestMessage, "\r\n");
		if (prevStart == lineStart || header == "")
			break;
		else
			client.getRequest().setHeader(header);
	}

	// Put the remaining message into the body.
	body = this->_getString(&lineStart, requestMessage, "\r\n\r\n");
	client.getRequest().setBody(body);

	// Finally we find the server that needs to handle this request.
	this->_findServerForRequest(client);
}

/*
 * This function finds the server that should handle the request of the client.
 */
void		WebServer::_findServerForRequest(Client &client)
{
	std::string									host;
	std::string									hostName;
	int											port;
	size_t										colonPosition;
	Task										task;
	std::vector<Server>							serversForPort;
	bool										serverFound;
	std::vector<Server>::iterator				serverIt;
	ServerConfig								serverConfig;
	std::vector<std::string>::const_iterator	nameIt;

	task.client = &client;

	// Find the port and hostname.
	host = client.getRequest().getHeader("Host");
	if (host == "")
	{
		std::cout << "Host header not found, panic and die" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "{" << host << "}" << std::endl;
	colonPosition = host.find(":");
	hostName = host.substr(0, colonPosition);
	port = std::stoi(host.substr(colonPosition + 1));
//	std::istringstream(host.substr(colonPosition + 1)) >> port;
	std::cout << hostName << "," << port << std::endl;

	// Get the servers for this port.
	serversForPort = this->_servers[port];

	// Find the correct server to handle this request.
	serverFound = false;
	for (serverIt = serversForPort.begin(); serverIt != serversForPort.end(); serverIt++)
	{
		serverConfig = serverIt->getConfig();
		std::vector<std::string> const serverNames = serverConfig.getServerNames();
		for (nameIt = serverNames.begin(); nameIt != serverNames.end(); nameIt++)
		{
			if (*nameIt == hostName)
			{
				client.getRequest().setMatchedServerName(hostName);
				serverIt->handleConnection(task);
				serverFound = true;
				break;
			}
		}
	}
	// Drop to the default if we haven't found a suitable server in the list.
	if (!serverFound)
	{
		serverConfig = serversForPort[0].getConfig();
		client.getRequest().setMatchedServerName(serverConfig.getDefaultServerName());
		serversForPort[0].handleConnection(task);
	}
	this->_addTask(task);
}

/*
 * This helper function gets a substring from a bigger string from a starting
 * position (which gets updated after, to point to the end of the returned
 * substring), and a delimiter.
 */
std::string		WebServer::_getString(size_t *startPosition, std::string const &source, std::string const &delimiter)
{
	size_t			endPosition;
	std::string		returnString;

	endPosition = source.find(delimiter, *startPosition);
	returnString = source.substr(*startPosition, endPosition - *startPosition);
	*startPosition = endPosition + delimiter.size();
	return returnString;
}

/*
 * This function reads a file into a buffer, to send to the Client later.
 */
void		WebServer::_readFile(Task &task)
{
	std::cout << "It's time to do shit here" << std::endl;
	char		buffer[READBUFFERSIZE + 1];
	int			bytesRead;
	std::string	fileRead;
	Task		responseTask;

	bzero(buffer, READBUFFERSIZE + 1);
	bytesRead = read(task.fd, buffer, READBUFFERSIZE);
	if (bytesRead < 0)
	{
		perror("stuk");
		throw std::runtime_error("read error");
	}
	else if (bytesRead > 0)
	{
		fileRead.append(buffer, bytesRead);
		task.client->getResponse().setBody(fileRead);
		return ;
	}

	close(task.fd);
	responseTask.type = Task::CLIENT_RESPONSE;
	responseTask.client = task.client;
	responseTask.fd = task.client->fd;
	this->_addTask(responseTask);
	this->_markFDForRemoval(task.fd, this->_readable, READ);
	std::cout << "done doing shit" << std::endl;
}

/*
 *
 */
void		WebServer::_writeFile(Task &task)
{
	std::cout << "het is tijd om te schrijven" << std::endl;
	(void) task;
	exit(0);
}

/*
 * This function sends a response back to the client.
 */
void		WebServer::_sendResponse(Task &task)
{
	std::cout << "sending respones :) " << std::endl;
	int					statusCode;
	std::string			responseString;
	std::string			body;
	std::stringstream	contentLength;

	Client	*client = task.client;
	Response	&response = client->getResponse();
	statusCode = response.getStatusCode();
	if (response.isError() && response.isDefaultError() == true)
	{
		std::cout << "error code: " << statusCode << std::endl;
		body = this->_defaultError;
		_replaceDefaultErrorMessage(body, statusCode);
		response.setBody(body);
	}
	else
		body = response.getBody();
	contentLength << "Content-Length: " << body.size();
	response.setHeader(contentLength.str());
	responseString = response.getResponseText(this->_responseStatus[response.getStatusCode()]);
	//std::cout << "SENDING:" << std::endl << std::endl << responseString << std::endl << std::endl;
	int		ret = send(client->fd, responseString.c_str(), responseString.size() + 1, 0);
	response.reset();
	(void)ret;
	// error check?
}

/*
 * This function adds a task to the correct task map, and sets the fd in the
 * correct fd set.
 */
void		WebServer::_addTask(Task &task)
{
	std::cout << "Ik ben in mn nieuwe functie" << std::endl;
	if (this->_getTaskIOType(task) == READ)
	{
		this->_readTaskMap[task.fd] = task;
		FD_SET(task.fd, &this->_readable);
	}
	else
	{
		this->_writeTaskMap[task.fd] = task;
		FD_SET(task.fd, &this->_writeable);
	}
}

/*
 * This function marks a file descriptor for removal at the end of the
 * select loop.
 */
void		WebServer::_markFDForRemoval(int fd, fd_set &set, TaskIOType mode)
{
	std::cout << "Removing fd " << fd << " from the ";
	if (mode == READ)
	{
		std::cout << "read";
		this->_readFDToRemove.push_back(fd);
	}
	else
	{
		std::cout << "write";
		this->_writeFDToRemove.push_back(fd);
	}
	std::cout << " set" << std::endl;

	FD_CLR(fd, &set);
}

/*
 * This function edits the body to the correct message in case an error occurred.
 */
void		WebServer::_replaceDefaultErrorMessage(std::string &body, int errorCode)
{
	std::cout << "Jemig de penig, we hebben echt een error hoor" << std::endl;
	size_t				pos;
	std::stringstream	errorCodeString;
	std::string const	replaceCode = "ERROR_CODE";
	std::string const	replaceMessage = "ERROR_MESSAGE";

	if ((pos = body.find(replaceCode)) != std::string::npos)
	{
		errorCodeString << errorCode;
		body = body.replace(pos, replaceCode.length(), errorCodeString.str());
	}
	if ((pos = body.find(replaceMessage)) != std::string::npos)
	{
		body = body.replace(pos, replaceMessage.length(), this->_responseStatus[errorCode]);
	}
}

/*
 * This function puts all the possible HTML errors in a map
 */
void		WebServer::_responsesSetup()
{
	this->_defaultError = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Spinnenwebservetjes</title>\r\n<style>\r\nbody{background: url('/spin.png');\r\nbackground-repeat: no-repeat; background-position-y: 100px;}\r\n</style>\r\n</head>\r\n<body>\r\n<h1>HTTP error: ERROR_CODE ERROR_MESSAGE</h1>\r\n<p>&copy; Spinnenwebservetjes</p>\r\n</body>\r\n</html>\r\n";
	this->_responseStatus[200] = "OK";
	this->_responseStatus[400] = "Bad Request";
	this->_responseStatus[403] = "Forbidden";
	this->_responseStatus[404] = "Not Found";
	this->_responseStatus[405] = "Method Not Allowed";
	this->_responseStatus[413] = "Payload Too Large";
	this->_responseStatus[500] = "Internal Server Error";
}

/*
 * This function returns a TaskIOType enum based on the Task given as argument.
 */
WebServer::TaskIOType	WebServer::_getTaskIOType(Task &task)
{
	if (task.type == Task::WAIT_FOR_CONNECTION ||
			task.type == Task::CLIENT_READ ||
			task.type == Task::FILE_READ)
		return READ;
	return WRITE;
}

/*
 * DEBUG
 *
 * print fd's that are set in a set. xD lmao
 */
void		WebServer::_debugPrintFDsInSet(fd_set &set, int max = 1024)
{
	bool first = true;
	std::cout << "The set fd's are [";
	for (int i = 0; i < max; i++)
	{
		if (FD_ISSET(i, &set))
		{
			if (!first)
				std::cout << " ";
			else
				first = false;
			std::cout << i;
		}
	}
	std::cout << "]" << std::endl;
}

/*
 * DEBUG
 *
 *
 * print fd type
 */
void		WebServer::_debugCheckTaskFD(Task &task, std::string const &mode)
{
	std::cout << "Checking fd [" << task.fd << "] of type [";
	task.printType();
	std::cout << "] for " << mode << std::endl;
}
