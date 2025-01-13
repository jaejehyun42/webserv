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
			cout << "\rWaiting..." << flush;
			int nev = serv.getKevent();

			for (int i = 0; i < nev; i++)
			{
				const struct kevent& event = serv.getEvList(i);

				if (serv.getServerIdx(event.ident) != -1)
					serv.acceptClient(event.ident);
				else if (serv.getClientIdx(event.ident) != -1)
				{
					if (event.flags & EV_EOF)
						serv.closeClient(event.ident);
					else if (event.filter == EVFILT_READ)
						serv.readClient(event.ident);
					else if (event.filter == EVFILT_WRITE)
						serv.sendClient(event.ident);
				}
				else
					throw runtime_error("Error: Failure to kqueue event handler");
			}
			serv.checkTimeout(conf.getAliveTime());
		}
	}
	catch(const exception& e)
	{
		cerr << e.what() << '\n';
	}
		
	return (0);
}