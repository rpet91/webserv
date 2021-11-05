#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Config.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>

class LocationConfig;

class ServerConfig : public Config
{
	public:
		//constructors()
		virtual ~ServerConfig();
		ServerConfig();
		ServerConfig(const ServerConfig&);
		ServerConfig&	operator=(const ServerConfig&);
		void			init(std::string& str);

		//get()
		size_t							getAmountLocations() const;
		const std::string&				getDefaultServerName() const;
		const std::vector<size_t>&		getPort() const;
		const std::vector<std::string>&	getServerNames() const;
		const LocationConfig&			getLocationConfig(std::string path) const;

		//set()
		void	setDefaultServerName();
		void	setPort(const std::string&);
		void	setServerName(const std::string&);
		void	setLocationConfig(const std::string&);

	private:
		size_t									_amountLocations;
		std::string    							_defaultServerName;
		std::vector<size_t>	    				_portNumber;
		std::vector<std::string>				_serverNames;
		std::map<std::string, LocationConfig>	_locations;
};

std::ostream &operator<<(std::ostream& out, ServerConfig const& loc);

#endif
