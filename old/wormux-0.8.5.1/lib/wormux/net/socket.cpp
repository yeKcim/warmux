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

#include <SDL_thread.h>
#include <SDL_timer.h>
#include <WORMUX_error.h>
#include <WORMUX_socket.h>

#include <iostream>
#include <sys/types.h>
#include <errno.h>
#include "extSDL_net.h"
//-----------------------------------------------------------------------------

// bigger packets are sent by a fake client
static const int MAX_VALID_PACKET_SIZE = 250*1024;

static void print_net_error(const std::string& text)
{
  int err = errno;
  fprintf(stderr, "Error in %s: SYS: %s - SDL: %s\n", text.c_str(), strerror(err), SDLNet_GetError());
}

//-----------------------------------------------------------------------------
// static method
WSocketSet* WSocketSet::GetSocketSet(uint maxsockets)
{
  SDLNet_SocketSet sdl_socket_set = SDLNet_AllocSocketSet(maxsockets);
  if (!sdl_socket_set) {
    print_net_error("SDLNet_AllocSocketSet");
    return NULL;
  }
  return new WSocketSet(maxsockets, sdl_socket_set);
}


WSocketSet::WSocketSet(uint maxsockets, SDLNet_SocketSet sdl_socket_set) :
  max_nb_sockets(maxsockets),
  socket_set(sdl_socket_set),
  lock(SDL_CreateMutex())
{
}

WSocketSet::~WSocketSet()
{
  ASSERT(sockets.empty());

  SDLNet_FreeSocketSet(socket_set);
  SDL_DestroyMutex(lock);
}

void WSocketSet::Lock()
{
  SDL_LockMutex(lock);
}

void WSocketSet::UnLock()
{
  SDL_UnlockMutex(lock);
}

bool WSocketSet::AddSocket(WSocket* socket)
{
  Lock();

  ASSERT(socket_set != NULL);

  if (!socket->AddToSocketSet(this))
    return false;

  sockets.push_back(socket);

  UnLock();

  return true;
}

void WSocketSet::RemoveSocket(WSocket* socket)
{
  Lock();

  ASSERT(socket_set != NULL);
  sockets.remove(socket);

  socket->RemoveFromSocketSet();

  UnLock();
}

std::list<WSocket*>& WSocketSet::GetSockets()
{
  return sockets;
}

int WSocketSet::CheckActivity(int timeout)
{
  return SDLNet_CheckSockets(socket_set, timeout);
}

uint WSocketSet::MaxNbSockets() const
{
  return max_nb_sockets;
}

uint WSocketSet::NbSockets() const
{
  return sockets.size();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

WSocket::WSocket(TCPsocket _socket, WSocketSet* _socket_set) :
  socket(_socket),
  socket_set(_socket_set),
  lock(SDL_CreateMutex()),
  using_tmp_socket_set(false),
  m_packet(NULL),
  m_packet_size(0),
  m_received(0)
{
  int r;
  r = socket_set->AddSocket(this);
  if (r == -1) {
    print_net_error("SDLNet_TCP_AddSocket");
    ASSERT(false);
  }
}

WSocket::WSocket(TCPsocket _socket):
  socket(_socket),
  socket_set(NULL),
  lock(SDL_CreateMutex()),
  using_tmp_socket_set(false),
  m_packet(NULL),
  m_packet_size(0),
  m_received(0)
{
}

WSocket::WSocket():
  socket(NULL),
  socket_set(NULL),
  lock(SDL_CreateMutex()),
  using_tmp_socket_set(false),
  m_packet(NULL),
  m_packet_size(0),
  m_received(0)
{
}

WSocket::~WSocket()
{
  Disconnect();

  SDL_DestroyMutex(lock);
}

connection_state_t WSocket::ConnectTo(const std::string &host, const int &port)
{
  WNet::Init();

  connection_state_t r = WNet::CheckHost(host, port);
  if (r != CONNECTED)
    return r;

  Lock();
  ASSERT(socket == NULL);

  IPaddress ip;
  TCPsocket tcp_socket;

  if (SDLNet_ResolveHost(&ip, host.c_str(), (Uint16)port) != 0) {
    print_net_error("SDLNet_ResolveHost");
    fprintf(stderr, "SDLNet_ResolveHost: %s to %s:%i\n", SDLNet_GetError(), host.c_str(), port);
    r = CONN_BAD_HOST;
    goto error;
  }

  // CheckHost opens and closes a connection to the server, so before reconnecting
  // wait a bit, so the connection really gets closed ..
  SDL_Delay(500);

  tcp_socket = SDLNet_TCP_Open(&ip);

  if (!tcp_socket) {
    print_net_error("SDLNet_TCP_Open");
    fprintf(stderr, "SDLNet_TCP_Open: %s to%s:%i\n", SDLNet_GetError(), host.c_str(), port);
    r = CONN_REJECTED;
    goto error;
  }

  socket = tcp_socket;
  r = CONNECTED;

 error:
  UnLock();
  return r;
}

bool WSocket::AcceptIncoming(const int& port)
{
  bool r = false;
  WNet::Init();

  Lock();
  ASSERT(socket == NULL);

  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, NULL, (Uint16)port) != 0) {
    print_net_error("SDLNet_ResolveHost");
    goto error;
  }

  socket = SDLNet_TCP_Open(&ip);
  if (!socket) {
    print_net_error("SDLNet_TCP_Open");
    goto error;
  }

  r = true;

 error:
  UnLock();
  return r;
}

WSocket* WSocket::LookForClient()
{
  TCPsocket client_sock = SDLNet_TCP_Accept(socket);
  if (!client_sock)
    return NULL;

  WSocket* client = new WSocket(client_sock);
  return client;
}

void WSocket::Disconnect()
{
  Lock();

  if (socket_set) {
    socket_set->RemoveSocket(this);
    if (using_tmp_socket_set) {
      delete socket_set;
    }
    socket_set = NULL;
  }

  if (socket) {
    SDLNet_TCP_Close(socket);
    socket = NULL;
  }

  if (m_packet)
    free(m_packet);
  m_packet_size = 0;
  m_received = 0;

  UnLock();
}

bool WSocket::IsConnected() const
{
  return (socket != NULL);
}

bool WSocket::AddToSocketSet(WSocketSet* _socket_set)
{
  ASSERT(socket_set == NULL);
  int r;

  Lock();
  socket_set = _socket_set;

  r = SDLNet_TCP_AddSocket(socket_set->socket_set, socket);
  if (r == -1) {
    print_net_error("SDLNet_TCP_AddSocket");
    UnLock();
    return false;
  }

  UnLock();
  return true;
}

void WSocket::RemoveFromSocketSet()
{
  ASSERT(!using_tmp_socket_set);

  int r;

  Lock();
  r = SDLNet_TCP_DelSocket(socket_set->socket_set, socket);
  if (r == -1) {
    print_net_error("SDLNet_TCP_DelSocket");
    ASSERT(false);
  }
  socket_set = NULL;
  UnLock();
}

bool WSocket::AddToTmpSocketSet()
{
  ASSERT(socket_set == NULL);
  int r;

  Lock();

  WSocketSet* tmp_socket_set = WSocketSet::GetSocketSet(1);
  socket_set = tmp_socket_set;

  socket_set->Lock();
  r = SDLNet_TCP_AddSocket(socket_set->socket_set, socket);
  if (r == -1) {
    print_net_error("SDLNet_TCP_AddSocket");
    delete tmp_socket_set;
    UnLock();
    return false;
  }
  socket_set->sockets.push_back(this);
  socket_set->UnLock();

  using_tmp_socket_set = true;
  UnLock();

  return true;
}

void WSocket::RemoveFromTmpSocketSet()
{
  ASSERT(using_tmp_socket_set);
  int r;

  Lock();
  socket_set->Lock();
  r = SDLNet_TCP_DelSocket(socket_set->socket_set, socket);
  if (r == -1) {
    print_net_error("SDLNet_TCP_DelSocket");
    ASSERT(false);
  }
  socket_set->sockets.remove(this);
  socket_set->UnLock();
  delete socket_set;
  socket_set = NULL;

  using_tmp_socket_set = false;

  UnLock();
}

void WSocket::Lock()
{
  SDL_LockMutex(lock);
}

void WSocket::UnLock()
{
  SDL_UnlockMutex(lock);
}

// Static methods usefull to communicate without action
// (index server, handshake, ...)

bool WSocket::SendInt_NoLock(const int& nbr)
{
  if (!IsConnected())
    return false;

  char tmppacket[4];
  // this is not cute, but we don't want an int -> uint conversion here
  Uint32 u_nbr = *((const Uint32*)&nbr);

  SDLNet_Write32(u_nbr, tmppacket);
  int len = SDLNet_TCP_Send_noBlocking(socket, tmppacket, sizeof(tmppacket));
  if (len < int(sizeof(tmppacket))) {
    print_net_error("SDLNet_TCP_Send");
    return false;
  }

  return true;
}

bool WSocket::SendInt(const int& nbr)
{
  bool r;

  Lock();
  r = SendInt_NoLock(nbr);
  UnLock();

  return r;
}

bool WSocket::SendStr_NoLock(const std::string &str)
{
  bool r = SendInt_NoLock(str.size());
  if (!r)
    return false;

  int len = SDLNet_TCP_Send_noBlocking(socket, (void*)str.c_str(), str.size());
  if (len < int(str.size())) {
    print_net_error("SDLNet_TCP_Send");
    return false;
  }

  return true;
}

bool WSocket::SendStr(const std::string &str)
{
  bool r;

  Lock();
  r = SendStr_NoLock(str);
  UnLock();

  return r;
}

bool WSocket::SendBuffer_NoLock(const void* data, size_t len)
{
  if (!IsConnected())
    return false;

  // cast is needed to please SDL that does not use const keyword.
  int size = SDLNet_TCP_Send_noBlocking(socket, (void*)(data), len);
  if (size < int(len)) {
    print_net_error("SDLNet_TCP_Send");
    return false;
  }

  return true;
}

bool WSocket::SendBuffer(const void* data, size_t len)
{
  bool r;

  Lock();
  r = SendBuffer_NoLock(data, len);
  UnLock();

  return r;
}

bool WSocket::NbBytesAvailable(size_t& _nb_bytes)
{
  int nb_bytes;

  nb_bytes = SDLNet_TCP_NbBytesAvailable(socket);
  if (nb_bytes < 0)
    return false;

  _nb_bytes = nb_bytes;
  return true;
}

bool WSocket::ReceiveBuffer_NoLock(void* data, size_t len)
{
  if (!IsConnected())
    return false;

  int received = 0;

  while (len) {
    // Documentation says that SDLNet_TCP_Recv receives data of *exactly* length "len" bytes
    // from the socket "socket" into the memory pointed to by "data".
    // BUT I have observed the contrary if len is big (len > 16384)
    // => A loop fixes this behavior
    //
    //                 Gentildemon.

    received = SDLNet_TCP_Recv(socket, data, len);
    if (received <= 0) {
      print_net_error("SDLNet_TCP_Recv");
      fprintf(stderr, "ERROR: SDLNet_TCP_Recv: %d\n", received);
      return false;
    }
    data = (char*)data + received;
    len -= received;
  }

  return true;
}

bool WSocket::ReceiveBuffer(void* data, size_t len)
{
  bool r;

  ASSERT(socket_set != NULL);

  if (!IsReady(5000)) {
    return false;
  }

  Lock();
  r = ReceiveBuffer_NoLock(data, len);
  UnLock();

  return r;
}

bool WSocket::ReceiveInt_NoLock(int& nbr)
{
  if (!IsConnected())
    return false;

  char tmppacket[4];
  Uint32 u_nbr;

  if (!ReceiveBuffer_NoLock(tmppacket, sizeof(tmppacket))) {
    return false;
  }

  u_nbr = SDLNet_Read32(tmppacket);
  nbr = *((int*)&u_nbr);

  return true;
}

bool WSocket::ReceiveInt(int& nbr)
{
  bool r;
  ASSERT(socket_set != NULL);

  if (!IsReady(5000)) {
    return false;
  }

  Lock();
  r = ReceiveInt_NoLock(nbr);
  UnLock();

  return r;
}

bool WSocket::ReceiveStr_NoLock(std::string &_str, size_t maxlen)
{
  bool r = true;
  unsigned int size = 0;
  char* str;

  r = ReceiveInt_NoLock((int&)size);
  if (!r) {
    goto out;
  }

  if (size == 0) {
    _str = "";
    goto out;
  }

  if (size > maxlen) {
    r = false;
    goto out;
  }

  str = new char[size+1];
  r = ReceiveBuffer_NoLock(str, size);
  if (!r) {
    goto out_delete;
  }

  str[size] = '\0';

  _str = str;

 out_delete:
  delete []str;
 out:
  return r;
}

bool WSocket::ReceiveStr(std::string &_str, size_t maxlen)
{
  bool r;
  ASSERT(socket_set != NULL);

  if (!IsReady(5000)) {
    return false;
  }

  Lock();
  r = ReceiveStr_NoLock(_str, maxlen);
  UnLock();

  return r;
}

uint32_t WSocket::ComputeCRC(const void* data, size_t len)
{
  uint32_t crc = 0;
  const uint32_t* buf = reinterpret_cast<const uint32_t*>(data);

  for (uint32_t i = 0; i < len/sizeof(uint32_t); i++) {
    crc += buf[i];
  }

  return crc;
}

bool WSocket::SendPacket(const char* data, size_t len)
{
  bool r;
  uint32_t crc;
  Lock();

  r = SendInt_NoLock(len);
  if (!r)
    goto out_unlock;

  r = SendBuffer_NoLock(data, len);
  if (!r)
    goto out_unlock;

  // Send a CRC check
  crc = ComputeCRC(data, len);
  r = SendInt_NoLock(crc);
  if (!r)
    goto out_unlock;

 out_unlock:
  UnLock();
  return r;
}

// ReceivePacket may return true with *data = NULL and len = 0
// That means that client is still valid BUT there are not enough data CURRENTLY
bool WSocket::ReceivePacket(char** data, size_t* len)
{
  bool r;

  Lock();

  int crc;
  size_t nbbytes, to_recv_now;
  bool tested = false;

  if (m_packet_size == 0) {

    // First check there is enough data to read the size of the packet
    r = NbBytesAvailable(nbbytes);
    if (!r) {
      goto error;
    }

    // There is nodata to read but the caller has (at least must have)
    // checked for activity. It means that the socket is disconnected.
    if (nbbytes == 0) {
      goto error;
    }

    tested = true;

    // there is not enough data to read the packet size but the
    // client is still valid
    if (nbbytes < sizeof(uint32_t)) {
      goto err_not_enough_data;
    }

    // Firstly, we read the size of the incoming packet
    r = ReceiveInt_NoLock(m_packet_size);
    if (!r) {
      goto error;
    }

    if (m_packet_size > MAX_VALID_PACKET_SIZE) {
      fprintf(stderr, "ERROR: network packet is too big\n");
      goto error;
    }

    m_packet = (char*)malloc(m_packet_size);
    if (!m_packet) {
      fprintf(stderr, "ERROR: memory allocation failed (%d bytes)\n", m_packet_size);
      goto error;
    }
  }

  // Check if the data (+crc) are already there
  r = NbBytesAvailable(nbbytes);
  if (!r) {
    goto error;
  }

  // There is nodata to read but the caller has (at least must have)
  // checked for activity. It means that the socket is disconnected.
  if (!tested && nbbytes == 0) {
    goto error;
  }

  // Compute how many data we have to receive now
  to_recv_now = m_packet_size - m_received;
  if (to_recv_now > nbbytes) {
    to_recv_now = nbbytes;
  }

  r = ReceiveBuffer_NoLock(m_packet + m_received, to_recv_now);
  if (!r) {
    fprintf(stderr, "ERROR: fail to receive data\n");
    goto error;
  }

  m_received += to_recv_now;
  nbbytes -= to_recv_now;

  // the packet (data + crc) can not have been read in one time
  // but client is still valid
  if (m_received != m_packet_size // packet is not yet fully received
      || nbbytes < sizeof(uint32_t) // not enough data to read CRC
      ) {
    goto err_not_enough_data;
  }

  // Check the CRC
  r = ReceiveInt_NoLock(crc);
  if (!r) {
    fprintf(stderr, "ERROR: fail to receive CRC\n");
    goto error;
  }

  if (uint32_t(crc) != ComputeCRC(m_packet, m_packet_size)) {
    fprintf(stderr, "ERROR: wrong CRC check\n");
    goto error;
  }

  *data = m_packet;
  *len = m_packet_size;

 out_finished:
  m_packet = NULL;
  m_packet_size = 0;
  m_received = 0;

  UnLock();
  return r;

 error:
  r = false;
  if (m_packet)
    free(m_packet);

  goto out_finished;

 err_not_enough_data:
  *data = NULL;
  *len = 0;
  r = true;
  UnLock();

  return true;
}

bool WSocket::IsReady(int timeout, bool force_check_activity) const
{
  if (socket == NULL)
    return false;

  if (timeout != 0 || force_check_activity) {
    ASSERT(socket_set != NULL);
    if (socket_set->CheckActivity(timeout) == 0)
      return false;
  }

  return SDLNet_SocketReady(socket);
}

bool WSocket::IsReady(int timeout) const
{
  return IsReady(timeout, false);
}

std::string WSocket::GetAddress() const
{
  ASSERT(socket != NULL);

  IPaddress* ip = SDLNet_TCP_GetPeerAddress(socket);
  std::string address;
  const char* resolved_ip = SDLNet_ResolveIP(ip);
  if (resolved_ip)
    address = resolved_ip;
  else
    return "Unresolved address";

  return address;
}
