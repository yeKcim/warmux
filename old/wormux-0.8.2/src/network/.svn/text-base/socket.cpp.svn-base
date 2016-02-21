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
 * SDL Network layer
 *****************************************************************************/

#include <SDL_net.h>
#include "include/base.h"
#include "network/network.h"
#include "network/socket.h"
#include "tool/debug.h"

Socket::Socket()
{
  connected = false;
  buf = NULL;
  buf_size = 0;
}

Socket::~Socket()
{
  Disconnect();
}

void Socket::ShowError()
{
  fprintf(stderr, "SDLNet error: %s\n", SDLNet_GetError());
}

connection_state_t Socket::Connect(std::string host, int port)
{
  ASSERT(!connected);

  //Network::GetInstance()->Init();

  MSG_DEBUG("socket", "Client connect to %s:%s", host.c_str(), port);

  connection_state_t r = Network::GetInstance()->CheckHost(host, port);
  if (r != CONNECTED)
    return r;

  if (SDLNet_ResolveHost(&ip,host.c_str(),(Uint16)port) == -1)
  {
    fprintf(stderr, "SDLNet_ResolveHost (%s): %s:%i\n", SDLNet_GetError(), host.c_str(), port);
    ShowError();
    return CONN_BAD_HOST;
  }

  // CheckHost opens and closes a connection to the server, so before reconnecting
  // wait a bit, so the connection really gets closed ..
  SDL_Delay(500);

  TCPsocket socket = SDLNet_TCP_Open(&ip);

  if (!socket)
  {
    fprintf(stderr, "SDLNet_TCP_Open: %s:%i\n", host.c_str(), port);
    ShowError();
    return CONN_REJECTED;
  }

  sock_set = SDLNet_AllocSocketSet(1);
  SDLNet_TCP_AddSocket(sock_set, socket);
  sock_lst.push_back(socket);

  connected = true;
  return CONNECTED;
}

void Socket::Disconnect()
{
  if(!connected)
    return;

  for(std::list<TCPsocket>::iterator sock = sock_lst.begin();
      sock != sock_lst.end();
      ++sock)
  {
    SDLNet_TCP_Close(*sock);
    SDLNet_TCP_DelSocket(sock_set, *sock);
  }
  SDLNet_FreeSocketSet(sock_set);
}

connection_state_t Socket::SendBinary(const char* data, int size)
{
  ASSERT(connected);

  int sent = SDLNet_TCP_Send(sock_lst.back(), (void*) data, size);

  if(sent != size)
    return CONN_REJECTED;

  return CONNECTED;
}

connection_state_t Socket::SendString(const char* data)
{
  return SendBinary(data, strlen(data));
}

connection_state_t Socket::Receive()
{
  ASSERT(connected);

  int r = SDLNet_CheckSockets(sock_set, 0);

  if(r < 0)
  {
    ShowError();
    return CONN_REJECTED;
  }

  if(r == 0)
    return CONNECTED; // But no data received

  for(std::list<TCPsocket>::iterator sock = sock_lst.begin();
      sock != sock_lst.end();
      ++sock)
  {
    if(SDLNet_SocketReady(*sock)) // Find the socket that received datas
    {
      const int to_receive = 1028;

      do
      {
        buf = (char*) realloc(buf, buf_size + to_receive);

        if(! buf)
        {
          free(buf);
          buf = NULL;
          buf_size = 0;
          return CONN_REJECTED;
        }

        int r = SDLNet_TCP_Recv(*sock, buf + buf_size, to_receive);
        
        if(r <= 0)
        {
          free(buf);
          buf = NULL;
          buf_size = 0;
          return CONN_REJECTED;
        }
        buf_size += r;
      } while(SDLNet_SocketReady(*sock));

      return CONNECTED;
    }
  }
  return CONN_REJECTED;
}

connection_state_t Socket::GetString(char** str, char eos)
{
  ASSERT(connected);

  *str = NULL;

  // Check for incoming data
  connection_state_t ret = Receive();
  if(ret != CONNECTED)
    return ret;

  // Find the end of the string
  unsigned int i=0;
  while(i < buf_size && buf[i] != eos)
    i++;

  if(i == buf_size)
    return CONNECTED;

  // Retrieve the string from the buffer
  ret = GetBinary(str, i + 1);
  if(ret != CONNECTED)
    return ret;

  // Add a NULL character at the end of the string
  *str = (char*) realloc(*str, i + 2);
  (*str)[i + 1] = '\0';

  return CONNECTED;
}
connection_state_t Socket::GetBinary(char** data, unsigned int size)
{
  ASSERT(connected);

  *data = 0;

  // Check for incoming data
  connection_state_t ret = Receive();
  if(ret != CONNECTED)
    return ret;

  if(buf_size < size)
    return CONNECTED;

  // Allocate the string to return
  *data = (char*) malloc(size);

  memcpy(*data, buf, size);

  if(size == buf_size)
  {
    free(buf);
    buf_size = 0;
    return CONNECTED;
  }

  // Suppress the string from our buffer
  char* new_buf = (char* )malloc(buf_size - size);
  memcpy(new_buf, buf + size, buf_size - size);

  free(buf);
  buf = new_buf;
  buf_size -= size;
  return CONNECTED;
}

