/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2010-2011 Warmux Team.
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
 * Maemo
 *****************************************************************************/

#include <conicconnection.h>
#include <conicconnectionevent.h>
#include "maemo/conic.h"
#include "maemo/glib.h"
#include <stdio.h>

namespace {

  static void connection_handler(ConIcConnection *connection,
                                 ConIcConnectionEvent *event,
                                 void (*cb)(bool))
  {
    printf("ConnectEvent\n");
      ConIcConnectionStatus status = con_ic_connection_event_get_status(event);
      switch(status) {
          case CON_IC_STATUS_CONNECTED:
              g_debug("Connected...");
              if (cb)
                (cb)(true);
              break;
          case CON_IC_STATUS_DISCONNECTING:
              g_debug("Disconnecting...");
              break;
          case CON_IC_STATUS_DISCONNECTED:
              g_debug("Disconnected...");
              if (cb)
                (cb)(false);
              break;
          default:
              g_debug("Unknown connection status received");
      }
  }
}

namespace Conic
{
  ConIcConnection *connection;

  void Init()
  {
    connection = con_ic_connection_new();
  }

  void ConnectionConnect(void (*cb)(bool))
  {
    printf("Connect\n");
    con_ic_connection_connect(connection, CON_IC_CONNECT_FLAG_NONE);
    g_signal_connect(connection, "connection-event", G_CALLBACK(connection_handler), (void*)cb);
    Glib::Process();
  }
}

