#include "WebServer.hpp"

#include <iostream>	// DEBUG

#include <sys/socket.h>		// socket, bind, listen, AF_INET, SOCK_STREAM
#include <sys/select.h>		// select
#include <netinet/in.h>		// struct sockaddr_in, INADDR_ANY
#include <stdexcept>		// std::runtime_error
#include <vector>			// vector
#include "Server.hpp"		// Server
#include "Task.hpp"			// Task

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
		read_tmp = this->_readable;
		write_tmp = this->_writeable;
		if (select(FD_SETSIZE, &read_tmp, &write_tmp, NULL, NULL) < 0)
		{
			std::cout << "HUILIE ER GING IETS OFUT" << std::endl;
			// In the future we won't exit, this is for testing purposes.
			exit(EXIT_FAILURE);
		}
		for (it = this->_taskMap.begin(); it != this->_taskMap.end(); it++)
		{
			if (FD_ISSET(it->first, &read_tmp))
			{
				std::cout << "Fd " << it->first << " is set" << std::endl;
				currentTask = it->second;
				if (currentTask.type == Task::WAIT_FOR_CONNECTION)
				{
					std::cout << "This was a server socket" << std::endl;
					this->_acceptConnection(it->first);
				}
				else if (currentTask.type == Task::CLIENT_READ)
				{
					std::cout << "This was a client socket from which we will read" << std::endl;
					this->_readRequest(8080, this->_clientMap[it->first]);
					FD_CLR(it->first, &this->_readable);
				}
				else
				{
					std::cout << "Something went wrong: unknown task type" << std::endl;
				}
			}
			else if (FD_ISSET(it->first, &write_tmp))
			{
				// write stuff
			}
		}
	}
}

/*
 * This function creates sockets, binds them to ports and starts listening to
 * them.
 */
void		WebServer::setup()
{
	std::cout << "Opening sockets I guess" << std::endl;

	// We're hardcoding 4 servers, 2 per port.
	this->_servers[18000].push_back(Server());
	this->_servers[18000].push_back(Server());
	this->_servers[8080].push_back(Server());
	this->_servers[8080].push_back(Server());

	// For now this is hardcoded to open a single socket on port 8080.
	this->_sockets.push_back(this->_openSocket(18000));
	this->_sockets.push_back(this->_openSocket(8080));

	// Zero out the fd sets.
	FD_ZERO(&this->_readable);
	FD_ZERO(&this->_writeable);

	// Put the server sockets into the readable fd set.
	std::vector<int>::iterator		it;
	Task							tmpTask;
	for (it = this->_sockets.begin(); it != this->_sockets.end(); it++)
	{
		std::cout << "Setting fd " << *it << " to listen" << std::endl;
		FD_SET(*it, &this->_readable);
		tmpTask.type = Task::WAIT_FOR_CONNECTION;
		tmpTask.fd = *it;
		this->_taskMap[*it] = tmpTask;
	}

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

	// Initialize the address struct that bind will use.
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Bind the socket to a port.
	ret = bind(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (ret < 0)
	{
		perror("Hee kut, je start hem te snel op. Kijk maar naar deze error");
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
	Client				client(clientSocketFD);
	if (client.fd < 0)
	{
		perror("halp");
		throw std::runtime_error("Failed to accept connection.");
	}
	FD_SET(client.fd, &this->_readable);
	this->_clientMap[client.fd] = client;
	Task		task(Task::CLIENT_READ, client.fd, 0, &this->_clientMap[client.fd]);
	this->_taskMap[client.fd] = task;
}

/*
 * This function reads the incoming HTTPRequest, and decides which server
 * will handle the request.
 */
void		WebServer::_readRequest(int port, Client &client)
{
	std::vector<Server>		serversForThisPort;
	Task					task;
	int						serverNum;

	serversForThisPort = this->_servers[port];
	serverNum = serversForThisPort.size();
	std::cout << "There are " << serverNum << " servers in this vector." << std::endl;
	if (serverNum == 0)
		std::cout << "rippie" << std::endl;
	else
		serversForThisPort[0].handleConnection(client, task);
}
