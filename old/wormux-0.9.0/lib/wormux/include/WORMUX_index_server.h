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
 ******************************************************************************
 * Notify an index server of an opened wormux server
 * Obtain information about running games from an index server
 *****************************************************************************/

#ifndef INDEX_SERVER_H
#define INDEX_SERVER_H
//-----------------------------------------------------------------------------
#include <WORMUX_index_svr_msg.h>
#include <WORMUX_singleton.h>
#include <WORMUX_socket.h>
#include <map>
#include <list>
#include <string>
#include <utility>

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
  WSocket socket;
  char    buffer[INDEX_SERVER_BUFFER_LENGTH];

  // Stores the hostname / port of all online servers
  std::map<std::string, int> server_lst;
  // First server we tried to connect to
  std::map<std::string, int>::iterator first_server;
  // The current server we are trying to connect to
  std::map<std::string, int>::iterator current_server;

  // If we are a server, tell if we are visible on internet
  bool hidden_server;

  std::string supported_versions;

  // Time of the last "Pong" message sent
  time_t time_pong;

  // Used to avoid race condition between ping (handled by the network thread)
  // and other receive (handled by the main thread)
  SDL_sem* action_sem;

  // Transfer functions
  static void NewMsg(IndexServerMsg msg_id, char* buffer, uint& used);
  static bool SendMsg(WSocket& socket, char* buffer, uint& used);

  bool SendPong();

  // Gives the address of a server in the list
  bool GetServerAddress(std::string& address, int& port, uint& nb_tries);
  // Connect to a server
  connection_state_t ConnectTo(const std::string& address, const int& port,
			       const std::string& wormux_version);

  // Perform a handshake with the server
  connection_state_t HandShake(const std::string& wormux_version);

  void __Disconnect();

  void Lock();
  void Unlock();
  bool TryLock();

public:
  IndexServer();
  ~IndexServer();

  // Connect/disconnect to a server
  connection_state_t Connect(const std::string& wormux_version);
  void Disconnect();

  bool IsConnected();

  // Answers to pings from the server / close connection if distantly closed
  void Refresh(bool nowait = false);

  // Set the address
  void SetAddress(const char* hostname, int port = 9997) { server_lst[hostname] = port; }

  // We want to host a game hidden on internet
  void SetHiddenServer() { hidden_server = true; };

  // Notify the top server we are hosting a game
  bool SendServerStatus(const std::string& game_name, bool passwd, int port);

  // returns a list of game servers
  std::list<GameServerInfo> GetHostList(bool symbolic_name
					/*make a reverse DNS resolution*/);

  const std::string& GetSupportedVersions() const;
};

#endif
