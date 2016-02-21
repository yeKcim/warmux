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
 * Network server layer for Wormux.
 *****************************************************************************/

#ifndef NETWORK_SERVER_H 
#define NETWORK_SERVER_H
//-----------------------------------------------------------------------------
#include "network.h"
//-----------------------------------------------------------------------------

class NetworkServer : public Network
{
  uint max_nb_players;
  TCPsocket server_socket; // Wait for incoming connections on this socket
public:
  NetworkServer();
  ~NetworkServer();

  virtual const bool IsConnected() const { return true; }
  virtual const bool IsServer() const { return true; }
  
  virtual void SendChatMessage(const std::string& txt);
  virtual void ReceiveActions();

  // Serveur specific methods
  Network::connection_state_t ServerStart(const std::string &port);

  bool AcceptIncoming();
  void RejectIncoming();
  std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed);
  void SetMaxNumberOfPlayers(uint max_nb_players);
  const uint GetNbConnectedPlayers() const;
  const uint GetNbInitializedPlayers() const;
  const uint GetNbReadyPlayers() const;
};

//-----------------------------------------------------------------------------
#endif
