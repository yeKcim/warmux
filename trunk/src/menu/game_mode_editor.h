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
 * Game mode editor
 *****************************************************************************/

#ifndef GAME_MODE_EDITOR_H
#define GAME_MODE_EDITOR_H

#include "gui/vertical_box.h"

// Forward declarations
class SpinButtonWithPicture;
class ComboBox;
class CheckBox;
class ScrollBox;
class TextBox;
class ItemBox;
class Button;
class WeaponsList;
class WeaponCfgBox;

class GameModeEditor : public VBox
{
  ItemBox               *opt_game_mode;
  std::vector<std::pair<std::string, std::string> > game_modes;
  TextBox               *filename;
  Button                *save;

  ComboBox              *opt_rules;
  ComboBox              *opt_allow_character_selection;

  SpinButtonWithPicture *opt_duration_turn;

  SpinButtonWithPicture *opt_energy_ini;
  SpinButtonWithPicture *opt_energy_max;

  SpinButtonWithPicture *opt_time_before_death_mode;
  SpinButtonWithPicture *opt_damage_during_death_mode;
  SpinButtonWithPicture *opt_gravity;

  ScrollBox             *opt_weapons_cfg;
  std::list<WeaponCfgBox*> weapon_cfg_list;

  void LoadGameMode(bool force = false);
  void Apply();
  void WarnBlitz();

public:
  GameModeEditor(const Point2i& size, float zoom, bool _draw_border=true);

  Widget *ClickUp(const Point2i & mousePosition, uint button);
  void ValidGameMode();
};

#endif
