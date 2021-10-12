#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>		// std::string
# include <map>			// std::map

class Request
{
	public:
		// Enum for the type of request.
		enum	RequestMethod
		{
			GET,
			POST,
			DELETE,
			OTHER
		};

		// Coplien form stuff
		Request();
		Request(Request const &src);
		virtual ~Request();
		Request								&operator=(Request const &src);

		// Getters and setters
		RequestMethod			getMethod() const;
		std::string				getURI() const;
		std::string				getProtocol() const;
		std::string				getHeader(std::string const &headerName);
		std::string				getBody() const;
		void					setMethod(std::string const &method);
		void					setURI(std::string const &URI);
		void					setProtocol(std::string const &protocol);
		void					setHeader(std::string const &header);
		void					setBody(std::string const &body);

	private:
		// Private variables
		RequestMethod						_method;
		std::string							_URI;
		std::string							_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		// Private functions
		RequestMethod			_mapStringToRequestMethod(std::string const &str);
};

#endif
