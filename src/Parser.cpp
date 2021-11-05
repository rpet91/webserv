#include "Parser.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

Parser::~Parser(void)
{
}

Parser::Parser(void)
{
}

Parser::Parser(const Parser& src)
{
	*this = src;
}

Parser& Parser::operator=(const Parser& src)
{
	this->_filename = src._filename;
	this->_filecontent = src._filecontent;
	this->_serverConfigs = src._serverConfigs;
	return (*this);
}

void	Parser::init(const std::string& configFile)
{
	this->_filename = configFile;
	readConfigfile();
	syntaxErrorCheck();
	semicolonsErrorCheck();
	createServerConfig();
}

size_t	Parser::getAmountServers() const
{
	return (this->_serverConfigs.size());
}

const std::vector<ServerConfig>&	Parser::getServerConfigs() const
{
	return (this->_serverConfigs);
}

const ServerConfig&	Parser::getServerConfigs(size_t index) const
{
	return (this->_serverConfigs[index]);
}

void	Parser::readConfigfile()
{
	std::ifstream	ifs;

	ifs.open(this->_filename);
	if (!ifs)
		throw std::runtime_error("input filestream open error");
	getline(ifs, this->_filecontent, '\0');
	ifs.close();
}

void	Parser::syntaxErrorCheck() const
{
	size_t	countOpen = 0;
	size_t	countClose = 0;
	size_t	pos = 0;

	while (pos != std::string::npos)
	{
		if (this->_filecontent[pos] == '{')
			countOpen++;
		else if (this->_filecontent[pos] == '}')
			countClose++;
		pos = this->_filecontent.find_first_of("{}", pos + 1);
	}
	if (countOpen > countClose)
		throw std::runtime_error("configfile: missing ('}')");
	if (countClose > countOpen)
		throw std::runtime_error("configfile: extraneous closing brace ('}')");
}

static bool	isEndSemicolon(const std::string& str)
{
	size_t pos = str.find_last_not_of(" \t\n");
	if (pos == std::string::npos)
		return (false);
	return (str[pos] == ';');
}

void	Parser::semicolonsErrorCheck() const
{
	std::string					str = this->_filecontent;
	size_t						pos;
	std::vector<std::string> 	lines;

	StringUtils::splitNoBraces(str, "\n", lines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (isEndSemicolon(lines[i]) == false)
		{
			pos = lines[i].find_last_not_of(" \t\n");
			if (lines[i][pos] == '}')
				continue;
			pos = lines[i].find_last_not_of(" \t\n");
			if (lines[i][pos] == '{')
				continue;
			if (i + 1 < lines.size())
				i++;
			else
				throw std::runtime_error("configfile: missing semicolon (1)");
			pos = lines[i].find_last_not_of(" \t\n");
			if (lines[i][pos] == '{')
				continue;
			throw std::runtime_error("configfile: missing semicolon (2)");
		}
	}
}

void	Parser::createServerConfig()
{
	const std::string			id = "server";
	std::vector<std::string>	lines;

	StringUtils::splitServers(this->_filecontent, lines);
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (!lines[i].compare(0, id.size(), id))
		{
			std::string serverBlock = lines[i].substr(id.size(), lines[i].length() - id.size());
			ServerConfig newServer;
			newServer.init(serverBlock);
			this->_serverConfigs.push_back(newServer);
		}
		else
			throw std::runtime_error("configfile: unidentified input");
 	}
	if (this->_serverConfigs.size() == 0)
		throw std::runtime_error("configfile: missing server(s)");
}
