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

#include <SDL_thread.h>
#include <SDL_timer.h>
#include "network.h"
#include "network_local.h"
#include "network_client.h"
#include "network_server.h"
#include "distant_cpu.h"
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
#endif

//-----------------------------------------------------------------------------

int  Network::num_objects = 0;
bool Network::sdlnet_initialized = false;
bool Network::stop_thread = true;

Network * Network::singleton = NULL;

Network * Network::GetInstance()
{
  if (singleton == NULL) {
    singleton = new   NetworkLocal();
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
  sync_lock(false),
#ifdef WIN32
  nickname(getenv("USERNAME"))
#else
  nickname(getenv("USER"))
#endif
{
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
      //printf("###  SDL_net end\n");
      SDLNet_Quit();
      sdlnet_initialized = false;
    }
  }
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
#ifndef WIN32
  Disconnect(); // this is really needed but it causes a deadlock on WIN32 for unknown reason
#endif
  std::cout << "Network : end of thread" << std::endl;
  return 1;
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
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::VERSION);

  if (singleton != NULL) {
    singleton->stop_thread = true;
    singleton->DisconnectNetwork();
    delete singleton;
    singleton = NULL;
  }
}

static inline void sdl_thread_wait_for(SDL_Thread* thread, uint /*timeout*/)
{
  SDL_WaitThread(thread, NULL);
}

// Protected method for client and server
void Network::DisconnectNetwork()
{
  if (thread != NULL && SDL_ThreadID() != SDL_GetThreadID(thread)) {
    sdl_thread_wait_for(thread, 4000);
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

const Network::connection_state_t Network::GetError() const
{
#ifdef WIN32
  int code = WSAGetLastError();
  switch (code)
  {
  case WSAECONNREFUSED: return Network::CONN_REJECTED;
  case WSAEINPROGRESS:
  case WSAETIMEDOUT: return Network::CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", code);
    return Network::CONN_BAD_SOCKET;
  }
#else
  switch(errno)
  {
  case ECONNREFUSED: return Network::CONN_REJECTED;
  case EINPROGRESS:
  case ETIMEDOUT: return Network::CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", errno);
    return Network::CONN_BAD_SOCKET;
  }
#endif
}

const Network::connection_state_t Network::CheckHost(const std::string &host, int prt) const
{
  MSG_DEBUG("network", "Checking connection to %s:%i", host.c_str(), prt);

  struct hostent* hostinfo;
  hostinfo = gethostbyname(host.c_str());
  if( ! hostinfo )
    return Network::CONN_BAD_HOST;

  SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd == INVALID_SOCKET )
    return Network::CONN_BAD_SOCKET;

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
    return Network::CONN_BAD_SOCKET;
  }

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
  {
    fprintf(stderr, "Setting send timeout on socket failed\n");
    return Network::CONN_BAD_SOCKET;
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
    return Network::GetError();
  }
  closesocket(fd);
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
    delete prev;
  }
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Client mode");
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
    AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Server mode");
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

void Network::SetTurnMaster(bool master)
{
  MSG_DEBUG("network.turn_master", "turn_master: %d", master);
  turn_master_player = master;
}

bool Network::IsTurnMaster() const
{
  return turn_master_player;
}
