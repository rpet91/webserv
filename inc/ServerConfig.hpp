#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>

typedef unsigned int uint;

class Serverconfig
{
	public:
		//constructors()
			~Serverconfig();
			Serverconfig();
			Serverconfig(std::string data);
			Serverconfig(Serverconfig const& src);
			Serverconfig const& operator=(Serverconfig const& src);

		//get()
			// std::string	get_default() const;
			// uint			get_port() const;
			// std::string	get_servernames() const;
			// std::string	get_error_page() const;

		//set()
			// void	set_default(std::string);
			// void	set_port(uint);
			// void	set_server_name(std::string);	//add one name to list
			// void	set_error_page(std::string);

		std::string					_rawInput;
	
	private:
		std::string    				_defaultServerName;
		uint	    				_portNumber;
		std::vector<std::string>	_serverNames;
		std::string					_errorPage;
		uint						_limitClientBodySize;
		//routes
		

};

std::ostream&	operator<<(std::ostream& os, Serverconfig const& sc);

#endif
