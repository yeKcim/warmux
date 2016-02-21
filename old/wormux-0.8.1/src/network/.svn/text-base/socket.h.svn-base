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

#ifndef SOCKET_H
#define SOCKET_H
#include <list>
#include <SDL_net.h>
#include "network/network.h"

// TODO:
// Use this debug to store network communication to a file
#ifdef DEBUG
//#define LOG_NETWORK
#endif

// Factorize some declarations for Visual C++
#ifdef _MSC_VER
#  define S_IRUSR _S_IREAD
#  define S_IWUSR _S_IWRITE
#endif
#ifdef WIN32
#  define S_IRGRP 0
#  define O_SYNC  O_BINARY
#endif

class Socket
{
  SDLNet_SocketSet sock_set;
  std::list<TCPsocket> sock_lst;
  IPaddress ip; // for server : store listening port
                // for client : store server address/port

  bool connected;
  char* buf;
  unsigned int buf_size;

#ifdef LOG_NETWORK
  int fout;
  int fin;
#endif

  
  void ShowError();

  connection_state_t Receive();

public:
  Socket();
  ~Socket();

  connection_state_t Connect(std::string host, int port);
  void Disconnect();

  connection_state_t SendString(const char* data); // Send a string terminated by a NULL character
  connection_state_t SendBinary(const char* data, int size); // Send binary datas

  connection_state_t GetString(char** str, char eos); // Allocates and receive a string terminated by eos (if available)
  connection_state_t GetBinary(char** data, unsigned int size); // Allocates and receive a buffer (if available)
};

#endif
