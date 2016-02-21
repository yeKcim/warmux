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
 * Network connection menu: this menu allows the user to choose between
 * hosting a game or connecting to a server.
 *****************************************************************************/

#ifndef NETWORK_CONNECTION_MENU_H
#define NETWORK_CONNECTION_MENU_H

#include <list>
#include "menu.h"
#include "../include/base.h"
#include "../graphic/font.h"
#include "../gui/button_text.h"
#include "../gui/msg_box.h"
#include "../gui/text_box.h"

class NetworkConnectionMenu : public Menu
{
   Font * normal_font;
   Font * big_font;

   /* Connection controllers */
   TextBox* server_adress;
   ButtonText* start_client;
   ButtonText* start_server;
   VBox* connection_box;

   MessageBox *msg_box;

   void OnClic(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);

   void __sig_ok();
   void __sig_cancel();

public:
   NetworkConnectionMenu(); 
   ~NetworkConnectionMenu();
};

#endif
