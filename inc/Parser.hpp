#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "ServerConfig.hpp"

class Parser
{
	public:
		//constructors()
			virtual ~Parser();
			Parser();
			Parser(const Parser&);
			Parser& operator=(const Parser&);
			void	init(const std::string&);

		//get()
			size_t								getAmountServers() const;
			const std::vector<ServerConfig>&	getServerConfigs() const;
			const ServerConfig&					getServerConfigs(size_t index) const;
	
	private:
		std::string					_filename;
		std::string					_filecontent;
		std::vector<ServerConfig>	_serverConfigs;

		void	readConfigfile();
		void	syntaxErrorCheck() const;
		void	semicolonsErrorCheck() const;
		void	createServerConfig();
};

#endif
