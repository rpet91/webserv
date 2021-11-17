#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "Config.hpp"
#include <string>
#include <iostream>

class ServerConfig;

class	LocationConfig : public Config
{
	public:
		//constructors
		virtual ~LocationConfig();
		LocationConfig();
		LocationConfig(const LocationConfig&);
		LocationConfig(const ServerConfig&);
		LocationConfig&	operator=(const LocationConfig&);
		LocationConfig&	operator=(const ServerConfig&);
		void			init(const std::string&);

		//get()
		const std::string&	getPath() const;

		//set()
		void				setPath(const std::string&);

	private:
		std::string	_path;

};

std::ostream &operator<<(std::ostream& out, LocationConfig const& loc);

#endif
