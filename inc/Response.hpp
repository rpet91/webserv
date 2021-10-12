#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>

class Response
{
	public:
		// Coplien form
		Response();
		Response(Response const &src);
		Response		&operator=(Response const &src);
		virtual ~Response();

		// Public functions
		void			reset();
		void			setBody(std::string const &body);
		void			setHeader(std::string const &header);
		void			setStatusCode(int code);
		void			setDefaultError();
		std::string		getBody();
		std::string		getHeaders();
		int				getStatusCode();
		bool			isDefaultError();
		std::string		getResponseText(std::string const &statusText);

	private:
		// Private variables
		std::string		_body;
		std::string		_headers;
		int				_statusCode;
		bool			_defaultError;
};

#endif
