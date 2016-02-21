/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Game menu
 *****************************************************************************/

#ifndef GAME_MENU_H
#define GAME_MENU_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "menu.h"
//-----------------------------------------------------------------------------

class GameMenu : public Menu
{
 public:
   GameMenu(); 
   ~GameMenu();

 private:
   bool terrain_init;

   /* Team controllers */
   ListBox *lbox_all_teams;   
   ListBox *lbox_selected_teams;
   Button *bt_add_team;
   Button *bt_remove_team;
   NullWidget *space_for_logo;
   VBox *team_box;

   /* Map controllers */
   ListBox *lbox_maps;
   Sprite *map_preview;  
   VBox *map_box;

   void ChangeMap();   
   void MoveTeams(ListBox * from, ListBox * to, bool sort);

   void SaveOptions();
   void OnClic ( int mouse_x, int mouse_y, int button);
   void Draw(int mouse_x, int mouse_y);


   void __sig_ok();
   void __sig_cancel();
};

#endif
