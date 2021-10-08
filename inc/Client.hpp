#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"		// Request
# include "Response.hpp"	// Response

class Client
{
	public:
		// Coplien form stuff
		Client();
		Client(Client const &src);
		Client	&operator=(Client const &src);
		virtual ~Client();
	
		// Parameterized constructor
		Client(int socketFD);

		// Public functions
		Request			&getRequest();
		Response		&getResponse();

		// Public variables
		int					fd;

	private:
		// Private variables
		Request				_request;
		Response			_response;
};

#endif
