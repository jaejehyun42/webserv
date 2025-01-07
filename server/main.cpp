#include "Serv.hpp"
#include "../config/ServConf.hpp"

int main(int argc, char** argv)
{
	string configPath;
	if (argc == 1)
		configPath = "file/nginx.conf";
	else if (argc == 2)
		configPath = argv[1];
	else
	{
		cerr << "Usage: ./webserv [config file path]" << endl;
		return 1;
	}

	try
	{
		ServConf sc;
		sc.parse(configPath);

		Serv serv(sc);

		cout << "Server Waiting...\n";
		while (true)
		{
			vector<struct kevent>& evList = serv.getEvList();
			int nev = kevent(serv.getKq(), NULL, 0, evList.data(), 32, NULL);
			if (nev == -1)
				throw runtime_error("kevent: " + string(strerror(errno)));

			for (int i = 0; i < nev; i++)
			{
				if (evList[i].filter == EVFILT_READ)
				{
					if (serv.checkSockfd(evList[i].ident))
					{
						cout << "Here\n";
						int client_fd = accept(evList[i].ident, NULL, NULL);
						if (client_fd == -1)
							throw runtime_error("accept: " + string(strerror(errno)));
						cout << "Client connected\n";

						struct kevent evSet;
						EV_SET(&evSet, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
						if (kevent(serv.getKq(), &evSet, 1, NULL, 0, NULL) == -1)
							throw runtime_error("kevent: " + string(strerror(errno)));
					}
					else
					{
						char buffer[1024];
						int client_fd = evList[i].ident;
						ssize_t n = read(client_fd, buffer, sizeof(buffer));
						if (n > 0)
						{
							buffer[n] = '\0';
							cout << "Received>\n";
							cout << buffer << endl;

							// 임시 메세지
							string message =
							"HTTP/1.1 200 OK\r\n"
							"Date: Sat, 06 Jan 2025 12:00:00 GMT\r\n"
							"Server: SimpleHTTPServer/1.0\r\n"
							"Content-Type: text/html; charset=UTF-8\r\n"
							"Content-Length: 88\r\n"
							"\r\n"
							"<!DOCTYPE html>\n"
							"<html>\n"
							"<head><title>Hello, World!</title></head>\n"
							"<body><h1>Hello, World!</h1></body>\n"
							"</html>";

							if (write(client_fd, message.c_str(), message.size()) == -1)
								throw runtime_error("write: " + string(strerror(errno)));
						}
						else
						{
							close(client_fd);
							cout << "Client disconnected\n";
						}
					}
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