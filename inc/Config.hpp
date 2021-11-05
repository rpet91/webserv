#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>

enum	e_httpMethods
{
	GET,
	POST,
	DELETE
};

class Config
{
	public:
		//constructors
		virtual ~Config();
		Config();
		Config(const Config&);
		Config&	operator=(const Config&);

		//get()
		size_t  									getLimitClientBodySize() const;
		const std::string&							getRoot() const;
		const std::vector<std::string>&				getIndex() const;
		bool    									getAutoindex() const;
		bool    									getHttpMethods(enum e_httpMethods method) const;
		const std::string&							getErrorPage(size_t) const;
		bool										hasErrorPage(size_t) const;
		bool										hasCgi() const;
		const std::string							getCgi(const std::string&) const;
		bool										newHasCgi(const std::string& string) const;
		const std::string							newGetCgi(const std::string& string);
		const std::string&							getUploadDir() const;
		size_t										getAmountErrorPages() const;
		size_t										getAmountCgi() const;
		const std::map<size_t, std::string>&		getMapErrorPages() const;
		const std::map<std::string, std::string>&	getMapCgi() const;

		//set()
		void	setLimitClientBodySize(const std::string&);
		void	setRoot(const std::string&);
		void	setIndex(const std::string&);
		void	setAutoindex(const std::string&);
		void	setHttpMethods(const std::string&);
		void	setCgi(const std::string&);
		void	setUploadDir(const std::string&);
		void	setErrorPage(const std::string&);

	protected:
		size_t								_limitClientBodySize;
		std::string							_root;
		std::vector<std::string>			_index;
		bool								_autoindex;
		std::map<size_t, std::string>		_errorPage;
		bool								_httpMethods[3];
		std::map<std::string, std::string>	_cgi;
		std::string							_uploadDir;
		size_t								_amountErrorpages;
		size_t								_amountCgi;
};

#endif
