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
 * Network client layer for Wormux.
 *****************************************************************************/

#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H
//-----------------------------------------------------------------------------
#include "network.h"
//-----------------------------------------------------------------------------

class NetworkClient : public Network
{
protected:
  connection_state_t HandShake(TCPsocket& server_socket);
  virtual void HandleAction(Action* a, DistantComputer* sender) const;
  virtual void WaitActionSleep() {};

public:
  NetworkClient(const std::string& password);
  ~NetworkClient();

  //virtual const bool IsConnected() const { return true; }
  virtual bool IsClient() const { return true; }

  std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator);

  // Client specific methods
  connection_state_t ClientConnect(const std::string& host,
				   const std::string& port);
};

//-----------------------------------------------------------------------------
#endif
