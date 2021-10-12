#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerConfig.hpp"	// ServerConfig
# include "Task.hpp"			// Task
# include <string>				// std::string

class Task; // Forward declaration Task

class Server
{
	public:
		// Coplien form
		Server(Server const &src);
		virtual ~Server();
		Server		&operator=(Server const &src);

		// Parameterized constructor
		Server(ServerConfig const &conf);

		// Public functions
		const ServerConfig	&getConfig() const;
		void				handleConnection(Task &task);
	
	private:
		// Private default constructor
		Server();

		// Private functions
		int			_openFile(Task &client, std::string &pathName);
		void		_checkErrorPath(Task &task);
		bool		_isValidPath(std::string const &path);
		int			_isValidContentLength(Task &task);

		// Private variables
		ServerConfig	_config;
};

#endif
