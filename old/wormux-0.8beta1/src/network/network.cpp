/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "network.h"
#include "network_local.h"
#include "network_client.h"
#include "network_server.h"
//-----------------------------------------------------------------------------
#include "game/game_mode.h"
#include "game/game.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "tool/debug.h"
#include "tool/i18n.h"

#if defined(DEBUG) && not defined(WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

//-----------------------------------------------------------------------------

// Standard header, only needed for the following method
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#else
#include <winsock.h>
#endif

//-----------------------------------------------------------------------------

bool Network::sdlnet_initialized = false;
bool Network::stop_thread = true;

Network * Network::singleton = NULL;

Network * Network::GetInstance()
{
  if (singleton == NULL) {
    singleton = new NetworkLocal();
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

Network::Network():
  state(NO_NETWORK),// useless value at beginning
  thread(NULL),
  socket_set(NULL),
  ip(),
#if defined(DEBUG) && not defined(WIN32)
  fout(0),
  fin(0),
#endif
  network_menu(NULL),
  cpu(),
  sync_lock(false),
#ifdef WIN32
  nickname(getenv("USERNAME"))
#else
  nickname(getenv("USER"))
#endif
{
  sdlnet_initialized = false;
}
//-----------------------------------------------------------------------------

Network::~Network()
{
  if (sdlnet_initialized)
  {
    SDLNet_Quit();
    sdlnet_initialized = false;

#if defined(DEBUG) && not defined(WIN32)
    if (fin != 0)
      close(fin);
    if (fout != 0)
      close(fout);
#endif
  }
}

//-----------------------------------------------------------------------------

bool Network::ThreadToContinue()
{
  return !stop_thread;
}

int Network::ThreadRun(void* no_param)
{
  GetInstance()->ReceiveActions();
  Disconnect();
  std::cout << "Network : end of thread" << std::endl;
  return 1;
}

//-----------------------------------------------------------------------------

void Network::Init()
{
  if (sdlnet_initialized) return;
  if (SDLNet_Init()) {
      Error("Failed to initialize network library! (SDL_Net)");
      exit(1);
  }
  sdlnet_initialized = true;

  std::cout << "o " << _("Network initialization") << std::endl;
}

//-----------------------------------------------------------------------------

// Static method
void Network::Disconnect()
{
  // restore Windows title
  AppWormux::GetInstance()->video.SetWindowCaption( std::string("Wormux ") + Constants::VERSION);

  if (singleton != NULL) {
    singleton->stop_thread = true;
    singleton->DisconnectNetwork();
    delete singleton;
    singleton = NULL;
  }
}

// Protected method for client and server
void Network::DisconnectNetwork()
{
  if (thread != NULL && SDL_ThreadID() != SDL_GetThreadID(thread)) {
    SDL_WaitThread(thread, NULL);
    printf("Network thread finished\n");
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

const Network::connection_state_t Network::CheckHost(const std::string &host,
						     const std::string& port) const
{
  MSG_DEBUG("network", "Checking connection to %s:%s", host.c_str(), port.c_str());
  int prt=0;
  sscanf(port.c_str(),"%i",&prt);

  struct hostent* hostinfo;
  hostinfo = gethostbyname(host.c_str());
  if( ! hostinfo )
    return Network::CONN_BAD_HOST;

#ifndef WIN32
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd == -1 )
    return Network::CONN_BAD_SOCKET;

#else
  SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd == INVALID_SOCKET )
    return Network::CONN_BAD_SOCKET;
#endif

  // Set the timeout
  struct timeval timeout;
  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_sec = 5; // 5seconds timeout
#ifndef WIN32
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&timeout, sizeof(timeout));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void*)&timeout, sizeof(timeout));
#else
  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
  setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
#endif

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
#ifndef WIN32
  addr.sin_addr.s_addr = *(in_addr_t*)*hostinfo->h_addr_list;
#else
  addr.sin_addr.s_addr = inet_addr(*hostinfo->h_addr_list);
#endif

  addr.sin_port = htons(prt);

#ifndef WIN32
  if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1 )
  {
    switch(errno)
    {
    case ECONNREFUSED: return Network::CONN_REJECTED;
    case EINPROGRESS:
    case ETIMEDOUT: return Network::CONN_TIMEOUT;
    default: return Network::CONN_BAD_SOCKET;
    }
  }
  close(fd);
#else
  if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) != 0 )
  {
    switch(WSAGetLastError())
    {
    case WSAECONNREFUSED: return Network::CONN_REJECTED;
    case WSAEINPROGRESS:
    case WSAETIMEDOUT: return Network::CONN_TIMEOUT;
    default: return Network::CONN_BAD_SOCKET;
    }
  }
  closesocket(fd);
#endif
  return Network::CONNECTED;
}

//-----------------------------------------------------------------------------
//----------------       Action handling methods   ----------------------------
//-----------------------------------------------------------------------------

// Send Messages
void Network::SendAction(Action* a) const
{
  MSG_DEBUG("network.traffic","Send action %s",
	    ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());

  int size;
  char* packet;
  a->WritePacket(packet, size);

  assert(packet != NULL);
  SendPacket(packet, size);

  free(packet);
}

void Network::SendPacket(char* packet, int size) const
{
#if defined(DEBUG) && not defined(WIN32)
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

const uint Network::GetPort() const
{
  Uint16 prt;
  prt = SDLNet_Read16(&ip.port);
  return (uint)prt;
}

//-----------------------------------------------------------------------------

// Static method
Network::connection_state_t Network::ClientStart(const std::string &host,
						 const std::string& port)
{
  NetworkClient* net = new NetworkClient();

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  stop_thread = false;
  const Network::connection_state_t error = net->ClientConnect(host, port);

  if (error != Network::CONNECTED) {
    // revert change if connection failed
    stop_thread = true;
    singleton = prev;
    delete net;
  } else if (prev != NULL) {

    // that's ok
    AppWormux::GetInstance()->video.SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Client mode");
    delete prev;
  }
  return error;
}

//-----------------------------------------------------------------------------

// Static method
Network::connection_state_t Network::ServerStart(const std::string& port)
{
  NetworkServer* net = new NetworkServer();

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  stop_thread = false;
  const Network::connection_state_t error = net->ServerStart(port);

  if (error != Network::CONNECTED) {
    // revert change
    stop_thread = true;
    singleton = prev;
    delete net;
  } else if (prev != NULL) {

    // that's ok
    AppWormux::GetInstance()->video.SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Server mode");
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
  SendAction(&a);
}
