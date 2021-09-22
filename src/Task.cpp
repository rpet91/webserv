#include "Task.hpp"
#include "Server.hpp"
#include "Client.hpp"

Task::Task()
{
}

Task::Task(Task const &src)
{
	*this =src;
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
	return *this;
}

Task::Task(TaskType type, int fd, Server *server, Client *client) :
	type(type), fd(fd), server(server), client(client)
{
}
