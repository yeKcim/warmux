/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "server.h"
#include "client.h"
#include "config.h"
#include "debug.h"
#include "sync_slave.h"
#include "clock.h"
#include "download.h"
#include "stat.h"

// map < version, client >
std::multimap<std::string, Client*> clients;

void SetChroot()
{
  // If root, do chroot
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
  DPRINT(INFO, "Wormux index server version %i", VERSION);
  DPRINT(INFO, "%s", wx_clock.DateStr());

  std::string working_dir;
  config.Get("working_dir", working_dir);

  DPRINT(INFO, "Entering folder %s", working_dir.c_str());
  if(chdir(working_dir.c_str()) == -1)
    TELL_ERROR;

  bool local;
  config.Get("local", local);
  if (!local)
    DownloadServerList();

  bool chroot_opt;
  config.Get("chroot", chroot_opt);

  // Attempt chroot only when root
  if (chroot_opt && !getgid())
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

  // Set the maximum number of connection
  int max_conn = SetMaxConnection();
  DPRINT(INFO, "Number of connexions allowed : %i", max_conn);

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
            TELL_ERROR;
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
              listen_sock.CloseConnection( client->second->GetFD() );
              delete client->second;
              clients.erase(client);
              DPRINT(CONN, "%i connections up!", (int)clients.size());
              break;
            }
          if( FD_ISSET( client->second->GetFD(), &acting_sock_set) )
            {
              if( ! client->second->Receive() ) {
                DPRINT(CONN, "Nothing received, disconnecting!");
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
