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
 * Irc client
 *****************************************************************************/

#ifndef IRC_H
#define IRC_H
#include <SDL_net.h>
#include <map>
#include <string>
#include "network/socket.h"

class IrcChat
{
  std::map<std::string, int> server_lst;
  void HandleCommand(std::string cmd);
  Socket sock;

public:
  IrcChat();
  ~IrcChat();

  connection_state_t Connect();
  void Disconnect();

  std::string GetChat();
};

#endif
