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
 * Fake Network layer for Wormux.
 *****************************************************************************/

#ifndef NETWORK_LOCAL_H
#define NETWORK_LOCAL_H
//-----------------------------------------------------------------------------
#include "network.h"
//-----------------------------------------------------------------------------

class NetworkLocal : public Network
{
protected:
  virtual void HandleAction(Action* /*a*/, DistantComputer* /*sender*/) const { ASSERT(false) };
  virtual void WaitActionSleep() { ASSERT(false) };

public:
  NetworkLocal();
  ~NetworkLocal();

  virtual bool IsConnected() const { return false; }
  virtual bool IsLocal() const { return true; }

  virtual void SendAction(const Action& action) const;

  virtual std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed);

};

//-----------------------------------------------------------------------------
#endif
