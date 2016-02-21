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

#ifndef SYNC_SLAVE_H
#define SYNC_SLAVE_H
#include <map>
#include <string>
#include "net_data.h"

const std::string sync_serv_version = "WIS";

// Connection to an other index server
// We don't send anything to this connection (except the handshake+version), we just receive information
// about new games on this server (this server will open a socket by itself to receive information about
// connections on us).
class IndexServerConn : public NetData
{
 private:
  bool HandShake();

 public:
  IndexServerConn(const std::string &addr, int port);
  ~IndexServerConn();
  bool HandleMsg(enum IndexServerMsg msg_id);
};

// List that contains informations about wormux client/server connected
// on other index servers
class SyncSlave : public std::map<std::string, IndexServerConn*>
{
 public:
  SyncSlave();
  ~SyncSlave();
  bool Start();
  void CheckGames();
};

extern SyncSlave sync_slave;

#endif
