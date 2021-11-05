#include "Task.hpp"
#include "Server.hpp"
#include "Client.hpp"

#include <iostream> // debug

Task::Task()
{
}

Task::Task(Task const &src)
{
	*this = src;
}

Task::~Task()
{
}

Task		&Task::operator=(Task const &src)
{
	this->type = src.type;
	this->fd = src.fd;
	this->server = src.server;
	this->client = src.client;
	this->cgi = src.cgi;
	return *this;
}

void		Task::printType()
{
	switch(this->type)
	{
		case WAIT_FOR_CONNECTION:
			std::cout << "Wait for connection";
			break;
		case CLIENT_READ:
			std::cout << "Client read";
			break;
		case CLIENT_RESPONSE:
			std::cout << "Client response";
			break;
		case FILE_READ:
			std::cout << "File read";
			break;
		case FILE_WRITE:
			std::cout << "File write";
			break;
		case FILE_DELETE:
			std::cout << "File delete";
			break;
		case CGI_READ:
			std::cout << "CGI read";
			break;
		case CGI_WRITE:
			std::cout << "CGI write";
			break;
		default:
			std::cout << "Unknown task type";
	}
}

Task::Task(TaskType type, int fd, Server *server, Client *client) :
	type(type), fd(fd), server(server), client(client)
{
}
