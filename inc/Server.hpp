#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerConfig.hpp"
# include "Task.hpp"
# include "Client.hpp"
# include <string>

class Task; // Forward declaration Task

class Server
{
	public:
		Server();
		Server(Server const &src);
		virtual ~Server();

		Server	&operator=(Server const &src);
		void	handleConnection(Client &client, Task &task);
	
	private:

		void	_handleGetMethod(char const *pathName, Task &task);
		void	_handlePostMethod(char const *pathName, Task &task);
		void	_handleDeleteMethod(char const *pathName, Task &task);
		// Many of the private variables will be replaced with a ServerConfig
		// object eventually
};

#endif
