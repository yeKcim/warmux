/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 ******************************************************************************
 * Notify an index server of an opened wormux server
 * Obtain information about running games from an index server
 *****************************************************************************/

#include <assert.h>
#include <SDL_net.h>
#include "network/download.h"
#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "network/index_server.h"
#include "network/index_svr_msg.h"
#include "network/network.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/random.h"

IndexServer::IndexServer():
  socket(),
  ip(),
  sock_set(),
  used(0),
  server_lst(),
  first_server(server_lst.end()),
  current_server(server_lst.end()),
  hidden_server(false),
  connected(false)
{
}

IndexServer::~IndexServer()
{
  server_lst.clear();
  if(connected)
    Disconnect();
}

/*************  Connection  /  Disconnection  ******************/
connection_state_t IndexServer::Connect()
{
  connection_state_t r = CONN_REJECTED;

  MSG_DEBUG("index_server", "Connecting..");
  ASSERT(!connected);

  if( hidden_server )
    return CONNECTED;

  // Download the server if it's empty
  if( server_lst.size() == 0 )
  {
    server_lst = Downloader::GetInstance()->GetServerList("server_list");
    first_server = server_lst.end();
    current_server = server_lst.end();
  }

  // If it's still empty, then something went wrong when downloading it
  if( server_lst.size() == 0 )
    return CONN_BAD_HOST;

  std::string addr;
  int port;
  uint nb_servers_tried = 0; // how many servers have we tried to connect ?

  // Cycle through the list of server
  // Until we find one running
  while (GetServerAddress(addr, port, nb_servers_tried))
  {
    r = Network::CheckHost(addr, port);
    if (r != CONNECTED)
      continue;

    // CheckHost opens and closes a connection to the server, so before reconnecting
    // wait a bit, so the connection really gets closed ..
    SDL_Delay(500);

    r = ConnectTo(addr, port);
    if (r == CONNECTED)
      return r;
  }

  // Undo what was done
  Disconnect();

  return r;
}

connection_state_t IndexServer::ConnectTo(const std::string & address, const int & port)
{
  connection_state_t status = CONN_REJECTED;
  int r;

  MSG_DEBUG("index_server", "Connecting to %s %i", address.c_str(), port);

  Network::Init(); // To get SDL_net initialized

  MSG_DEBUG("index_server", "Opening connection");

  if (SDLNet_ResolveHost(&ip, address.c_str() , port) == -1 ) {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    status = CONN_BAD_HOST;
    goto err;
  }

  socket = SDLNet_TCP_Open(&ip);
  if (!socket) {
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    status = CONN_REJECTED;
    goto err;
  }

  sock_set = SDLNet_AllocSocketSet(1);
  if (!sock_set) {
    printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
    status = CONN_REJECTED;
    goto err_alloc_socket_set;
  }

  r = SDLNet_TCP_AddSocket(sock_set, socket);
  if (r != 1) {
    printf("SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
    status = CONN_REJECTED;
    goto err_add_socket;
  }

  connected = true; // we need it before HandShake
  status = HandShake();
  if (status != CONNECTED)
    goto err_handshake;

  return status;

 err_handshake:
  connected = false;
  SDLNet_TCP_DelSocket(sock_set, socket);
 err_add_socket:
  SDLNet_FreeSocketSet(sock_set);
 err_alloc_socket_set:
  SDLNet_TCP_Close(socket);
 err:
  return status;
}

void IndexServer::Disconnect()
{
  if (hidden_server)
  {
    hidden_server = false;
    return;
  }

  first_server = server_lst.end();
  current_server = server_lst.end();

  if (!connected)
    return;

  MSG_DEBUG("index_server", "Closing connection");

  SDLNet_TCP_DelSocket(sock_set, socket);
  SDLNet_TCP_Close(socket);
  connected = false;
  SDLNet_FreeSocketSet(sock_set);
}

bool IndexServer::GetServerAddress( std::string & address, int & port, uint & nb_servers_tried)
{
  // have we already tried all servers ?
  if (server_lst.size() == nb_servers_tried) {
    return false;
  }
  nb_servers_tried++;

  MSG_DEBUG("index_server", "Trying a new server");
  // Cycle through the server list to find the first one
  // accepting connection
  if (first_server == server_lst.end())
    {
      // First try :
      // Randomly select a server in the list
      int nbr = RandomLocal().GetLong( 0, server_lst.size()-1 );
      first_server = server_lst.begin();
      while(nbr--)
	++first_server;

      ASSERT(first_server != server_lst.end());

      current_server = first_server;

      address = current_server->first;
      port = current_server->second;
      return true;
    }

  ++current_server;
  if (current_server == server_lst.end())
    current_server = server_lst.begin();

  address = current_server->first;
  port = current_server->second;

  return (current_server != first_server);
}

/*************  Basic transmissions  ******************/
void IndexServer::NewMsg(IndexServerMsg msg_id)
{
  assert(used == 0);
  Batch((int)msg_id);
  // Reserve 4 bytes for the total message length.
  used += 4;
}

void IndexServer::Batch(const int& nbr)
{
  assert(used+4 < INDEX_SERVER_BUFFER_LENGTH);
  used += Network::Batch(buffer+used, nbr);
}

void IndexServer::Batch(const std::string &str)
{
  assert(used+4+str.size() < INDEX_SERVER_BUFFER_LENGTH);
  used += Network::Batch(buffer+used, str);
}

bool IndexServer::SendMsg()
{
  bool r = Network::SendBatch(socket, buffer, used);
  used = 0;
  return r;
}

int IndexServer::ReceiveInt()
{
  //somehow we can get here while being disconnected... this should not be
  if (!connected) {
    MSG_DEBUG("index_server", "Not connected!!\n");
    return -1;
  }

  int r, nbr;
  r = Network::ReceiveInt(sock_set, socket, nbr);
  if (r == -2) {
    Disconnect();
    return 0;
  } else if (r != 0) {
    return r;
  }

  return nbr;
}

std::string IndexServer::ReceiveStr(size_t maxlen)
{
  if (!connected)
    return "";

  int r;
  std::string str("");

  r = Network::ReceiveStr(sock_set, socket, str, maxlen);
  if (r == -2) {
    Disconnect();
  }

  return str;
}

connection_state_t IndexServer::HandShake()
{
  connection_state_t status = CONN_REJECTED;
  bool r;
  int msg;
  std::string sign;

  MSG_DEBUG("index_server", "Beginning handshake...");

  NewMsg(TS_MSG_VERSION);
  Batch(Constants::WORMUX_VERSION);

  MSG_DEBUG("index_server", "Sending information...");

  r = SendMsg();
  if (!r)
    goto error;

  MSG_DEBUG("index_server", "Receiving...");

  msg = ReceiveInt();
  MSG_DEBUG("index_server", "Received: %d", msg);

  if (msg == -1 || msg != TS_MSG_VERSION)
    goto error;

  MSG_DEBUG("index_server", "Receiving...");
  sign = ReceiveStr(20);

  MSG_DEBUG("index_server", "Received: %s", sign.c_str());

  if (sign == "Bad version") {
    status = CONN_WRONG_VERSION;
    sign = ReceiveStr(20);
    AppWormux::DisplayError(Format(_("Sorry, your version is not supported anymore. "
				     "Supported version are %s. "
				     "You can download a updated version "
				     "on http://www.wormux.org/wiki/download.php"),
				   sign.c_str()));
    goto error;
  }

  if (sign != "MassMurder!")
    goto error;

  MSG_DEBUG("index_server", "Handshake : OK");

  status = CONNECTED;
  return status;

 error:
  MSG_DEBUG("index_server", "Handshake : ERROR!");
  return status;
}

bool IndexServer::SendServerStatus(const std::string& game_name, bool pwd)
{
  std::string ack;
  ASSERT(Network::GetInstance()->IsServer());

  if (hidden_server)
    return true;

  NewMsg(TS_MSG_REGISTER_GAME);
  Batch(game_name);
  Batch((int)pwd);
  SendMsg();
  NewMsg(TS_MSG_HOSTING);
  Batch(Network::GetInstance()->GetPort());
  SendMsg();

  ack = ReceiveStr(5);
  if (ack == "OK")
    return true;

  Disconnect();
  return false;
}

std::list<GameServerInfo> IndexServer::GetHostList()
{
  NewMsg(TS_MSG_GET_LIST);
  SendMsg();
  int lst_size = ReceiveInt();
  std::list<GameServerInfo> lst;
  if(lst_size == -1)
    return lst;
  while(lst_size--)
  {
    GameServerInfo game_server_info;
    IPaddress ip;
    ip.host = ReceiveInt();
    ip.port = ReceiveInt();
    game_server_info.passworded = !!ReceiveInt();
    game_server_info.game_name = ReceiveStr(40);

    const char* dns_addr = SDLNet_ResolveIP(&ip);
    char port[10];
    sprintf(port, "%d", ip.port);
    game_server_info.port = std::string(port);

    // We can't resolve the hostname, so just show the ip address
    unsigned char* str_ip = (unsigned char*)&ip.host;
    char formated_ip[16];
    snprintf(formated_ip, 16, "%i.%i.%i.%i", (int)str_ip[0],
	     (int)str_ip[1],
	     (int)str_ip[2],
	     (int)str_ip[3]);
    game_server_info.ip_address = std::string(formated_ip);

    if (dns_addr != NULL)
      game_server_info.dns_address = std::string(dns_addr);
    else
      game_server_info.dns_address = game_server_info.ip_address;

    MSG_DEBUG("index_server","ip: %s, port: %s, dns: %s, name: %s, pwd=%s\n",
	      game_server_info.ip_address.c_str(),
	      game_server_info.port.c_str(),
	      game_server_info.dns_address.c_str(),
	      game_server_info.game_name.c_str(),
              (game_server_info.passworded) ? "yes" : "no");

    lst.push_back(game_server_info);
  }
  return lst;
}

void IndexServer::Refresh()
{
  if(!connected)
    return;

  if(SDLNet_CheckSockets(sock_set, 100) == 0)
    return;

  if(!SDLNet_SocketReady(socket))
    return;

  int msg_id = ReceiveInt();
  if(msg_id == -1)
    return;

  if( msg_id == TS_MSG_PING )
  {
    NewMsg(TS_MSG_PONG);
    SendMsg();
  }
  else
    Disconnect();
}
