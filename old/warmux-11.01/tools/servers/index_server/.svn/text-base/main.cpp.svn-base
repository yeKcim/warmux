/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include <getopt.h>
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

std::string config_file = "warmux_index_server.conf";

void printUsage(char *argv[])
{
  printf("Usage: %s [OPTIONS]\n", argv[0]);
  printf("OPTIONS:\n"
	 "  -h|--help: print this help and exit\n"
	 "  -v|--version: print version and exit\n"
	 "  -d|--daemon: start as daemon (in background)\n"
	 "  -f|--file: specify config file\n"
	 );
  printf("\nConfig file is reloaded when receiving a -HUP signal BUT some configuration options need program to be restarted\n");
}

void parseArgs(int argc, char *argv[])
{
  int opt;

  struct option long_options[] = {
    {"help",	     no_argument,       NULL, 'h'},
    {"version",	     no_argument,       NULL, 'v'},
    {"daemon",       no_argument,       NULL, 'd'},
    {"file",         required_argument, NULL, 'f'},
    {NULL,           no_argument,       NULL,  0 }
  };

  while ((opt = getopt_long(argc, argv, "hvdf:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      printUsage(argv);
      exit(EXIT_SUCCESS);
      break;

    case 'v':
      printf("Warmux index server version %s\n", PACKAGE_VERSION);
      exit(EXIT_SUCCESS);
      break;

    case 'd':
      Env::Daemonize();
      break;

    case 'f':
      config_file = optarg;
      break;

    case '?': /* returns by getopt if option was invalid */
      printUsage(argv);
      exit(EXIT_FAILURE);
      break;

    default:
      fprintf(stderr, "Sorry, it seems that option '-%c' is not implemented!\n", opt);
      exit(EXIT_FAILURE);
      break;
    }
  }
}

int main(int argc, char* argv[])
{
  parseArgs(argc, argv);

  config.Load(config_file);

  DPRINT(INFO, "Warmux index server version %i", VERSION);
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
  Env::SetupAutoReloadConf();

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

