#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>

enum	e_httpMethods
{
	GET,
	POST,
	DELETE
};

enum	e_cgi
{
	EXTENSION,
	PATH
};

class	LocationConfig
{
	public:
		//constructors()
			virtual ~LocationConfig();
			LocationConfig();
			LocationConfig(const LocationConfig&);
			LocationConfig& operator=(const LocationConfig&);
			void			init(const std::string&, size_t body, bool autoindex, bool methods[3]);

		//get()
			const std::string&				getPath() const;
			const std::string&				getRoot() const;
			size_t							getLimitClientBodySize() const;
			const std::vector<std::string>&	getIndex() const;
			bool							getAutoindex() const;
			bool							getHttpMethods(enum e_httpMethods method) const;
			const std::string&				getCgi(enum e_cgi arg) const;

		//set()
			void	setRoot(const std::string&);
			void	setLimitClientBodySize(const std::string&);
			void	setIndex(const std::string&);
			void	setAutoindex(const std::string&);
			void	setHttpMethods(const std::string&);
			void	setCgi(const std::string&);

	private:
		std::string					_path;
		std::string					_root;
		size_t						_limitClientBodySize;
		std::vector<std::string>	_index;
		bool						_autoindex;
		bool						_httpMethods[3];
		std::string					_cgi[2];
};

template <typename T>	//debug
void	printVec(const std::vector<T>& vec)
{
	//std::cout << "vector size: " << vec.size() << std::endl;
	for (size_t i = 0 ; i < vec.size(); i++)
		std::cout << "\t[" << i << "]:" << "[" << vec[i] << "]" << std::endl;
}

std::ostream &operator<<(std::ostream& out, LocationConfig const& loc);

#endif


