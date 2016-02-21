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
#include "../include/action.h"
#include "../include/base.h" 
#include "../menu/network_menu.h"
//-----------------------------------------------------------------------------
const std::string WORMUX_NETWORK_PORT = "9999";
const uint WORMUX_NETWORK_PORT_INT = 9999;

class Network
{
  friend class DistantComputer;

  bool inited;

#if defined(DEBUG) && not defined(WIN32)
  int fout;
  int fin;	
#endif

protected:
  bool m_is_connected;
  bool m_is_server;
  bool m_is_client;

  TCPsocket server_socket; // Wait for incoming connections on this socket
  SDL_Thread* thread; // network thread, where we receive data from network
  SDLNet_SocketSet socket_set;
  IPaddress ip; // for server : store listening port
                // for client : store server address/port

public:
  NetworkMenu* network_menu;

  typedef enum
    {
      NETWORK_NOT_CONNECTED,
      NETWORK_OPTION_SCREEN,
      NETWORK_INIT_GAME,
      NETWORK_READY_TO_PLAY,
      NETWORK_PLAYING
    } network_state_t;
  network_state_t state;

  std::list<DistantComputer*> cpu; // list of the connected computer
  uint max_player_number;
  uint connected_player;
  uint client_inited;
  bool sync_lock;
  std::string nickname; //Clients: Send to Server at connect
                        //Server: Send in chat messages

  Network();
  ~Network();
  void Init();
  
  const bool IsConnected() const;
  const bool IsLocal() const;
  const bool IsServer() const;
  const bool IsClient() const;
  const uint GetPort();
  
  // Network functions common to client and server
  void Disconnect();

  // Action handling
  void SendAction(Action* action);
  void SendPacket(char* packet, int size);
  void ReceiveActions();
  // Client specific
  void ClientConnect(const std::string &host, const std::string &port);

  // Serveur specific methods
  void ServerStart(const std::string &port);
  void AcceptIncoming();
  void RejectIncoming();
  std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed);

  void SendChatMessage(std::string txt);
};

extern Network network;
//-----------------------------------------------------------------------------
#endif
