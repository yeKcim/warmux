/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
#include <WSERVER_debug.h>
#include <WSERVER_index_msg.h>
#include "client.h"
#include "config.h"
#include "net_data.h"
#include "sync_slave.h"
#include "stat.h"

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
}

bool HostOptions::Set(const std::string & _game_name, bool _passwd)
{
  if (game_name != "")
    return false;

  game_name = _game_name;
  passwd = _passwd;

  return true;
}

bool HostOptions::UsePassword() const
{
  return passwd;
}

const std::string& HostOptions::GetGameName() const
{
  return game_name;
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
  if (is_hosting) {

      if (version != "") {
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

  if (config.IsVersionSupported(version)) {

    stats.NewClient(version);
    if (!SendSignature())
      goto error;

  } else if (version == "WIS") {

    DPRINT(MSG, "New index server connected");

    if (!SendInt((int) TS_MSG_WIS_VERSION ))
      goto error;

    if (!SendInt( VERSION ))
      goto error;

  } else {
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
  bool r;
  std::string game_name;
  r = ReceiveStr(game_name);
  if (!r)
    return false;

  int passwd;
  r = ReceiveInt(passwd);
  if (!r)
    return false;

  passwd = !!passwd;
  r = options.Set(game_name, passwd);
  if (!r)
    return false;

  is_hosting = true;

  DPRINT(MSG, "game: version=%s name=%s pwd=%s", version.c_str(), game_name.c_str(), (passwd) ? "yes" : "no");

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
  stats.NewServer(version);
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
  for (std::multimap<std::string, Client*>::iterator client = clients.lower_bound("unknown");
       client != clients.upper_bound("unknown");
       client++) {

    if (client->second == this)
      {
	clients.erase(client);
	return;
      }
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
  r = SendStr(config.SupportedVersions2Str());
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

  nb_s += fake_clients.count(version);

  if (!SendInt(nb_s))
    return false;

  if (nb_s == 0) {
    stats.NewClientWoAnswer(version);
    return true;
  }

  for (std::multimap<std::string, Client*>::iterator client = clients.lower_bound(version);
       client != clients.upper_bound(version);
       client++) {

    if (client->second->is_hosting) {

      if (!SendInt(client->second->GetIP()))
	return false;
      if (!SendInt(client->second->port))
	return false;
      if (!SendInt(client->second->options.UsePassword()))
	return false;
      if (!SendStr(client->second->options.GetGameName()))
	  return false;
    }
  }

  for (std::multimap<std::string, FakeClient>::iterator client = fake_clients.lower_bound(version);
       client != fake_clients.upper_bound(version);
       client++) {

    if (!SendInt(client->second.ip))
      return false;
    if (!SendInt(client->second.port))
      return false;
    if (!SendInt(client->second.options.UsePassword()))
      return false;
    if (!SendStr(client->second.options.GetGameName()))
      return false;
  }

  return true;
}


void Client::NotifyServers(bool joining)
{
  // Tell other index server that a new wormux server just registered
  DPRINT(MSG, "Notify of a new wormux server...");

  for (std::multimap<std::string, Client*>::iterator serv = clients.lower_bound(sync_serv_version);
       serv != clients.upper_bound(sync_serv_version);
       serv++) {

    if (!serv->second->SendInt(TS_MSG_JOIN_LEAVE))
      return /*false*/;
    if (!serv->second->SendStr(version))
      return /*false*/;
    if (!serv->second->SendInt(GetIP()))
      return /*false*/;
    // Send the port number : if this client is leaving, send -port
    if (!serv->second->SendInt(joining? port : -port))
      return /*false*/;

    if (joining) {
      if (!serv->second->SendStr(serv->second->options.GetGameName()))
	return /*false*/;
      if (!serv->second->SendInt(serv->second->options.UsePassword()))
	return /*false*/;
    }
  }

  return /*true*/;
}

