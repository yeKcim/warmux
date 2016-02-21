#ifndef CLIENT_H
#define CLIENT_H
#include <netinet/in.h>
#include <string>
#include "../../src/network/index_svr_msg.h"
#include "net_data.h"

class Client : public NetData
{
	std::string version;
	bool handshake_done;
	bool is_hosting;
	int port; // port number where a game is hosted

	bool SendSignature();
	bool SendList();

	void SetVersion(const std::string & ver);
public:
	Client(int client_fd,unsigned int & ip);
	~Client();

	// Return false if the client closed the connection
	bool HandleMsg(const std::string & str);

	// Tell other index server that a new wormux server just registered
	void NotifyServers(bool joining);
};

class FakeClient
{
public:
	FakeClient(const int & _ip, const int & _port) 
	{
		ip = _ip;
		port = _port;
	}
	int ip;
	int port;
};
	
#endif
