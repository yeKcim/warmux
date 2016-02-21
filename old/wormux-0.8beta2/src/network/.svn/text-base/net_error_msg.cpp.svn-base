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
 *  Displays network related error messages in a pop-up
 *****************************************************************************/

#include "net_error_msg.h"
#include "network.h"
#include "tool/i18n.h"
#include "gui/question.h"

void DispNetworkError(Network::connection_state_t err)
{
  Question question;
  std::string msg;
  switch(err)
  {
  case Network::CONNECTED:
    msg = _("Connected !");
    break;
  case Network::CONN_BAD_HOST:
    msg = _("Unable to contact host.");
    break;
  case Network::CONN_BAD_PORT:
    msg = _("Unable to use this port!");
    break;
  case Network::CONN_BAD_SOCKET:
    msg = _("Bad socket ...");
    break;
  case Network::CONN_REJECTED:
    msg = _("The server rejected the connection.");
    break;
  case Network::CONN_TIMEOUT:
    msg = _("The connection timed out. Check there is no firewall in the way!");
    break;
  default: ASSERT(false);
  }

  question.Set(msg, 1, 0);
  question.Ask();
}

