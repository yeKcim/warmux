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
 * Interface affichant différentes informations sur la jeu.
 *****************************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H
#include <vector>
#include "weapon_menu.h"
#include "../graphic/surface.h"
#include "../graphic/sprite.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../team/character.h"
#include "../team/team.h"

#ifdef WIN32
#undef interface
#endif

class Interface
{

public:
  Character *character_under_cursor;
  Weapon* weapon_under_cursor;
  WeaponsMenu weapons_menu;
  Surface weapon_box_button;

 private:
   // Timers
   Text * global_timer;
   Text * timer;
   
   // Character information
   Text * t_NAME;
   Text * t_character_name;
   
   Text * t_ENERGY;
   Text * t_character_energy;
   
   // Weapon information
   Text * t_WEAPON;
   Text * t_weapon_name;
   Text * t_STOCK;
   Text * t_weapon_stock;
   
   bool display;
   bool display_timer;
   void DisplayCharacterInfo ();
   void DisplayWeaponInfo ();
   BarreProg barre_energie;

   Surface game_menu;
   Surface bg_time;
   Point2i bottom_bar_pos;

   static Interface * singleton;

 private:
   Interface();
   ~Interface();   
   
 public:
   static Interface * GetInstance();

   void Init();
   void Reset();
   void Draw();
   
   bool IsDisplayed () const { return display; };
   void EnableDisplay (bool _display);
   
   int GetWidth() const;
   int GetHeight() const;
   Point2i GetSize() const;
   
   void UpdateTimer(uint utimer);
   void EnableDisplayTimer (bool _display) {display_timer = _display;};
};

void AbsoluteDraw(Surface& s, Point2i pos);

#endif
