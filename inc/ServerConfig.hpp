#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationConfig.hpp"
#include <iostream>


class ServerConfig
{
	public:
		//constructors()
			virtual ~ServerConfig();
			ServerConfig();
			ServerConfig(const ServerConfig&);
			ServerConfig&	operator=(const ServerConfig&);
			void			init(std::string& str);

		//get()
			const std::string&				getDefaultServerName() const;
			const std::string&				getDefaultErrorPage() const;
			const std::vector<size_t>&		getPort() const;
			const std::vector<std::string>&	getServerNames() const;
			const std::string&				getErrorPage(size_t) const;
			bool							hasErrorPage(size_t) const;
			size_t							getLimitClientBodySize() const;
			const std::string&				getRoot() const;
			const std::vector<std::string>&	getIndex() const;
			bool							getAutoindex() const;
			bool							getHttpMethods(enum e_httpMethods) const;
			const std::string&				getCgi(enum e_cgi) const;
			const LocationConfig&			getLocationConfig(std::string path) const;

		//set()
			void	setDefaultServerName();
			void	setPort(const std::string&);
			void	setServerName(const std::string&);
			void	setErrorPage(const std::string&);
			void	setLimitClientBodySize(const std::string&);
			void	setRoot(const std::string&);
			void	setIndex(const std::string&);
			void	setAutoindex(const std::string&);
			void	setHttpMethods(const std::string&);
			void	setCgi(const std::string&);
			void	setLocationConfig(const std::string&);

	private:
		std::string    							_defaultServerName;
		std::vector<size_t>	    				_portNumber;
		std::vector<std::string>				_serverNames;
		std::map<size_t, std::string> 			_errorPage;
		size_t									_limitClientBodySize;
		std::string								_root;
		std::vector<std::string>				_index;
		bool									_autoindex;
		bool									_httpMethods[3];	//G,P,D
		std::string								_cgi[2];			//extension,path
		std::map<std::string, LocationConfig>	_locations;
};

std::ostream &operator<<(std::ostream& out, ServerConfig const& loc);

typedef ServerConfig S;
#endif
