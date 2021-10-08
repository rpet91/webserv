#include "Parser.hpp"				// Parser
#include "WebServer.hpp"			// WebServer
#include <iostream>					// std::cout, std::cerr
#include <sys/select.h>				// select

int		main(int argc, char **argv)
{
	Parser			parser;
	WebServer		webServer;
	std::string		configFile;

	if (argc == 1)
		configFile = "config/default_configfile.conf";
	else if (argc == 2)
		configFile = argv[1];
	else
	{
		std::cout << "Too many arguments" << std::endl;
		return EXIT_FAILURE;
	}
	try
	{
		parser.init(configFile);
		webServer.setup(parser.getServerConfigs());
		webServer.run();
	}
	catch (std::exception &e)
	{
		std::cout << "main error" << std::endl;
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
