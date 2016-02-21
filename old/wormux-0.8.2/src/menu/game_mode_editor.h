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
 * Game mode editor
 *****************************************************************************/

#ifndef GAME_MODE_EDITOR_H
#define GAME_MODE_EDITOR_H

#include "gui/box.h"

// Forward declarations
class SpinButtonWithPicture;
class ComboBox;
class CheckBox;

class GameModeEditor : public GridBox
{
  /* If you need this, implement it (correctly)*/
  GameModeEditor(const GameModeEditor&);
  GameModeEditor operator=(const GameModeEditor&);
  /********************************************/

  ComboBox *opt_game_mode;

  ComboBox *opt_allow_character_selection;

  SpinButtonWithPicture *opt_duration_turn;

  SpinButtonWithPicture *opt_energy_ini;
  SpinButtonWithPicture *opt_energy_max;

  SpinButtonWithPicture *opt_time_before_death_mode;
  SpinButtonWithPicture *opt_damage_during_death_mode;
  SpinButtonWithPicture *opt_gravity;

public:
  GameModeEditor(uint max_line_width, const Point2i& option_size, bool _draw_border=true);
  ~GameModeEditor();

  const ComboBox* GetGameModeComboBox() const { return opt_game_mode; };

  void LoadGameMode();
  void ValidGameMode() const;
};

#endif
