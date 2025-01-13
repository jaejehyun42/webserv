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
<<<<<<< HEAD
		ServConf sc;
		sc.parse(fileName);
=======
		ServConf sc(fileName);
>>>>>>> 392c5e196ea4ea03ec73f9307fd51bf55ae9ce2e
		unordered_map<string, LocBlock>::const_iterator it = sc.getServBlock(0).getPathIter("/test/abc/def/ghi/123/456/789");
		if (it != sc.getServBlock(0).getPath().end())
            cout << "Path found: \"" << it->first << "\"" << endl;
		else
            cout << "Path not found." << endl;
<<<<<<< HEAD
=======
		cout << sc.getMime("tttttt") << endl;
>>>>>>> 392c5e196ea4ea03ec73f9307fd51bf55ae9ce2e
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return (0);
}