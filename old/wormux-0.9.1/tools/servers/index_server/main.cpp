/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************/

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
#include <iostream>

#include <WSERVER_debug.h>
#include <WSERVER_env.h>
#include "config.h"
#include "client.h"
#include "clock.h"
#include "download.h"
#include "server.h"
#include "sync_slave.h"
#include "stat.h"

// map < version, client >
std::multimap<std::string, Client*> clients;

std::string config_file = "wormux_index_server.conf";

void ShowUsage(void)
{
  std::cout << "Wormux Index Server (" << PACKAGE_VERSION <<")" << std::endl
	    << "===================" << std::endl
	    << "Usage: wormux-inder_server [OPTIONS]" << std::endl
	    << "       -d             : run as daemon" << std::endl
	    << "       -f config_file : set an alternative configuration file" << std::endl;
  exit(EXIT_FAILURE);

  return;
}

void DoFork(void)
{
  pid_t fwis = fork();
  switch (fwis) {
    case EAGAIN:
      DPRINT(INFO, "Cannot fork due to system restriction. Try to increase KERN_MAXPROC, KERN_MAXPROCPERUID or RLIMIT_NPROC.");
      exit(EXIT_FAILURE);
      break;
    case ENOMEM:
      DPRINT(INFO, "Cannot fork due to insufficient swap or memory space.");
      exit(EXIT_FAILURE);
      break;
    case 0: // Forked successfully
      break;
    default:
      exit(EXIT_SUCCESS);
      break;
  }
}

void parseArgs(int argc, char *argv[])
{
  int opt;

  while ((opt = getopt(argc, argv, "f:d")) != -1) {
    switch (opt) {
    case 'f':
      config_file = optarg;
      break;
    case 'd':
      DoFork();
      break;
    case '?':
    default:
      ShowUsage();
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  parseArgs(argc, argv);

  config.Load(config_file);

  DPRINT(INFO, "Wormux index server version %i", VERSION);
  DPRINT(INFO, "%s", wx_clock.DateStr());

  Env::SetConfigClass(config);
  Env::SetWorkingDir();

  bool local, r;
  r = config.Get("local", local);
  if (!r)
    PRINT_FATAL_ERROR;

  if (!local)
    DownloadServerList();

  Env::SetChroot();

  Env::MaskSigPipe();

  // Set the maximum number of connection
  Env::SetMaxConnection();

  stats.Init();

  int port = 0;
  config.Get("port", port);
  Server listen_sock(port);

  // Set of socket where an activity have been detected
  fd_set acting_sock_set;

  if (!local)
    sync_slave.Start();

  while(1)
    {
      wx_clock.HandleJobs(local);
      sync_slave.CheckGames();

      struct timeval timeout;
      memset(&timeout, 0, sizeof(timeout));
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      acting_sock_set = listen_sock.GetSockSet();
      // Lock the process until activity is detected
      if ( select(FD_SETSIZE, &acting_sock_set, NULL, NULL, &timeout) < 1 )
        {
          // Timeout to check for other games on other servers
          if(timeout.tv_sec != 0 && timeout.tv_usec != 0)
            PRINT_FATAL_ERROR;
        }

      // Find the socket where activity have been detected:
      // First check for already established connections
      for(std::multimap<std::string,Client*>::iterator client = clients.begin();
          client != clients.end();
          ++client)
        {
          client->second->CheckState();

          if( ! client->second->connected )
            {
              // Connection closed
              DPRINT(CONN, "Closind FD %u from client %p\n", client->second->GetFD(), client->second);
              listen_sock.CloseConnection( client->second->GetFD() );
              delete client->second;
              clients.erase(client);
              DPRINT(CONN, "%i connections up!", (int)clients.size());
              break;
            }
          if( FD_ISSET( client->second->GetFD(), &acting_sock_set) )
            {
              if( ! client->second->Receive() ) {
                DPRINT(CONN, "Nothing received from client %p, disconnecting!", client->second);
                client->second->connected = false;
              }
              // Exit as the clients list may have changed
              break;
            }
        }

      // Then check if there is any new incoming connection
      if( FD_ISSET(listen_sock.GetFD(), &acting_sock_set) )
        {
          Client* client = listen_sock.NewConnection();
          if(client != NULL)
            clients.insert(std::make_pair("unknown", client ));
          DPRINT(CONN, "%i connections up!", (int)clients.size());

          // Exit as the 'clients' list may have changed
        }
    }
}

