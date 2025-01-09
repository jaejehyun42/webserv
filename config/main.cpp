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
		unordered_map<string, LocBlock>::iterator it = sc.getServBlock(0).getPathIter("/test/abc/def/ghi/123/456/789");
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