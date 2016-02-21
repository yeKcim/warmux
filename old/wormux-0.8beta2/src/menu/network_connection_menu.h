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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/

#ifndef NETWORK_CONNECTION_MENU_H
#define NETWORK_CONNECTION_MENU_H

#include <list>
#include "menu.h"
#include "network/network.h"

// Forward declarations
class TextBox;
class VBox;
class MsgBox;
class CheckBox;

class NetworkConnectionMenu : public Menu
{
 public:
  typedef enum {
    NET_HOST,
    NET_CONNECT_LOCAL,
    NET_BROWSE_INTERNET
  } network_menu_action_t;

 private:
  /* If you need this, implement it (correctly)*/
   NetworkConnectionMenu(const NetworkConnectionMenu&);
   NetworkConnectionMenu operator=(const NetworkConnectionMenu&);
   /********************************************/

   /* Connection controllers */
   Button *previous_action_bt, *next_action_bt;

   Label* action_label;
   network_menu_action_t current_action;

   Label* server_address_label;
   TextBox* server_address;

   Label* port_number_label;
   TextBox* port_number;

   CheckBox* internet_server;
   VBox* connection_box;

   MsgBox *msg_box;

   void OnClick(const Point2i &mousePosition, int button);
   void OnClickUp(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);

   void DisplayError(Network::connection_state_t conn);

   bool signal_ok();
   bool signal_cancel();

public:
   NetworkConnectionMenu();
   ~NetworkConnectionMenu();

   void SetAction(network_menu_action_t action);
};

#endif
