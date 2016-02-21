/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 *  Displays network related error messages in a pop-up
 *****************************************************************************/

#include "network/net_error_msg.h"
#include "network/network.h"
#include "gui/question.h"

const std::string NetworkErrorToString(connection_state_t err)
{
  switch(err) {
  case CONNECTED:        return _("Connected !");
  case CONN_BAD_HOST:    return _("Unable to contact the host.");
  case CONN_BAD_PORT:    return _("Unable to use this port!");
  case CONN_BAD_SOCKET:  return _("Bad socket ...");
  case CONN_REJECTED:    return _("The server rejected the connection.");
  case CONN_TIMEOUT:     return _("The connection timed out. Check there is no firewall in the way!");
  case CONN_WRONG_PASSWORD: return _("The server rejected the connection: wrong password!");
  default: ASSERT(false); return "";
  }
}

