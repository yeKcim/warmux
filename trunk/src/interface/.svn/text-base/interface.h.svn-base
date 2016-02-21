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
 * Graphical interface showing various information about the game.
 *****************************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H
#include <vector>
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "interface/weapon_menu.h"
#include "gui/progress_bar.h"
#include "weapon/weapon_strength_bar.h"

// Forward declarations
class Character;
class Team;
class Text;
class Polygon;
class Weapon;
class WeaponHelp;
class Profile;
class EnergyBar;

#ifdef WIN32
#undef interface
#endif

class Interface : public Singleton<Interface>
{
public:
  typedef enum
  {
    CLICK_TYPE_LONG,
    CLICK_TYPE_DOWN,
    CLICK_TYPE_UP
  } ClickType;

  typedef enum
  {
    MODE_NORMAL,
    MODE_CONTROL,
    MODE_SMALL,
    MODE_REPLAY
  } Mode;

  Character *character_under_cursor;
  Weapon *weapon_under_cursor;
  WeaponsMenu weapons_menu;
  WeaponHelp *help;
  Team *tmp_team;

private:
  // Regular part of the interface
  int     clock_width;

  // Timers
  Text * global_timer;
  Text * timer;
  uint remaining_turn_time;

  ProgressBar wind_bar;

  // This part is for normal interface mode
  EnergyBar * energy_bar;

  // Character information
  Text * t_character_name;
  Text * t_team_name;
  Text * t_player_name;
  Text * t_character_energy;

  // Weapon information
  Text * t_weapon_name;
  Text * t_weapon_stock;

  // Replay info
  Text * t_speed;
  void SetSpeed(const Double& speed);

  // Related to interface mode
  Mode mode;
  Surface default_toolbar;
  Surface control_toolbar;
  Surface replay_toolbar;
  Surface small_interface;

  // Other stuff
  int start_hide_display;
  int start_show_display;
  bool display_timer;
  bool display_minimap;
  WeaponStrengthBar weapon_strength_bar;

  Sprite *clock, *clock_normal, *clock_emergency;
  Point2i bottom_bar_pos;
  int last_width;
  float zoom;

  //Minimap
  Surface *minimap;
  uint m_last_minimap_redraw;
  Point2i m_last_preview_size;
  //Styled box
  Surface *mask;
  Surface *scratch;

  Color m_camera_preview_color;
  Color m_playing_character_preview_color;
  Color m_text_color, m_energy_text_color;

  void FreeDrawElements();
  void DrawCharacterInfo();
  void DrawTeamEnergy() const;
  void DrawWeaponInfo() const;
  void DrawWindIndicator(const Point2i &wind_bar_pos) const;
  void DrawWindInfo() const;
  void DrawClock(const Point2i &time_pos) const;
  void DrawTimeInfo() const;
  void DrawMapPreview();
  void DrawReplayInfo() const;
  void DrawSmallInterface() const;

  void LoadDataInternal(Profile *res);
  int GetWidth() const { return default_toolbar.GetWidth(); }
  int GetHeight() const;
  Point2i GetSize() const { return Point2i(GetWidth(), GetHeight()); }

  // Handle clicks for various states of the interface
  bool ReplayClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos = Point2i());
  bool ControlClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos = Point2i());
  bool DefaultClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos = Point2i());
  // -1 means nothing clicked, 0 means return false, 1 means return true
  int AnyClick(const Point2i &mouse_pos, ClickType type, Point2i old_mouse_pos = Point2i());

protected:
  friend class Singleton<Interface>;
  Interface();
  ~Interface();

public:
  const WeaponsMenu &GetWeaponsMenu() const { return weapons_menu; }

  void Reset();
  void Draw();
  void LoadData();

  Point2i GetMenuPosition() const { return bottom_bar_pos; }
  bool IsDisplayed () const { return mode != MODE_SMALL; }
  Mode GetMode() const { return mode; }
  void SetMode(Mode new_mode) { mode = new_mode; }
  void EnableDisplay(bool display)
  {
    if (mode==MODE_REPLAY) return;
    mode = (display) ? MODE_NORMAL : MODE_SMALL;
  }
  void Show();
  void Hide();

  void SetCurrentOverflyWeapon(Weapon * weapon) { weapon_under_cursor = weapon; }
  void UpdateTimer(uint utimer, bool emergency, bool reset_anim);
  void UpdateWindIndicator(int wind_value);
  void EnableDisplayTimer(bool _display) { display_timer = _display; }
  void ToggleMinimap() { display_minimap = !display_minimap; }
  void MinimapSizeDelta(int delta);
  bool Intersect(const Point2i &mouse_pos);

  bool ActionClickUp(const Point2i &mouse_pos, const Point2i &old_click_pos);
  bool ActionLongClick(const Point2i &mouse_pos, const Point2i &old_mouse_pos);
  bool ActionClickDown(const Point2i &mouse_pos);
  bool ActionDoubleClick(const Point2i &mouse_pos);

  int GetMenuHeight() const;
};

void AbsoluteDraw(const Surface& s, const Point2i& pos);
void HideGameInterface();
inline void ShowGameInterface() { Interface::GetInstance()->Show(); }

#endif
