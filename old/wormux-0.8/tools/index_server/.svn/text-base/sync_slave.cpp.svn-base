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

#include <list>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "sync_slave.h"
#include "config.h"
#include "client.h"
#include "net_data.h"
#include "download.h"
#include "debug.h"
#include "stat.h"
#include "../../src/network/index_svr_msg.h"

SyncSlave sync_slave;

// map < version, ip >
std::multimap<std::string, FakeClient> fake_clients;

SyncSlave::SyncSlave()
{
}

SyncSlave::~SyncSlave()
{
  for(iterator it = begin();
      it != end();
      ++it)
    {
      delete (it->second);
    }
}


static ssize_t getline(std::string& line, std::ifstream& file)
{
  line.clear();
  std::getline(file, line);
  if(file.eof())
    return -1;
  return line.size();
}

bool SyncSlave::Start()
{
  const std::string server_fn = "./server_list";
  DPRINT(TRAFFIC, "Contacting other servers ...");
  std::ifstream fin;
  fin.open(server_fn.c_str(), std::ios::in);
  if (!fin)
    return false;

  ssize_t read;
  std::string line;

  std::string my_hostname;
  config.Get( "my_hostname", my_hostname);

  // GNU getline isn't available on *BSD and Win32, so we use a new function, see getline above
  while ((read = getline(line, fin)) >= 0)
    {
      if (line.size() == 0 || line.at(0) == '#')
        continue;

      std::string::size_type port_pos = line.find(':', 0);
      if (port_pos == std::string::npos)
        continue;

      std::string hostname = line.substr(0, port_pos);
      std::string portstr = line.substr(port_pos+1);
      int port = atoi(portstr.c_str());

      if (hostname != my_hostname && find(hostname) == end())
        {
          IndexServerConn* c = new IndexServerConn(hostname, port);
          if (c->connected)
            insert(make_pair(hostname, c));
          else
            delete c;
        }
    }
  fin.close();
  return true;
}

void SyncSlave::CheckGames()
{
  iterator it = begin();
  while (it != end())
    {
      int received;
      bool result = false;

      if (ioctl( it->second->GetFD(), FIONREAD, &received) == -1)
        {
          PRINT_ERROR;
          result = false;
        }

      if (received == 0 || it->second->Receive())
        result = true;

      if (!result || ! it->second->connected)
        {
          DPRINT(INFO, "Index server at %s disconnected", it->first.c_str());
          delete it->second;
          erase(it);
          it = begin();
        }
      else
        {
          it->second->CheckState();
          ++it;
        }
    }
}


IndexServerConn::IndexServerConn(const std::string &addr, int port)
{
  DPRINT(INFO, "Contacting index server at %s ...",addr.c_str());
  if(!ConnectTo( addr, port))
    return;
  SendInt((int)TS_MSG_VERSION);
  SendStr("WIS");
}

IndexServerConn::~IndexServerConn()
{
  close(GetFD());
}

bool IndexServerConn::HandShake()
{
  std::string key;
  if (!ReceiveStr(key))
    return false;

  if (key != "MassMurder!")
    {
      DPRINT(INFO,"Doesn't seem do be a valid server...");
      return false;
    }
  DPRINT(INFO,"Server identified...");
  return true;
}

bool IndexServerConn::HandleMsg(enum IndexServerMsg msg_id)
{
  switch(msg_id)
    {
    case TS_MSG_VERSION:
      return HandShake();
      break;

    case TS_MSG_WIS_VERSION:
      {
        int version;
        if (received < 4) // The message is not completely received
          return true;
        if (!ReceiveInt(version)) // Receive the version number of the server index
          return false;
        if (version > VERSION)
          {
            DPRINT(INFO,"The server at %i have a version %i, while we are only running a %i version",
                   GetIP(), version, VERSION);
            exit(EXIT_FAILURE);
          }
        else if(version < VERSION)
          {
            DPRINT(INFO,"This server is running an old version (v%i) !", version);
            return false;
          }
        DPRINT(INFO,"We are running the same version..");
      }
      break;
    case TS_MSG_JOIN_LEAVE:
      {
        int ip;
        int port;
        std::string server_id;

        if (!ReceiveStr(server_id))
            return false;

        if (received < 8) // The message is not completely received
          return true;
        if (!ReceiveInt(ip)) // Receive the IP of the wormux server
          return false;
        if (!ReceiveInt(port)) // Receive the port of the wormux server
          return false;

        if (port < 0) // means it disconnected
          {
            std::multimap<std::string, FakeClient>::iterator serv = fake_clients.find( server_id );
            if( serv != fake_clients.end() )
              {
                do
                  {
                    if( serv->second.ip == ip
                        &&  serv->second.port == -port )
                      {
                        fake_clients.erase(serv);
                        DPRINT(MSG, "A fake server disconnected");
                        break;
                      }
                  } while (serv != fake_clients.upper_bound(server_id));
              }
          }
        else
          {
            int host_options;
            if (!ReceiveInt(host_options)) // Does the server publish a name and a passwd ?
              return false;

            HostOptions options;
            options.used = host_options;
            if (options.used) {
              std::string game_name;
              if (!ReceiveStr(game_name))
                return false;
              
              int passwd;
              if (!ReceiveInt(passwd))
                return false;
              options.Set(game_name, passwd);
            }

            fake_clients.insert( std::make_pair(server_id, FakeClient(ip, port, options)));
            stats.NewFakeServer();
          }
      }
      break;
    default:
      DPRINT(INFO,"Bad message!");
      return false;
    }
  msg_id = TS_NO_MSG;
  return true;
}
