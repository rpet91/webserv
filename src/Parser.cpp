#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Parser.hpp"

static size_t	getCurlyBraceOpen(std::string str, size_t start)
{
	return (str.find('{', start));
}

static size_t	getCurlyBraceClose(std::string str, size_t start)
{
	size_t	pos = start + 1;
	uint	open_curly = 1;

	while (pos != std::string::npos && open_curly != 0)
	{
		if (str[pos] == '{')
			open_curly++;
		else if (str[pos] == '}')
			open_curly--;
		pos++;
	}
	return (pos);
}

static std::string	getCurlyBraceSubstr(std::string str, size_t start, size_t end)
{
	return (str.substr(start, end - start));
}

static std::string readConfigfile(std::string& filename)
{
	std::string		line;
	std::string		filecontent;
	std::ifstream	ifs;

	ifs.open(filename);
	if (!ifs)
	{
		std::cout << std::strerror(errno) << std::endl;
		exit(0);
	}
	getline(ifs, line);
	filecontent += line;
	while (!ifs.eof())
	{
		getline(ifs, line);
		filecontent += "\n";
		filecontent += line;
	}
	ifs.close();
	return (filecontent);
}

Parser::~Parser(void)
{
}

Parser::Parser(void)
{
}

Parser::Parser(std::string filename) : _filename(filename)
{
	this->_filecontent = readConfigfile(this->_filename);
	createServerconfig();
}

Parser::Parser(Parser const& src)
{
	*this = src;
}

Parser const& Parser::operator=(Parser const& src)
{
	(void)src;
	return (*this);
}

void	Parser::createServerconfig()
{
	size_t	count = 0;
	size_t	pos = 0;

	pos = this->_filecontent.find("server", pos);
	while (pos != std::string::npos)
	{
		count++;
		size_t bodyOpen = getCurlyBraceOpen(this->_filecontent, pos);
		size_t bodyClose = getCurlyBraceClose(this->_filecontent, bodyOpen + 1);
		//std::cout << "" << getCurlyBracesubstr(this->_filecontent, bodyOpen, bodyClose) << std::endl;;
		std::string serverConfBody = getCurlyBraceSubstr(this->_filecontent, bodyOpen, bodyClose);
		Serverconfig newServer(serverConfBody);
		this->_serverConfigs.push_back(newServer);
		pos = this->_filecontent.find("server", bodyClose + 1);
	}
	std::cout << "num of servers: " << this->_serverConfigs.size() << std::endl;
	std::cout << this->_serverConfigs[0] << std::endl;
	std::cout << this->_serverConfigs[1] << std::endl;
}
