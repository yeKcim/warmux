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
#include <list>
#include <time.h>
#include "net_data.h"
#include "debug.h"

#ifdef WIN32
# define SOCKET_PARAM    char
#else
typedef int SOCKET;
# define SOCKET_PARAM    void
# define SOCKET_ERROR    (-1)
# define INVALID_SOCKET  (-1)
# define closesocket(fd) close(fd)
#endif

NetData::NetData()
  : str(NULL)
  , str_size(0)
  , msg_size(0)
  , fd(INVALID_SOCKET)
  , ping_sent(false)
  , bytes_received(0)
  , msg_id(TS_NO_MSG)
  , connected(false)
{
  UpdatePing();
}

NetData::~NetData()
{
  // WARNING: potential bug here... to clean!!
  if (fd != INVALID_SOCKET)
    {
      if (connected) {
        DPRINT(INFO, "Certainly leaking FD %i: still connected", fd);
      } else {
        DPRINT(INFO, "Probably leaking FD %i: not connected", fd);
      }
    }
  else if (connected) {
    DPRINT(INFO, "Inconsistant state: connected but no FD");
  }

  DPRINT(CONN, "Disconnected.");
}

void NetData::Host(const int & client_fd, const unsigned int & ip)
{
  fd = client_fd;
  ip_address = *(int*)&ip;
  connected = true;
}

int NetData::GetConnection(const char* host, int prt)
{
  struct hostent* hostinfo;
  hostinfo = gethostbyname(host);
  if( ! hostinfo )
    {
      DPRINT(INFO, "Couldn't get a socket");
      return -1 /*CONN_BAD_HOST*/;
    }

  SOCKET lfd = socket(AF_INET, SOCK_STREAM, 0);
  if( lfd == INVALID_SOCKET )
    {
      DPRINT(INFO, "Couldn't get a socket");
      return -1 /*CONN_BAD_SOCKET*/;
    }

  // Set the timeout
#ifdef WIN32
  int timeout = 5000; //ms
#else
  struct timeval timeout;
  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_sec = 5; // 5seconds timeout
#endif
  if (setsockopt(lfd, SOL_SOCKET, SO_RCVTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
      DPRINT(INFO, "Setting receive timeout on socket failed\n");
      goto error /*CONN_BAD_SOCKET*/;
    }

  if (setsockopt(lfd, SOL_SOCKET, SO_SNDTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
      DPRINT(INFO, "Setting send timeout on socket failed\n");
      goto error /*CONN_BAD_SOCKET*/;
    }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(prt);
#ifndef WIN32
  addr.sin_addr.s_addr = *(in_addr_t*)*hostinfo->h_addr_list;
#else
  addr.sin_addr.s_addr = inet_addr(inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list));
#endif

  if( connect(lfd, (struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR )
    {
      DPRINT(INFO, "Connection to %s:%i : failed", host, prt);
      goto error /*GetError()*/;
    }

  return lfd;

error:
  closesocket(lfd);
  return -1;
}


bool NetData::ConnectTo(const std::string & address, const int & port)
{
  // Resolve the name of the host:
  struct hostent* hostinfo;
  hostinfo = gethostbyname(address.c_str());
  if(!hostinfo)
    {
      PRINT_ERROR;
      return false;
    }

  fd = GetConnection(address.c_str(), port);
  if(fd == -1)
    {
      PRINT_ERROR;
      return false;
    }

  connected = true;
  DPRINT(CONN, "Connected.");
  UpdatePing();
  return true;
}

bool NetData::ReceiveInt(int & nbr)
{
  uint32_t packet;

  if (recv(fd, &packet, sizeof(packet), MSG_DONTWAIT) != sizeof(packet)) {
    PRINT_ERROR;
    return false;
  }

  unsigned int u_nbr = ntohl(packet);
  nbr = *((int*)&u_nbr);
  DPRINT(TRAFFIC, "Received int: %i", nbr);
  bytes_received -= sizeof(uint32_t);
  UpdatePing();
  return true;
}

bool NetData::ReceiveStr(std::string & full_str)
{
  const unsigned int max_str_size = 16;

  if (str_size == 0)
    {
      // We don't know the string size -> read it
      if (bytes_received < sizeof(uint32_t)) {
        DPRINT(TRAFFIC, "Not enough data to store string length: %zu", bytes_received);
        return false;
      }

      int size = 0;
      if (!ReceiveInt(size))
        return false;

      str_size = (unsigned int)size;
      if (str_size <= 0 || str_size > max_str_size)
        return false;

      str = new char[str_size+1];
      memset(str, 0, str_size+1);
    }

  unsigned int old_size = strlen(str);
  unsigned int to_receive = str_size - old_size;
  int str_received;

  str_received = recv(fd, str + old_size, to_receive, MSG_DONTWAIT);

  if (str_received == -1 )
    {
      PRINT_ERROR;
      goto err_rcv;
    }

  // Check the client is not sending some 0
  // that could lock us
  if( strlen(str) != old_size + str_received )
    goto err_rcv;

  bytes_received -= str_received;
  DPRINT(TRAFFIC, "Received string: %s", str);

  if(strlen(str) == str_size)
    {
      full_str = std::string(str);
      delete []str;
      str_size = 0;
    }
  UpdatePing();
  return true;

 err_rcv:
  delete []str;
  return false;
}

bool NetData::SendInt(const int &nbr)
{
  uint32_t packet;

  unsigned int u_nbr = *((unsigned int*)&nbr);
  packet = htonl(u_nbr);

  if ( send(fd, &packet, sizeof(packet), MSG_NOSIGNAL) != sizeof(packet) ) {
    PRINT_ERROR;
    return false;
  }

  DPRINT(TRAFFIC, "Sent int: %i", nbr);
  UpdatePing();
  return true;
}

bool NetData::SendStr(const std::string &full_str)
{
  if (!SendInt((int)full_str.size()))
    return false;

  if ( send(fd, full_str.c_str(), full_str.size(), MSG_NOSIGNAL) != ssize_t(full_str.size()) ) {
    PRINT_ERROR;
    return false;
  }

  DPRINT(TRAFFIC, "Sent string: %s", full_str.c_str());
  UpdatePing();
  return true;
}

bool NetData::ReceiveData()
{
  int packet_size = 0;

  if( ioctl( GetFD(), FIONREAD, &packet_size) == -1 )
    {
      PRINT_ERROR;
      return false;
    }

  // received < 1 when the client disconnect
  if (packet_size < 1)
    {
      DPRINT(TRAFFIC, "Nothing received");
      return false;
    }
  bytes_received += packet_size;

  return true;
}

/*
 * returns false if client must be disconnected
 */
bool NetData::Receive()
{
  int r = true;

  if (!ReceiveData())
    return false;

  // Get the ID of the message
  // if we don't already know it
  if (msg_id == TS_NO_MSG)
    {

      // Waiting for the msg id + size
      if (bytes_received <= 2 * sizeof(uint32_t))
        {
          DPRINT(TRAFFIC, "Not yet ready to read (msg id + size)!");
          return true;
        }

      int id = 0;
      if (!ReceiveInt(id) )
        {
          DPRINT(TRAFFIC, "Didn't received msg id!");
          return false;
        }
      msg_id = (IndexServerMsg)id;

      int lsize = 0;
      if (!ReceiveInt(lsize))
        {
          DPRINT(TRAFFIC, "Didn't received msg size!");
          return false;
        }

      // Message is at least id+length
      if (lsize < 8 || lsize > 65535)
        {
          DPRINT(TRAFFIC, "Incorrect msg size of %i!", lsize);
          return false;
        }

      msg_size = lsize;
      DPRINT(TRAFFIC, "Message of id %i and size %zu\n", id, msg_size);
    }

  // Check that enough data has been received
  if (bytes_received + 2*sizeof(uint32_t) < msg_size) {
    DPRINT(TRAFFIC, "Not enough data: %zu / %zu", bytes_received + 2*sizeof(uint32_t), msg_size);
    return true;
  }

  switch (msg_id) {
  case TS_MSG_PING:
    r = SendInt(TS_MSG_PONG);
    break;

  case TS_MSG_PONG:
    ping_sent = false;
    break;

  default:
    r = HandleMsg(msg_id);
    break;
  }

  msg_id = TS_NO_MSG;
  return r;
}

void NetData::CheckState()
{
  if(difftime(time(NULL), ping_time) < 60.0)
    return;

  if(ping_sent)
    {
      DPRINT(CONN, "Connection to %i timed out", GetIP());
      connected = false;
      return;
    }

  if (SendInt(TS_MSG_PING))
    ping_sent = true;
  else
    ping_sent = false;
}

void NetData::UpdatePing()
{
  ping_time = time(NULL);
}

