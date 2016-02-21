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

NetData::NetData()
  : str(NULL)
  , str_size(0)
  , msg_size(0)
  , ping_sent(false)
  , msg_id(TS_NO_MSG)
  , connected(false)
{
  UpdatePing();
}

NetData::~NetData()
{
  // WARNING: potential bug here... to clean!!
  // if (connected)
  //  close(fd);
  DPRINT(CONN, "Disconnected.");
}

void NetData::Host(const int & client_fd, const unsigned int & ip)
{
  fd = client_fd;
  ip_address = *(int*)&ip;
  connected = true;
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

  fd = socket(AF_INET, SOCK_STREAM, 0);

  if(fd == -1)
    {
      PRINT_ERROR;
      DPRINT(INFO, "Rejected");
      return false;
    }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = *(in_addr_t*)*hostinfo->h_addr_list;
  addr.sin_port = htons(port);

  if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) != 0 )
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
  if( read(fd, &packet, sizeof(packet)) == -1 )
    {
      PRINT_ERROR;
      return false;
    }

  unsigned int u_nbr = ntohl(packet);
  nbr = *((int*)&u_nbr);
  DPRINT(TRAFFIC, "Received int: %i", nbr);
  received -= 4;
  UpdatePing();
  return true;
}

bool NetData::ReceiveStr(std::string & full_str)
{
  const unsigned int max_str_size = 16;

  if (str_size == 0)
    {
      // We don't know the string size -> read it
      if (received < 4) {
        DPRINT(TRAFFIC, "Not enough data to store string length: %i", received);
        return false;
      }

      int size;
      if (!ReceiveInt(size))
        return false;

      str_size = (unsigned int)size;
      if (str_size <= 0 || str_size > max_str_size)
        return false;

      str = new char[str_size+1];
      memset(str, 0, str_size+1);
    }

  // Check if the string is already arrived
 //  if (received == 0)
//     {
//       printf("the string is already arrived\n");
//       full_str = "";
//       return true;
//     }

  unsigned int old_size = strlen(str);
  unsigned int to_receive = str_size - old_size;
  int str_received;

  str_received = read(fd, str + old_size, to_receive);

  if( str_received == -1 )
    {
      PRINT_ERROR;
      return false;
    }

  // Check the client is not sending some 0
  // that could lock us
  if( strlen(str) != old_size + str_received )
    return false;

  received -= str_received;
  DPRINT(TRAFFIC, "Received string: %s", str);

  if(strlen(str) == str_size)
    {
      full_str = std::string(str);
      delete []str;
      str_size = 0;
    }
  UpdatePing();
  return true;
}

bool NetData::SendInt(const int &nbr)
{
  uint32_t packet;

  unsigned int u_nbr = *((unsigned int*)&nbr);
  packet = htonl(u_nbr);

  if( write(fd, &packet, sizeof(packet)) == -1 )
    {
      PRINT_ERROR;
      return false;
    }

  DPRINT(TRAFFIC, "Sent int: %i", nbr);
  UpdatePing();
  return true;
}

bool NetData::SendStr(const std::string &full_str)
{
  if( ! SendInt((int)full_str.size()) )
    return false;

  if( write(fd, full_str.c_str(), full_str.size()) == -1 )
    {
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
  received += packet_size;

  return true;
}

bool NetData::Receive()
{
  int r = true;

  if (!ReceiveData())
    return false;

  // Get the ID of the message
  // if we don't already know it
  if(msg_id == TS_NO_MSG)
    {
      int id;
      if( !ReceiveInt(id) )
        {
          DPRINT(TRAFFIC, "Didn't received msg id!");
          return false;
        }
      msg_id = (IndexServerMsg)id;

      int lsize;
      if( !ReceiveInt(lsize) )
        {
          DPRINT(TRAFFIC, "Didn't received msg size!");
          return false;
        }

      // Message is at least id+length
      if (lsize<8 || lsize > 65535)
        {
          DPRINT(TRAFFIC, "Incorrect msg size of %i!", lsize);
          return false;
        }

      msg_size = lsize;
      DPRINT(TRAFFIC, "Message of id %i and size %u\n", id, msg_size);
    }

  // Check that enough data has been received
  if (received < msg_size-8)
    return false;

  switch(msg_id) {
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

  SendInt(TS_MSG_PING);
  ping_sent = true;
}

void NetData::UpdatePing()
{
  ping_time = time(NULL);
}

