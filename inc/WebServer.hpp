#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "Server.hpp"		// Server class
#include "Task.hpp"			// Task class
#include "Client.hpp"		// client class
#include <vector>			// vector class
#include <map>				// map class
#include <sys/select.h>		// fd_set

class WebServer
{
	public:
		WebServer();
		virtual ~WebServer();

		void								run();
		void								setup();

	private:
		int									_openSocket(int port);
		void								_acceptConnection(int socketFD);
		void								_readRequest(int port, Client &client);

		std::vector<int>					_sockets;
		std::map<int, std::vector<Server> >	_servers;
		std::map<int, Task>					_taskMap;
		std::map<int, Client>				_clientMap;
		fd_set								_readable;
		fd_set								_writeable;
};

#endif
