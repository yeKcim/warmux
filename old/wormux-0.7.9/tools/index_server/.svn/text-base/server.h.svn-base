#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <stdio.h>

class Client;

class Server
{
	int fd;
	fd_set sock_set;
	struct sockaddr_in address;

public:
	Server(int port);
	~Server();

	int & GetFD();
	fd_set & GetSockSet();

	// Open a connection requested by a new client
	Client* NewConnection();
	void CloseConnection(int client_fd);
};

#endif
