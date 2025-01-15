#include "ServConf.hpp"

int main(int argc, char** argv)
{
	std::string fileName; 
	if (argc == 1)
		fileName = "../file/nginx.conf";
	else if (argc == 2)
		fileName = argv[1];
	else
		exit(1);

	try
	{
		ServConf sc(fileName);
		for (const auto& pair : sc.getServBlock(0).getPath()) {
			std::cout << "Key: " << pair.first << std::endl;
		}
		unordered_map<string, LocBlock>::const_iterator it = sc.getServBlock(0).getPathIter(".py");
		if (it != sc.getServBlock(0).getPath().end())
            cout << "Path found: \"" << it->first << "\"" << endl;
		else
            cout << "Path not found." << endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return (0);
}