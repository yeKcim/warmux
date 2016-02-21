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

#ifndef NET_DATA_H
#define NET_DATA_H
#include <netinet/in.h>
#include <string>
#include <time.h>
#include "../../src/network/index_svr_msg.h"

class NetData
{
  char*  str;
  size_t str_size;
  size_t msg_size;

  int    fd;
  int    ip_address;
  time_t ping_time;
  bool   ping_sent;

  size_t bytes_received;

  void UpdatePing();
 protected:
  enum IndexServerMsg msg_id;

  // Return false if the client closed the connection
  bool ReceiveStr(std::string & full_str);
  bool ReceiveInt(int & nbr);

  bool SendInt(const int & nbr);
  bool SendStr(const std::string & full_str);

  size_t BytesReceived() const { return bytes_received; };
 public:
  bool connected;

  NetData();
  virtual ~NetData();

  const int & GetFD() { return fd; };
  const int & GetIP() { return ip_address; };

  void Host(const int & client_fd, const unsigned int & ip);
  static int GetConnection(const char* host, int port);
  bool ConnectTo(const std::string & address, const int & port);
  bool Receive();
  bool ReceiveData();
  virtual bool HandleMsg(enum IndexServerMsg msg_id) = 0;
  void CheckState();
};

#endif

