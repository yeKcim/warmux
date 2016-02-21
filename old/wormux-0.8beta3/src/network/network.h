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
#include <list>
#include <string>
#include "include/base.h"
//-----------------------------------------------------------------------------

// Use this debug to store network communication to a file
//#define LOG_NETWORK
// Factorize some declarations for Visual C++
#ifdef _MSC_VER
#  define S_IRUSR _S_IREAD
#  define S_IWUSR _S_IWRITE
#endif
#ifdef WIN32
#  define S_IRGRP 0
#  define O_SYNC  O_BINARY
#endif

const std::string WORMUX_NETWORK_PORT = "3826";
const uint WORMUX_NETWORK_PORT_INT = 3826;

// Some forward declarations
struct SDL_Thread;
class Action;
class DistantComputer;
class NetworkServer;
class NetworkMenu;

typedef enum
{
  CONNECTED,
  CONN_BAD_HOST,
  CONN_BAD_PORT,
  CONN_BAD_SOCKET,
  CONN_REJECTED,
  CONN_TIMEOUT
} connection_state_t;

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

private:

  Network(const Network&);
  const Network& operator=(const Network&);
  friend class DistantComputer;
  const connection_state_t GetError() const;

  static Network * singleton;
  static bool sdlnet_initialized;
  static int  num_objects;

  static bool stop_thread;
  bool turn_master_player;

  void ReceiveActions();

protected:
  network_state_t state;

  Network(); // pattern singleton

  SDL_Thread* thread; // network thread, where we receive data from network
  SDLNet_SocketSet socket_set;
  IPaddress ip; // for server : store listening port
                // for client : store server address/port

#ifdef LOG_NETWORK
  int fout;
  int fin;
#endif

  bool ThreadToContinue() const;
  static int ThreadRun(void* no_param);

  virtual void HandleAction(Action* a, DistantComputer* sender) = 0;
  virtual void WaitActionSleep() = 0;

  void DisconnectNetwork();
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

  virtual void SendChatMessage(const std::string& txt) = 0;
  virtual std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed) = 0;

  // Start a client
  static connection_state_t ClientStart(const std::string &host, const std::string &port);

  // Start a server
  static connection_state_t ServerStart(const std::string &port);

  // Manage network state
  const connection_state_t CheckHost(const std::string &host, int prt) const;
  void SetState(Network::network_state_t state);
  Network::network_state_t GetState() const;
  void SendNetworkState() const;

  void SetTurnMaster(bool master);
  bool IsTurnMaster() const;
};

//-----------------------------------------------------------------------------
#endif
