#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <map>

#include "server.h"
#include "client.h"
#include "config.h"
#include "debug.h"
#include "sync_slave.h"

// map < version, client >
std::multimap<std::string, Client*> clients;

void SetChroot()
{
	if(chroot("./") == -1)
		TELL_ERROR;
	if(chdir("/") == -1)
		TELL_ERROR;

	int uid, gid;
	config.Get("chroot_uid", uid);
	config.Get("chroot_gid", gid);

	if(setgid(gid) == -1)
		TELL_ERROR;
	if(setuid(uid) == -1)
		TELL_ERROR;
}

int SetMaxConnection()
{
	// Set the maximum number of file descriptor allowed
	int max_conn;
	config.Get("connexion_max", max_conn);

	struct rlimit limit;

	if( getrlimit(RLIMIT_NOFILE, &limit) == -1 )
		TELL_ERROR;

	// Keep the system default
	if(max_conn == -2)
	{
		DPRINT(INFO, "Number of connexions allowed : system default");
		return limit.rlim_cur;
	}


	// Use the max allowed
	if(max_conn == -1)
	{
		DPRINT(INFO, "Number of connexions allowed : maximum allowed");
		limit.rlim_cur = limit.rlim_max;
	}
	else
	{
		DPRINT(INFO, "Number of connexions allowed : user defined");
		limit.rlim_cur = max_conn;
		limit.rlim_max = max_conn;
	}

	if( setrlimit(RLIMIT_NOFILE, &limit) == -1 )
		TELL_ERROR;
	return limit.rlim_cur;
}

int main(int argc, void** argv)
{
	bool chroot_opt;
	config.Get("chroot", chroot_opt);

	if (chroot_opt)
		SetChroot();

	if(getuid() == 0)
	{
		DPRINT(INFO, "Don't start me as root user!!");
		exit(EXIT_FAILURE);
	}

	// Ignore broken pipe signal (elsewise we would break,
	// as soon as we are trying to write on client that closed)
	if( signal(SIGPIPE, SIG_IGN) == SIG_ERR )
		TELL_ERROR;

	// Contact other index servers:
	sync_slave.Start();
	
	// Set the maximum number of connection
	int max_conn = SetMaxConnection();
	DPRINT(INFO, "Number of connexions allowed : %i", max_conn);

	int port = 0;
	config.Get("port", port);

	Server listen_sock(port);

	// Set of socket where an activity have been detected
	fd_set acting_sock_set;

	while(1)
	{
		DPRINT(TRAFFIC, "Waiting for incoming connections...");
		sync_slave.CheckGames();

		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		acting_sock_set = listen_sock.GetSockSet();
		// Lock the process until activity is detected
		if ( select(FD_SETSIZE, &acting_sock_set, NULL, NULL, &timeout) < 1 )
		{
			// Timeout to check for other games on other servers
			if(timeout.tv_sec == 0 && timeout.tv_usec == 0)
				continue;
			TELL_ERROR;
		}
		
		// Find the socket where activity have been detected:
		// First check for already established connections
		for(std::multimap<std::string,Client*>::iterator client = clients.begin();
			client != clients.end();
			++client)
		{
			if( FD_ISSET( client->second->GetFD(), &acting_sock_set) )
			{
				if( ! client->second->Receive() )
				{
					// Connection closed
					listen_sock.CloseConnection( client->second->GetFD() );
					delete client->second;
					clients.erase(client);
					DPRINT(CONN, "%i connections up!", clients.size());
				}
				// Exit as soon as, we have read a socket as the 'clients' list may have changed
				break;
			}
		}
		// First check if there is any new incoming connection
		if( FD_ISSET(listen_sock.GetFD(), &acting_sock_set) )
		{
			Client* client = listen_sock.NewConnection();
			if(client != NULL)
				clients.insert(std::make_pair("unknown", client ));
			DPRINT(CONN, "%i connections up!", clients.size());
		}
	}
}
