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

#ifndef CLIENT_H
#define CLIENT_H
#include <netinet/in.h>
#include <string>
#include "../../src/network/index_svr_msg.h"
#include "net_data.h"

class HostOptions {
 public:
  std::string game_name;
  bool        passwd;
  bool        used;

  HostOptions();
  bool Set(const std::string & game_name, bool pwd);
};

class Client : public NetData
{
  std::string version;
  HostOptions options;

  bool handshake_done;
  bool is_hosting;
  int port; // port number where a game is hosted

  bool HandShake(const std::string & version);
  bool RegisterWormuxServer();
  bool SetGameNamePasswd(const std::string & game_name,
			 const std::string & passwd);

  bool SendSignature();
  bool RejectBadVersion();
  bool SendList();
  void AddMeToClientsList(const std::string & version);

  // Tell other index server that a new wormux server just registered
  void NotifyServers(bool joining);
 public:
  Client(int client_fd,unsigned int & ip);
  ~Client();

  // Return false if the client closed the connection
  bool HandleMsg(enum IndexServerMsg msg_id);
};

class FakeClient
{
public:
  FakeClient(const int & _ip, const int & _port, const HostOptions & _options)
    {
      ip = _ip;
      port = _port;
      options = _options;
    }
  int ip;
  int port;
  HostOptions options;
};

#endif

