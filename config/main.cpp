#include "ServConf.hpp"

int main(int argc, char** argv)
{
	std::string fileName; 
	if (argc == 1)
		fileName = "file/nginx.conf";
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

<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes
		const ServBlock& sb = sc.getServBlock(0);
		cout << sb.getReturn().first << ", " << sb.getReturn().second << endl;

		unordered_map<string, LocBlock>::const_iterator it = sc.getServBlock(0).getPathIter("/return");
		if (it != sc.getServBlock(0).getPath().end())
		{
			cout << "Path found: \"" << it->first << "\"" << endl;
			cout << it->second.getReturn().first << ", " << it->second.getReturn().second << endl;
		}
		else
			cout << "Path not found." << endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

<<<<<<< Updated upstream
=======
=======
		unordered_map<string, LocBlock>::const_iterator it = sc.getServBlock(0).getPathIter("/return");
		if (it != sc.getServBlock(0).getPath().end())
		{
			cout << "Path found: \"" << it->first << "\"" << endl;
			cout << it->second.getReturn().first << ", " << it->second.getReturn().second << endl;
		}
		else
			cout << "Path not found." << endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

>>>>>>> Stashed changes
>>>>>>> Stashed changes
	return (0);
}