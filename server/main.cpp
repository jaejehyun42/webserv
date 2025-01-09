#include "Server.hpp"
#include "../config/ServConf.hpp"

int main(int argc, char** argv)
{
	string config;
	if (argc == 1)
		config = "file/nginx.conf";
	else if (argc == 2)
		config = argv[1];
	else
	{
		cerr << "Usage: ./webserv [config file path]" << endl;
		return (1);
	}

	try
	{
		ServConf conf(config);
		Server serv(conf);

		while (true)
		{
			vector<struct kevent>& evList = serv.getEvList();
			int nev = kevent(serv.getKq(), NULL, 0, evList.data(), 32, NULL);
			if (nev == -1)
				throw runtime_error("kevent: " + string(strerror(errno)));
			for (int i = 0; i < nev; i++)
			{
				// if (evList[i].flags & EV_EOF)
				// {
				// 	close(evList[i].ident);
				// 	cout << "Client disconnected\n";
				// }
				if (evList[i].filter == EVFILT_READ)
				{
					if (serv.getServerIdx(evList[i].ident) != -1)
						serv.acceptClient(evList[i].ident);
					else
						serv.readClient(evList[i].ident);
				}
				else if (evList[i].filter == EVFILT_WRITE)
				{
					if (serv.getClientIdx(evList[i].ident) != -1)
						serv.sendClient(evList[i].ident);
				}
			}
		}
	}
	catch(const exception& e)
	{
		cerr << e.what() << '\n';
	}

	return (0);
}