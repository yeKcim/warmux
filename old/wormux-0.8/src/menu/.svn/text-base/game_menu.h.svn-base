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
 * Game menu
 *****************************************************************************/

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "menu.h"

// Forward declarations
class TeamsSelectionBox;
class MapSelectionBox;
class SpinButtonWithPicture;
class CheckBox;

class GameMenu : public Menu
{
  /* If you need this, implement it (correctly)*/
   GameMenu(const GameMenu&);
   GameMenu operator=(const GameMenu&);
   /********************************************/

   /* Team controllers */
   TeamsSelectionBox * team_box;

   /* Map controllers */
   MapSelectionBox * map_box;

   /* Game options controllers */
   Box * game_options;
   SpinButtonWithPicture *opt_duration_turn;
   SpinButtonWithPicture *opt_energy_ini;

   void SaveOptions();
   void OnClick(const Point2i &mousePosition, int button);
   void OnClickUp(const Point2i &mousePosition, int button);
   void Draw(const Point2i &mousePosition);

   bool signal_ok();
   bool signal_cancel();
   void key_left();
   void key_right();

public:
   GameMenu();
   ~GameMenu();
};

#endif
