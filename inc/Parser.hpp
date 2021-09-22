#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "ServerConfig.hpp"

class Parser
{
	public:
		//constructors()
			~Parser();
			Parser();
			Parser(std::string filename);
			Parser(Parser const& src);
			Parser const& operator=(Parser const& src);

		//get()
			size_t	getNumServers() const;

	private:
		std::string					_filename;
		std::string					_filecontent;
		size_t						_numberOfServers;
		std::vector<Serverconfig>	_serverConfigs;

		void	createServerconfig();
};

#endif
