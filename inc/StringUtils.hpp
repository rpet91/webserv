#ifndef StringUtils_HPP
# define StringUtils_HPP

# include <string>
# include <vector>

# define Whitespaces " \v\t\n"

class StringUtils
{
	public:
		static void			trim(std::string&, const char*);
		static void			trimBraces(std::string&);
		static std::string	substrDelimiter(size_t *startPosition, std::string const &source, std::string const &delimiter);
		static void			split(const std::string&, const char*, std::vector<std::string>&);
		static void			splitNoBraces(const std::string&, const char*, std::vector<std::string>&);
		static void			splitServers(const std::string&, std::vector<std::string>&);
		static void			matchIdentifier(const std::string&, std::string&);
		static void			lowerCase(std::string& string);
};

#endif
