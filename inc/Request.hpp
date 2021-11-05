#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>		// std::string
# include <map>			// std::map

class Request
{
	public:
		// Coplien form stuff
		Request();
		Request(Request const &src);
		virtual ~Request();
		Request			&operator=(Request const &src);

		// Getters and setters
		std::string		getMethod() const;
		std::string		getURI() const;
		std::string		getQueryString() const;
		std::string		getProtocol() const;
		std::string		getHeader(std::string const &headerName);
		std::string		getBody() const;
		std::string		getMatchedServerName() const;
		std::string		getPort();
		void			setMethod(std::string const &method);
		void			setURI(std::string const &URI);
		void			setQueryString(std::string const &queryString);
		void			setProtocol(std::string const &protocol);
		void			setHeader(std::string const &header);
		void			setBody(std::string const &body);
		void			setMatchedServerName(std::string const &matchedServerName);

	private:
		// Private variables
		std::string							_method;
		std::string							_URI;
		std::string							_queryString;
		std::string							_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_matchedServerName;

		// Private functions
};

#endif
