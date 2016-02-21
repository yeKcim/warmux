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
 * Graphical interface showing various information about the game.
 *****************************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H
#include <vector>
#include "include/base.h"
#include "include/singleton.h"
#include "weapon_menu.h"
#include "graphic/colors.h"
#include "gui/energy_bar.h"
#include "gui/energy_bar.h"
#include "gui/progress_bar.h"

// Forward declarations
class Character;
class Team;
class Text;
class Polygon;
class Weapon;

#ifdef WIN32
#undef interface
#endif

class Interface : public Singleton<Interface>
{
public:
  Character *character_under_cursor;
  Weapon *weapon_under_cursor;
  WeaponsMenu weapons_menu;
  Team *tmp_team;

 private:
   /* If you need this, implement it (correctly)*/
   Interface(const Interface&);
   const Interface& operator=(const Interface&);
   /*********************************************/

   // Timers
   Text * global_timer;
   Text * timer;
   uint remaining_turn_time;

   // Character information
   Text * t_character_name;
   Text * t_team_name;
   Text * t_player_name;

   Text * t_character_energy;

   // Weapon information
   Text * t_weapon_name;
   Text * t_weapon_stock;

   bool display;
   int start_hide_display;
   int start_show_display;
   bool display_timer;
   bool display_minimap;
   EnergyBar energy_bar;
   ProgressBar wind_bar;

   Surface game_menu;
   Surface clock_background;
   Surface small_background_interface;
   Sprite * clock;
   Surface wind_icon;
   Surface wind_indicator;
   Point2i bottom_bar_pos;

protected:
  friend class Singleton<Interface>;
   Interface();
   ~Interface();

 public:
   const WeaponsMenu &GetWeaponsMenu() const { return weapons_menu; };

   void Reset();
   void Draw();

   void DrawCharacterInfo();
   void DrawTeamEnergy() const;
   void DrawWeaponInfo() const;
   void DrawWindIndicator(const Point2i &wind_bar_pos, const bool draw_icon) const;
   void DrawWindInfo() const;
   void DrawClock(const Point2i &time_pos) const;
   void DrawTimeInfo() const;
   void DrawMapPreview();
   void DrawSmallInterface() const;

   bool IsDisplayed () const { return display; };
   void EnableDisplay(bool _display);
   void Show();
   void Hide();
   bool IsVisible() const { return display; };

   int GetWidth() const { return game_menu.GetWidth(); };
   int GetHeight() const;
   int GetMenuHeight() const;
   Point2i GetSize() const;

   void SetCurrentOverflyWeapon(Weapon * weapon) { weapon_under_cursor = weapon; };
   void UpdateTimer(uint utimer, const Color& color = black_color);
   void UpdateWindIndicator(int wind_value) { wind_bar.UpdateValue(wind_value); };
   void EnableDisplayTimer (bool _display) {display_timer = _display;};
   void ToggleMinimap() { display_minimap = !display_minimap; };
};

void AbsoluteDraw(const Surface& s, const Point2i& pos);
void HideGameInterface();
void ShowGameInterface();

#endif
