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

#ifndef INDEX_SERVER_H
#define INDEX_SERVER_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <map>
#include <list>
#include <string>
#include <utility>
#include "network/network.h"
#include "network/index_svr_msg.h"
#include "include/singleton.h"

class GameServerInfo
{
 public:
  std::string ip_address;
  std::string port;
  std::string dns_address;
  std::string game_name;
  bool        passworded;
};

#define INDEX_SERVER_BUFFER_LENGTH   1500

class IndexServer : public Singleton<IndexServer>
{
  /* If you need this, implement it (correctly)*/
  IndexServer(const IndexServer&);
  const IndexServer& operator=(const IndexServer&);
  /*********************************************/

  // Connection to the server
  TCPsocket socket;
  IPaddress ip;
  SDLNet_SocketSet sock_set;
  char    buffer[INDEX_SERVER_BUFFER_LENGTH];
  uint    used;

  // Stores the hostname / port of all online servers
  std::map<std::string, int> server_lst;
  // First server we tried to connect to
  std::map<std::string, int>::iterator first_server;
  // The current server we are trying to connect to
  std::map<std::string, int>::iterator current_server;

  // If we are a server, tell if we are visible on internet
  bool hidden_server;

  bool connected;

  // Transfer functions
  void NewMsg(IndexServerMsg msg_id);
  void Batch(const int &nbr);
  void Batch(const std::string &str);
  void SendMsg();
  int ReceiveInt();
  std::string ReceiveStr();

  // Gives the address of a server in the list
  bool GetServerAddress(std::string & address, int & port, uint& nb_tries);
  // Connect to a server
  bool ConnectTo(const std::string & address, const int & port);

  // Perform a handshake with the server
  bool HandShake();
public:
  IndexServer();
  ~IndexServer();

  // Connect/disconnect to a server
  connection_state_t Connect();
  void Disconnect();

  // Answers to pings from the server / close connection if distantly closed
  void Refresh();

  // Set it local
  void SetLocal(const char* hostname = "127.0.0.1") { server_lst[hostname] = 9997; }

  // We want to host a game hidden on internet
  void SetHiddenServer() { hidden_server = true; };

  // Notify the top server we are hosting a game
  bool SendServerStatus(const std::string& game_name, bool passwd);

  // returns a list with string pairs: first element = hostname/ip, second element = port
  std::list<GameServerInfo> GetHostList();
};

#endif
