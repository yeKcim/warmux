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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include "client.h"
#include "config.h"
#include "net_data.h"
#include "debug.h"
#include "sync_slave.h"
#include "stat.h"
#include "../../src/network/index_svr_msg.h"

// map < version, client >
extern std::multimap<std::string,Client*> clients;
// map < version, ip >
extern std::multimap<std::string,FakeClient> fake_clients;

// number of server currently logged depending on the version:
std::map<std::string, int> nb_server;

HostOptions::HostOptions()
{
  game_name = "";
  passwd = false;
  used = false;
}

bool HostOptions::Set(const std::string & _game_name, bool _passwd)
{
  if (used) {
    DPRINT(MSG, "Game Name and passwd already set");
    return false;
  }

  game_name = _game_name;
  passwd = _passwd;
  used = true;

  return true;
}


Client::Client(int client_fd, unsigned int & ip)
{
  version = "";

  handshake_done = false;
  is_hosting = false;
  Host(client_fd, ip);

  DPRINT(MSG, "Client: Creation of client(%d, %d)\n", client_fd, ip);
}

Client::~Client()
{
  if( is_hosting )
    {
      if( version != "" )
        {
          nb_server[ version ]--;
          NotifyServers( false );
        }
    }

  DPRINT(MSG, "Client: Deletion of client(%d, %d)\n", GetFD(), GetIP());
}

bool Client::HandShake(const std::string & version)
{
  if (handshake_done)
    return false;

  if (version == "0.8"
      || version == "0.8.1"
      || version == "0.8.2"
      || version == "0.8.1svn")
    {
      stats.NewClient();
      if (!SendSignature())
	goto error;
    }
  else if (version == "WIS")
    {
      DPRINT(MSG, "New index server connected");

      if (!SendInt((int) TS_MSG_WIS_VERSION ))
	goto error;

      if (!SendInt( VERSION ))
	goto error;
    }
  else
    {
      DPRINT(MSG, "Bad client version: %s", version.c_str());
      RejectBadVersion();
      goto error;
    }

  AddMeToClientsList( version );
  msg_id = TS_NO_MSG;

  handshake_done = true;
  DPRINT(MSG, "Handshake : OK");
  return true;

 error:
  DPRINT(MSG, "Handshake failure");
  return false;
}

bool Client::RegisterWormuxServer()
{
  if (BytesReceived() < 4)
    return true;

  DPRINT(MSG, "This is a server (%s)", version.c_str());
  is_hosting = true;

  if (nb_server.find(version) != nb_server.end())
    nb_server[ version ]++;
  else
    nb_server[ version ] = 1;

  if (!ReceiveInt(port))
    return false;

  // try opening a connection to see if it's firewalled or not
  unsigned char* str_ip = (unsigned char*)&(GetIP());
  char formated_ip[16];
  snprintf(formated_ip, 16, "%i.%i.%i.%i", (int)str_ip[0],
           (int)str_ip[1],
           (int)str_ip[2],
           (int)str_ip[3]);

  int lfd = NetData::GetConnection(formated_ip, port);

  if (lfd == -1) {
    DPRINT(MSG, "server is not reachable");
    // answer to the server
    if (!SendStr("UNREACHABLE"))
      goto err_send;

    return false;
  }
  close(lfd);

  DPRINT(INFO, "Connection to %s:%i : OK", formated_ip, port);
  if (!SendStr("OK"))
    goto err_send;

  NotifyServers(true);
  stats.NewServer();
  return true;

 err_send:
  DPRINT(MSG, "Fail to register new game server");
  return false;
}

bool Client::HandleMsg(enum IndexServerMsg msg_id)
{
  bool r = true;

  if (msg_id == TS_MSG_VERSION)
    {
      std::string clt_version;
      r = ReceiveStr(clt_version);
      if (!r) {
        DPRINT(TRAFFIC, "didn't receive client version string");
        goto next_msg;
      }

      DPRINT(TRAFFIC, "client version: %s", clt_version.c_str());
      r = HandShake(clt_version);
      goto next_msg;
    }

  if (!handshake_done)
    {
      r = false;
      goto next_msg;
    }

  switch(msg_id)
    {
    case TS_MSG_HOSTING:
      r =  RegisterWormuxServer();
      break;

    case TS_MSG_GET_LIST:
      if (is_hosting) {
        r = false;
      } else {
        r = SendList();
      }
      break;

    case TS_MSG_REGISTER_GAME:
      {
        std::string game_name;
        r = ReceiveStr(game_name);
        if (!r)
          goto next_msg;

        int passwd;
        r = ReceiveInt(passwd);
        if (!r)
          goto next_msg;

        passwd = !!passwd;
        r = options.Set(game_name, passwd);
        DPRINT(MSG, "game: name=%s pwd=%s", game_name.c_str(), (passwd) ? "yes" : "no");
      }
      break;

    default:
      DPRINT(MSG, "Wrong message");
      return false;
    }

 next_msg:
  // We are ready to read a new message
  msg_id = TS_NO_MSG;
  return r;
}

void Client::AddMeToClientsList(const std::string & ver)
{
  DPRINT(MSG, "Setting version to %s", ver.c_str());
  version = ver;
  clients.insert( std::make_pair(version, this) );

  // We are currently registered as an unknown version
  // So, we unregister it:
  std::multimap<std::string, Client*>::iterator client;
  client = clients.find( "unknown" );
  if (client != clients.end())
    {
      do
        {
          if (client->second == this)
            {
              clients.erase(client);
              return;
            }
          ++client;
        } while (client != clients.upper_bound( "unknown" ));
    }
}

bool Client::SendSignature()
{
  bool r;
  DPRINT(MSG, "Sending signature");

  r = SendInt((int)TS_MSG_VERSION);
  if (!r)
    goto err_send;
  r = SendStr("MassMurder!");
  if (!r)
    goto err_send;

  return true;

 err_send:
  DPRINT(MSG, "Fail to send signature");
  return false;
}

bool Client::RejectBadVersion()
{
  bool r;
  DPRINT(MSG, "Rejecting wrong version client");

  r = SendInt((int)TS_MSG_VERSION);
  if (!r)
    goto err_send;
  r = SendStr("Bad version");
  if (!r)
    goto err_send;
  r = SendStr("0.8, 0.8.1, 0.8.2, 0.8.1svn");
  if (!r)
    goto err_send;

  return true;

 err_send:
  DPRINT(MSG, "Fail to send bad version msg");
  return false;
}

bool Client::SendList()
{
  DPRINT(MSG, "Sending list...");

  int nb_s = 0;
  if (nb_server.find( version ) != nb_server.end())
    nb_s = nb_server[ version ];

  // TODO : replace this loop by the correct stl function
  std::multimap<std::string, FakeClient>::iterator fclient = fake_clients.find(version);
  if (fake_clients.find( version ) != fake_clients.end())
    {
      do
        {
          nb_s++;
          ++fclient;
        } while (fclient != fake_clients.upper_bound(version));
    }

  if (!SendInt(nb_s))
    return false;

  std::multimap<std::string, Client*>::iterator client = clients.find(version);
  if (client != clients.end())
    {
      do
        {
          if (client->second->is_hosting)
            {
              if (!SendInt(client->second->GetIP()))
                return false;
              if (!SendInt(client->second->port))
                return false;
              if (!SendInt(client->second->options.passwd))
                return false;

              if (client->second->options.used) {
                if (!SendStr(client->second->options.game_name))
                  return false;
              }
            }
          ++client;
        } while (client != clients.upper_bound(version));
    }

  fclient = fake_clients.find(version);
  if (fclient != fake_clients.end())
    {
      do
        {
          if (!SendInt(fclient->second.ip))
            return false;
          if (!SendInt(fclient->second.port))
            return false;

          if (fclient->second.options.used) {
            if (!SendStr(fclient->second.options.game_name))
              return false;

//             if (fclient->second.options.passwd != "") {
//               if (!SendInt(1))
//                 return false;
//             } else {
//               if (!SendInt(0))
//                 return false;
//             }
          }

          ++fclient;
        } while (fclient != fake_clients.upper_bound(version));
    }
  return true;
}


void Client::NotifyServers(bool joining)
{
  // Tell other index server that a new wormux server just registered
  DPRINT(MSG, "Notify of a new wormux server...");

  std::multimap<std::string, Client*>::iterator serv = clients.find( sync_serv_version );
  if (serv != clients.end())
    {
      do
        {
          if (!serv->second->SendInt(TS_MSG_JOIN_LEAVE))
            return /*false*/;
          if (! serv->second->SendStr(version))
            return /*false*/;
          if (! serv->second->SendInt(GetIP()))
            return /*false*/;
          // Send the port number : if this client is leaving, send -port
          if (! serv->second->SendInt(joining? port : -port))
            return /*false*/;

          if (joining) {
            if (!SendInt(serv->second->options.used))
              return /*false*/;

            if (serv->second->options.used) {
              if (!SendStr(serv->second->options.game_name))
                return /*false*/;
//               if (!SendStr(serv->second->options.passwd))
//                 return /*false*/;
            }
          }

          ++serv;
        } while (serv != clients.upper_bound(sync_serv_version));
    }
  return /*true*/;
}

