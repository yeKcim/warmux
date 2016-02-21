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

#ifndef INTERNET_MENU_H
#define INTERNET_MENU_H

#include "menu.h"
#include "network/irc.h"

// Forward declarations
class ButtonText;
class VBox;
class ListBox;

class InternetMenu : public Menu
{
   IrcChat irc;
   VBox* connection_box;
   ListBox* connect_lst;
   ButtonText* refresh;
   ButtonText* connect;

   void OnClick(const Point2i &mousePosition, int button);
   void OnClickUp(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);
   void RefreshList(bool warning_if_empty);
   void DisplayNoGameRunning();

   bool signal_ok();
   bool signal_cancel();

public:
   InternetMenu(); 
   ~InternetMenu();
};

#endif
