#include "StringUtils.hpp"
#include <string>
#include <vector>
#include <iostream>

#define WHITESPACES " \t\n"

void	StringUtils::trim(std::string& str, const char *chars)
{
	size_t start = str.find_first_not_of(chars);
	if (start == std::string::npos)
		start = 0;
	size_t end = str.find_last_not_of(chars);
	if (end == std::string::npos)
		end = str.length();
	size_t len = end - start + 1;
	str = str.substr(start, len);
}

void	StringUtils::trimBraces(std::string& str)
{
	size_t start = str.find('{');
	size_t end = str.rfind('}');
	size_t len = end - start - 1;
	str = str.substr(start + 1, len);
	trim(str, WHITESPACES);
}

std::string	StringUtils::substrDelimiter(size_t *startPosition, std::string const &source, std::string const &delimiter)
{
	size_t			endPosition;
	std::string		result;

	endPosition = source.find(delimiter, *startPosition);
	if (endPosition != std::string::npos)
	{
		result = source.substr(*startPosition, endPosition - *startPosition);
		*startPosition = endPosition + delimiter.size();
	}
	else
	{
		result = source.substr(*startPosition);
		*startPosition = endPosition;
	}
	return result;
}

static size_t	getCurlyBraceMatch(const std::string& str, size_t curlyBraceOpen)
{
	size_t	pos = curlyBraceOpen;
	int		BraceSubString = 1;

	while (pos < str.length() && BraceSubString != 0)
	{
		pos = str.find_first_of("{}", pos + 1);
		if (str[pos] == '{')
			BraceSubString++;
		else if (str[pos] == '}')
			BraceSubString--;
	}
	return (pos);
}

void	StringUtils::split(const std::string& str, const char* delims, std::vector<std::string>& out)
{
	size_t	posBegin = 0;
	size_t	posEnd;
	size_t  posDelim;
	size_t	braceOpen;
	size_t	braceClose;
	size_t	subLength;

	while (posBegin < str.length())
	{
		braceOpen = str.find('{', posBegin);
		if (braceOpen != std::string::npos)
			braceClose = getCurlyBraceMatch(str, braceOpen);
		posDelim = str.find_first_of(delims, posBegin);
		if (braceOpen < posDelim)
			posEnd = braceClose + 1;
		else
			posEnd = std::min(posDelim, str.length());
		subLength = posEnd - posBegin;
		if (subLength)
		{
			std::string sub = str.substr(posBegin, subLength);
			StringUtils::trim(sub, WHITESPACES);
			out.push_back(sub);
		}
		posBegin = posEnd + 1;
	}
}

void	StringUtils::splitNoBraces(const std::string& str, const char* delims, std::vector<std::string>& out)
{
	size_t	posBegin = 0;
	size_t	posEnd;
	size_t  posDelim;
	size_t	subLength;

	while (posBegin < str.length())
	{
		posDelim = str.find_first_of(delims, posBegin);
		posEnd = std::min(posDelim, str.length());
		subLength = posEnd - posBegin;
		if (subLength)
		{
			std::string sub = str.substr(posBegin, subLength);
			StringUtils::trim(sub, WHITESPACES);
			out.push_back(sub);
		}
		posBegin = posEnd + 1;
	}
}

void	StringUtils::splitServers(const std::string& str, std::vector<std::string>& out)
{
	size_t	posBegin = 0;
	size_t	posEnd;
	size_t	braceOpen;
	size_t	subLength;

	while (posBegin < str.length())
	{
		posBegin = str.find_first_not_of(WHITESPACES, posBegin);
		if (posBegin == std::string::npos)
			return;
		if (str.compare(posBegin, 6, "server"))
			throw std::runtime_error("configfile: not a server");
		braceOpen = str.find_first_not_of(WHITESPACES, posBegin + 6);
		if (braceOpen == std::string::npos)
			throw std::runtime_error("configfile: server missing body");
		if (str[braceOpen] != '{')
			throw std::runtime_error("configfile: server missing body");
		posEnd = getCurlyBraceMatch(str, braceOpen);
		subLength = posEnd - posBegin + 1;
		if (subLength)
		{
			std::string substr = str.substr(posBegin, subLength);
			StringUtils::trim(substr, WHITESPACES);
			out.push_back(substr);
		}
		posBegin = posEnd + 1;
	}
}

void	StringUtils::matchIdentifier(const std::string& str, std::string& identifier)
{
	std::vector<std::string> vec;

	StringUtils::split(str, WHITESPACES, vec);
	identifier = vec[0];
}
