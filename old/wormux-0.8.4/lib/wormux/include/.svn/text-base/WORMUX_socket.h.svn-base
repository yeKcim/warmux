/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Socket abstraction
 *****************************************************************************/

#ifndef WORMUX_SOCKET_H
#define WORMUX_SOCKET_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <list>
#include <string>
#include <WORMUX_network.h>
#include <WORMUX_types.h>
//-----------------------------------------------------------------------------

class WSocket;

class WSocketSet
{
  friend class WSocket;

private:
  uint max_nb_sockets;
  SDLNet_SocketSet socket_set;
  std::list<WSocket*> sockets;
  SDL_mutex* lock;

  void Lock();
  void UnLock();

  WSocketSet(uint max_sockets, SDLNet_SocketSet socket_set);

public:
  // may return NULL in case of problem
  static WSocketSet* GetSocketSet(uint max_sockets);

  ~WSocketSet();
  bool AddSocket(WSocket* socket);
  void RemoveSocket(WSocket* socket);

  int CheckActivity(int timeout);
  uint MaxNbSockets() const;
  uint NbSockets() const;

  std::list<WSocket*>& GetSockets();
};

class WSocket
{
  friend class WSocketSet;

private:
  TCPsocket socket;
  WSocketSet* socket_set;
  SDL_mutex* lock;

  bool using_tmp_socket_set;

  bool AddToSocketSet(WSocketSet* _socket_set);
  void RemoveFromSocketSet();
  uint32_t ComputeCRC(const void* data, size_t len);

public:
  WSocket(TCPsocket _socket, WSocketSet* _socket_set);
  WSocket(TCPsocket _socket);
  WSocket();
  ~WSocket();

  void Lock();
  void UnLock();

  // For clients
  connection_state_t ConnectTo(const std::string &host, const int &port);

  // For servers
  bool AcceptIncoming(const int &port);
  WSocket* LookForClient();

  void Disconnect();
  bool IsConnected() const;

  bool AddToTmpSocketSet();
  void RemoveFromTmpSocketSet();

  std::string GetAddress() const;
  bool IsReady(int timeout = 0) const;

  bool SendInt_NoLock(const int& nbr);
  bool SendInt(const int& nbr);

  bool SendStr_NoLock(const std::string &str);
  bool SendStr(const std::string &str);

  bool SendBuffer_NoLock(const void* data, size_t len);
  bool SendBuffer(const void* data, size_t len);

  bool ReceiveBuffer_NoLock(void* data, size_t len);
  bool ReceiveBuffer(void* data, size_t len);

  bool ReceiveInt_NoLock(int& nbr);
  bool ReceiveInt(int& nbr);

  bool ReceiveStr_NoLock(std::string &_str, size_t maxlen);
  bool ReceiveStr(std::string &_str, size_t maxlen);

  // Packet is composed of [ size (4 bytes),  data ]
  bool SendPacket(const void* data, size_t len);
  bool ReceivePacket(void* &data, size_t& len);
};

//-----------------------------------------------------------------------------
#endif
