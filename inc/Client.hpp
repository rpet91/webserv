#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"		// Request
# include "Response.hpp"	// Response
# include <sys/socket.h>	// sockaddr struct
# include <string>			// std::string

class Client
{
	public:
		// Coplien form stuff
		Client();
		Client(Client const &src);
		Client	&operator=(Client const &src);
		virtual ~Client();
	
		// Parameterized constructor
		Client(int socketFD, struct sockaddr *addressInfo);

		// Public functions
		Request				&getRequest();
		Response			&getResponse();
		std::string			&getAddress();
		std::string const	&getIncomingMessage();
		void				setIncomingMessage(std::string const &message);

		// Public variables
		int					fd;

	private:
		// Private variables
		Request				_request;
		Response			_response;
		std::string			_address;
		std::string			_incomingMessage;
};

#endif
