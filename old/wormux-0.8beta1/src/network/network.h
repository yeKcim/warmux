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

#ifndef NETWORK_H
#define NETWORK_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <SDL_thread.h>
#include <list>
#include <string>
#include "distant_cpu.h"
#include "include/action.h"
#include "include/base.h"
#include "menu/network_menu.h"
//-----------------------------------------------------------------------------
const std::string WORMUX_NETWORK_PORT = "3826";
const uint WORMUX_NETWORK_PORT_INT = 3826;

class NetworkServer;

class Network
{
public:
  typedef enum
    {
      NO_NETWORK,
      NETWORK_MENU_OK,
      NETWORK_LOADING_DATA,
      NETWORK_READY_TO_PLAY,
      NETWORK_PLAYING
    } network_state_t;

  typedef enum
    {
      CONNECTED,
      CONN_BAD_HOST,
      CONN_BAD_PORT,
      CONN_BAD_SOCKET,
      CONN_REJECTED,
      CONN_TIMEOUT
    } connection_state_t;

private:

  Network(const Network&);
  const Network& operator=(const Network&);
  friend class DistantComputer;

  static Network * singleton;
  static bool sdlnet_initialized;

  static bool stop_thread;

protected:
  network_state_t state;

  Network(); // pattern singleton

  SDL_Thread* thread; // network thread, where we receive data from network
  SDLNet_SocketSet socket_set;
  IPaddress ip; // for server : store listening port
                // for client : store server address/port

#if defined(DEBUG) && not defined(WIN32)
  int fout;
  int fin;
#endif

  bool ThreadToContinue();
  static int ThreadRun(void* no_param);

  void DisconnectNetwork();
  const connection_state_t CheckHost(const std::string &host,
				     const std::string &port) const;
public:
  NetworkMenu* network_menu;

  std::list<DistantComputer*> cpu; // list of the connected computer
  bool sync_lock;
  std::string nickname; //Clients: Send to Server at connect
                        //Server: Send in chat messages

  virtual ~Network();

  static Network* GetInstance();
  static NetworkServer* GetInstanceServer(); // WARNING: return NULL if not server!!

  static void Init();
  static void Disconnect();

  static bool IsConnected();
  virtual const bool IsLocal() const { return false ; }
  virtual const bool IsServer() const { return false ; }
  virtual const bool IsClient() const { return false ; }
  const uint GetPort() const;

  // Action handling
  void SendPacket(char* packet, int size) const;
  virtual void SendAction(Action* action) const;

  virtual void ReceiveActions() = 0;
  virtual void SendChatMessage(const std::string& txt) = 0;
  virtual std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed) = 0;

  // Start a client
  static connection_state_t ClientStart(const std::string &host, const std::string &port);

  // Start a server
  static connection_state_t ServerStart(const std::string &port);

  // Manage network state
  void SetState(Network::network_state_t state);
  Network::network_state_t GetState() const;
  void SendNetworkState() const;
};

//-----------------------------------------------------------------------------
#endif
