#ifndef CGI_HPP
# define CGI_HPP

class CGI
{
	public:
		// Coplien form stuff
		CGI();
		CGI(CGI const &src);
		CGI		&operator=(CGI const &src);
		virtual ~CGI();

		// Public variables
		int			fdin[2];
		int			fdout[2];
};

#endif
