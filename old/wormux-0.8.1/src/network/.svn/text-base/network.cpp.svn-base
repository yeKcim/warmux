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
 * Network layer for Wormux.
 *****************************************************************************/

#include <SDL_thread.h>
#include <SDL_timer.h>
#include "network/network.h"
#include "network/network_local.h"
#include "network/network_client.h"
#include "network/network_server.h"
#include "network/distant_cpu.h"
#include "network/chatlogger.h"
//-----------------------------------------------------------------------------
#include "game/game_mode.h"
#include "game/game.h"
#include "graphic/video.h"
#include "include/action.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "tool/debug.h"
#include "tool/i18n.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif
//-----------------------------------------------------------------------------

// Standard header, only needed for the following method
#ifdef WIN32
#  include <winsock.h>
#else
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <resolv.h>
#  include <errno.h>
#  include <unistd.h>
#endif

#include "team/team.h"
#include "team/teams_list.h"

//-----------------------------------------------------------------------------

int  Network::num_objects = 0;
bool Network::sdlnet_initialized = false;
bool Network::stop_thread = true;

Network * Network::GetInstance()
{
  if (singleton == NULL) {
    singleton = new NetworkLocal();
    MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkLocal'\n", singleton);
  }
  return singleton;
}

NetworkServer * Network::GetInstanceServer()
{
  if (singleton == NULL || !singleton->IsServer()) {
    return NULL;
  }
  return (NetworkServer*)singleton;
}

Network::Network(const std::string& passwd):
  password(passwd),
  turn_master_player(false),
  state(NO_NETWORK),// useless value at beginning
  thread(NULL),
  socket_set(NULL),
  ip(),
#ifdef LOG_NETWORK
  fout(0),
  fin(0),
#endif
  network_menu(NULL),
  cpu(),
  sync_lock(false)
{
  nickname = GetDefaultNickname();
  sdlnet_initialized = false;
  num_objects++;
}
//-----------------------------------------------------------------------------

Network::~Network()
{
#ifdef LOG_NETWORK
  if (fin != 0)
    close(fin);
  if (fout != 0)
    close(fout);
#endif

  num_objects--;
  if (num_objects==0)
  {
    if (sdlnet_initialized)
    {
      SDLNet_Quit();
      sdlnet_initialized = false;
    }
  }
}

//-----------------------------------------------------------------------------

std::string Network::GetDefaultNickname() const
{
  std::string s_nick;
  const char *nick = NULL;
#ifdef WIN32
  char  buffer[32];
  DWORD size = 32;
  if (GetUserName(buffer, &size))
    nick = buffer;
#else
  nick = getenv("USER");
#endif
  s_nick = (nick) ? nick : _("Unnamed");
  return s_nick;
}

void Network::SetNickname(const std::string& _nickname)
{
  nickname = _nickname;
}

const std::string& Network::GetNickname() const
{
  return nickname;
}

//-----------------------------------------------------------------------------

bool Network::ThreadToContinue() const
{
  return !stop_thread;
}

int Network::ThreadRun(void*/*no_param*/)
{
  MSG_DEBUG("network", "Thread created: %u", SDL_ThreadID());
  GetInstance()->ReceiveActions();
  return 1;
}

void Network::ReceiveActions()
{
  char* packet;
  std::list<DistantComputer*>::iterator dst_cpu;

  while (ThreadToContinue()) // While the connection is up
  {
    if (state == NETWORK_PLAYING && cpu.empty())
    {
      // If while playing everybody disconnected, just quit
      break;
    }

    //Loop while nothing is received
    while (ThreadToContinue())
    {
      WaitActionSleep();

      // Check forced disconnections
      for (dst_cpu = cpu.begin();
           ThreadToContinue() && dst_cpu != cpu.end();
           dst_cpu++)
      {
        if((*dst_cpu)->force_disconnect)
        {
          dst_cpu = CloseConnection(dst_cpu);
          if (cpu.empty())
            break; // Let it be handled afterwards
        }
      }

      // List is now maybe empty
      if (cpu.empty()) {
        if (IsClient()) {
          fprintf(stderr, "you are alone!\n");
	  stop_thread = true;
          return; // We really don't need to go through the loops
        }
        // Even for server, as Visual Studio in debug mode has trouble with that loop
	continue;
      }
      int num_ready = SDLNet_CheckSockets(socket_set, 100);
      // Means something is available
      if (num_ready>0)
        break;
      // Means an error
      else if (num_ready == -1)
      {
        fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
        continue; //Or break?
      }
    }

    for (dst_cpu = cpu.begin();
         ThreadToContinue() && dst_cpu != cpu.end();
         dst_cpu++)
    {
      if((*dst_cpu)->SocketReady()) // Check if this socket contains data to receive
      {
        // Read the size of the packet
        int packet_size = (*dst_cpu)->ReceiveDatas(packet);
        if( packet_size == -1) { // An error occured during the reception
          dst_cpu = CloseConnection(dst_cpu);
          // Please Visual Studio that in debug mode has trouble with continuing
          if (cpu.empty()) {
            if (IsClient()) {
              fprintf(stderr, "you are alone!\n");
	      stop_thread = true;
              return; // We really don't need to go through the loops
            }
            break;
          }
          continue;
        } else
        if (packet_size == 0) // We didn't receive the full packet yet
          continue;

#ifdef LOG_NETWORK
        if (fin != 0) {
          int tmp = 0xFFFFFFFF;
          write(fin, &packet_size, 4);
          write(fin, packet, packet_size);
          write(fin, &tmp, 4);
        }
#endif

        Action* a = new Action(packet, (*dst_cpu));
        if(!a->CheckCRC()) {
          MSG_DEBUG("network.crc_bad","!!! Bad CRC for action received !!!");
          delete a;
        } else {
          MSG_DEBUG("network.traffic", "Received action %s",
                    ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());
          HandleAction(a, *dst_cpu);
        }
        free(packet);

        if (cpu.empty()) {
          if (IsClient()) {
            fprintf(stderr, "you are alone!\n");
            stop_thread = true;
            return; // We really don't need to go through the loops
          }
          break;
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------

void Network::Init()
{
  if (sdlnet_initialized)
  {
      std::cout << "Network already initialized!" << std::endl;
      return;
  }
  if (SDLNet_Init()) {
      Error("Failed to initialize network library! (SDL_Net)");
      exit(1);
  }
  //printf("###  SDL_net start\n");
  sdlnet_initialized = true;

  std::cout << "o " << _("Network initialization") << std::endl;
}

//-----------------------------------------------------------------------------

// Static method
void Network::Disconnect()
{
  // restore Windows title
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::WORMUX_VERSION);

  if (singleton != NULL) {
    singleton->stop_thread = true;
    singleton->DisconnectNetwork();
    delete singleton;
    ChatLogger::CloseIfOpen();
  }
}

// Protected method for client and server
void Network::DisconnectNetwork()
{
  if (thread != NULL && SDL_ThreadID() != SDL_GetThreadID(thread)) {
    SDL_WaitThread(thread, NULL);
  }

  thread = NULL;
  stop_thread = true;

  for(std::list<DistantComputer*>::iterator client = cpu.begin();
      client != cpu.end();
      client++)
  {
    delete *client;
  }
  cpu.clear();

  if (socket_set != NULL) {
    SDLNet_FreeSocketSet(socket_set);
    socket_set = NULL;
  }
}

//-----------------------------------------------------------------------------
#ifdef WIN32
# define SOCKET_PARAM    char
#else
typedef int SOCKET;
# define SOCKET_PARAM    void
# define SOCKET_ERROR    (-1)
# define INVALID_SOCKET  (-1)
# define closesocket(fd) close(fd)
#endif

// static method
connection_state_t Network::GetError()
{
#ifdef WIN32
  int code = WSAGetLastError();
  switch (code)
  {
  case WSAECONNREFUSED: return CONN_REJECTED;
  case WSAEINPROGRESS:
  case WSAETIMEDOUT: return CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", code);
    return CONN_BAD_SOCKET;
  }
#else
  switch(errno)
  {
  case ECONNREFUSED: return CONN_REJECTED;
  case EINPROGRESS:
  case ETIMEDOUT: return CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", errno);
    return CONN_BAD_SOCKET;
  }
#endif
}

// static method
connection_state_t Network::CheckHost(const std::string &host, int prt)
{
  MSG_DEBUG("network", "Checking connection to %s:%i", host.c_str(), prt);

  struct hostent* hostinfo;
  hostinfo = gethostbyname(host.c_str());
  if( ! hostinfo )
    return CONN_BAD_HOST;

  SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd == INVALID_SOCKET )
    return CONN_BAD_SOCKET;

  // Set the timeout
#ifdef WIN32
  int timeout = 5000; //ms
#else
  struct timeval timeout;
  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_sec = 5; // 5seconds timeout
#endif
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
  {
    fprintf(stderr, "Setting receive timeout on socket failed\n");
    return CONN_BAD_SOCKET;
  }

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
  {
    fprintf(stderr, "Setting send timeout on socket failed\n");
    return CONN_BAD_SOCKET;
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

  if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR )
  {
    return GetError();
  }
  closesocket(fd);
  return CONNECTED;
}

//-----------------------------------------------------------------------------
//----------------       Action handling methods   ----------------------------
//-----------------------------------------------------------------------------

// Send Messages
void Network::SendAction(const Action& a) const
{
  MSG_DEBUG("network.traffic","Send action %s",
            ActionHandler::GetInstance()->GetActionName(a.GetType()).c_str());

  int size;
  char* packet;
  a.WriteToPacket(packet, size);

  ASSERT(packet != NULL);
  SendPacket(packet, size);

  free(packet);
}

void Network::SendPacket(char* packet, int size) const
{
#ifdef LOG_NETWORK
  if (fout != 0) {
    int tmp = 0xFFFFFFFF;
    write(fout, &size, 4);
    write(fout, packet, size);
    write(fout, &tmp, 4);
  }
#endif

  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++)
  {
    (*client)->SendDatas(packet, size);
  }
}

//-----------------------------------------------------------------------------

// Static method
bool Network::IsConnected()
{
  return (!GetInstance()->IsLocal() && !stop_thread);
}

uint Network::GetPort() const
{
  Uint16 prt;
  prt = SDLNet_Read16(&ip.port);
  return (uint)prt;
}

//-----------------------------------------------------------------------------

// Static method
connection_state_t Network::ClientStart(const std::string& host,
                                        const std::string& port,
					const std::string& password)
{
  NetworkClient* net = new NetworkClient(password);
  MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkClient'\n", net);

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  stop_thread = false;
  const connection_state_t error = net->ClientConnect(host, port);

  if (error != CONNECTED) {
    // revert change if connection failed
    stop_thread = true;
    singleton = prev;
    delete net;
  } else if (prev != NULL) {
    delete prev;
  }
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::WORMUX_VERSION + " - Client mode");
  return error;
}

//-----------------------------------------------------------------------------

// Static method
connection_state_t Network::ServerStart(const std::string& port, const std::string& password)
{
  NetworkServer* net = new NetworkServer(password);
  MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkServer'\n", net);

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  stop_thread = false;
  const connection_state_t error = net->ServerStart(port);

  if (error != CONNECTED) {
    // revert change
    stop_thread = true;
    singleton = prev;
    delete net;
  } else if (prev != NULL) {

    // that's ok
    AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::WORMUX_VERSION + " - Server mode");
    delete prev;
  }
  return error;
}

//-----------------------------------------------------------------------------

void Network::SetState(Network::network_state_t _state)
{
  state = _state;
}

Network::network_state_t Network::GetState() const
{
  return state;
}

void Network::SendNetworkState() const
{
  Action a(Action::ACTION_NETWORK_CHANGE_STATE);
  a.Push(state);
  SendAction(a);
}

void Network::SetTurnMaster(bool master)
{
  MSG_DEBUG("network.turn_master", "turn_master: %d", master);
  turn_master_player = master;
}

bool Network::IsTurnMaster() const
{
  return turn_master_player;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Static methods usefull to communicate without action
// (index server, handshake, ...)

bool Network::Send(TCPsocket& socket, const int& nbr)
{
  char packet[4];
  // this is not cute, but we don't want an int -> uint conversion here
  Uint32 u_nbr = *((const Uint32*)&nbr);

  SDLNet_Write32(u_nbr, packet);
  int len = SDLNet_TCP_Send(socket, packet, sizeof(packet));
  if (len < int(sizeof(packet)))
    return false;

  return true;
}

bool Network::Send(TCPsocket& socket, const std::string &str)
{
  bool r = Send(socket, str.size());
  if (!r)
    return false;

  int len = SDLNet_TCP_Send(socket, (void*)str.c_str(), str.size());
  if (len < int(str.size()))
    return false;

  return true;
}

uint Network::Batch(void* buffer, const int& nbr)
{
  // this is not cute, but we don't want an int -> uint conversion here
  Uint32 u_nbr = *((const Uint32*)&nbr);

  SDLNet_Write32(u_nbr, buffer);

  return 4;
}

uint Network::Batch(void* buffer, const std::string &str)
{
  uint size = str.size();
  Batch(buffer, size);
  memcpy(((char*)buffer)+4, str.c_str(), size);
  return 4+size;
}

// A batch consists in a msg id, a size, and the batch itself.
// Size wasn't known yet, so write it now.
bool Network::SendBatch(TCPsocket& socket, void* data, size_t len)
{
  SDLNet_Write32(len, (void*)( ((char*)data)+4 ) );

  int size = SDLNet_TCP_Send(socket, data, len);
  if (size < int(len)) {
    MSG_DEBUG("network", "size = %d", size);
    return false;
  }
  return true;
}

int Network::ReceiveInt(SDLNet_SocketSet& sock_set, TCPsocket& socket, int& nbr)
{
  char packet[4];
  int r = 0;
  Uint32 u_nbr;

  if (SDLNet_CheckSockets(sock_set, 5000) == 0) {
    r = 1;
    goto out;
  }

  if (!SDLNet_SocketReady(socket)) {
    r = -1;
    goto out;
  }

  if (SDLNet_TCP_Recv(socket, packet, sizeof(packet)) < 1)
  {
    r = -2;
    goto out;
  }

  u_nbr = SDLNet_Read32(packet);
  nbr = *((int*)&u_nbr);

 out:
  MSG_DEBUG("network", "r = %d", r);
  return r;
}

int Network::ReceiveStr(SDLNet_SocketSet& sock_set, TCPsocket& socket, std::string &_str, size_t maxlen)
{
  int r;
  uint size = 0;
  char* str;

  r = ReceiveInt(sock_set, socket, (int&)size);
  if (r) {
    goto out;
  }

  if (size == 0) {
    _str = "";
    goto out;
  }

  if (size > maxlen) {
    r = -1;
    goto out;
  }

  if (SDLNet_CheckSockets(sock_set, 5000) == 0) {
    r = -1;
    goto out;
  }

  if (!SDLNet_SocketReady(socket)) {
    r = -1;
    goto out;
  }

  str = new char[size+1];
  if( SDLNet_TCP_Recv(socket, str, size) < 1 )
  {
    r = -2;
    goto out_delete;
  }

  str[size] = '\0';

  _str = str;

 out_delete:
  delete []str;
 out:
  MSG_DEBUG("network", "r = %d", r);
  return r;
}
