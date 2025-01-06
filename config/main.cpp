#include "ServConf.hpp"

int main(int argc, char** argv)
{
	std::string fileName; 
	if (argc == 1)
		fileName = "./file/nginx.conf";
	else if (argc == 2)
		fileName = argv[1];
	else
		exit(1);

	try
	{
		ServConf sc;
		sc.parse(fileName);
		sc.print();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return (0);
}