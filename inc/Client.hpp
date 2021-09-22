#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

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
		std::string		&getRequest();
		void			setRequest(std::string const &message);

		// Public variables
		int					fd;

	private:
		// Private variables
		std::string			_request;
		std::string			_response;
};

#endif
