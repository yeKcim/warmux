#ifndef SYNC_SLAVE_H
#define SYNC_SLAVE_H
#include <list>
#include <string>
#include "net_data.h"

class IndexServerConn;

const std::string sync_serv_version = "WIS";

// List that contains informations about wormux client/server connected
// on other index servers
class SyncSlave : public std::list<IndexServerConn*>
{
public:
	SyncSlave();
	~SyncSlave();
	bool Start();
	void CheckGames();
};

extern SyncSlave sync_slave;

// Connection to an other index server
// We don't send anything to this connection (except the handshake+version), we just receive information
// about new games on this server (this server will open a socket by itself to receive information about
// connections on us).
class IndexServerConn : public NetData
{
public:
	IndexServerConn(const std::string &addr, int port);
	~IndexServerConn();
	bool HandleMsg(const std::string & full_str);
};

#endif
