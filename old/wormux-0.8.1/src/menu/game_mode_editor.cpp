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

#include <vector>

#include "menu/game_mode_editor.h"

#include "game/config.h"
#include "game/game_mode.h"
#include "gui/combo_box.h"
#include "gui/spin_button_picture.h"
#include "tool/i18n.h"

const uint TPS_TOUR_MIN = 10;
const uint TPS_TOUR_MAX = 240;

GameModeEditor::GameModeEditor(uint max_line_width, const Point2i& option_size, bool _draw_border) :
  GridBox(max_line_width, option_size, _draw_border)
{
  // ################################################
  // ##  GAME OPTIONS
  // ################################################

  std::vector<std::pair<std::string, std::string> > game_modes;
  game_modes.push_back(std::pair<std::string, std::string>("classic", _("Classic")));
  game_modes.push_back(std::pair<std::string, std::string>("unlimited", _("Unlimited")));
  game_modes.push_back(std::pair<std::string, std::string>("blitz", _("Blitz")));

  opt_game_mode = new ComboBox(_("Game mode"), "menu/game_mode", option_size,
			       game_modes, Config::GetInstance()->GetGameMode());
  AddWidget(opt_game_mode);

  opt_duration_turn = new SpinButtonWithPicture(_("Duration of a turn"), "menu/timing_turn",
                                                option_size,
                                                TPS_TOUR_MIN, 10,
                                                TPS_TOUR_MIN, TPS_TOUR_MAX);
  AddWidget(opt_duration_turn);

  std::vector<std::pair<std::string, std::string> > character_selections;
  character_selections.push_back(std::pair<std::string, std::string>("always", _("Always")));
  character_selections.push_back(std::pair<std::string, std::string>("before_action", _("Before action")));
  character_selections.push_back(std::pair<std::string, std::string>("never", _("Never")));

  opt_allow_character_selection = new ComboBox(_("Character switching"), "menu/character_selection", option_size,
					       character_selections, "always");
  AddWidget(opt_allow_character_selection);

  /* Characters energy */
  opt_energy_ini = new SpinButtonWithPicture(_("Initial energy"), "menu/init_energy",
                                             option_size,
                                             10, 10,
                                             10, 500);
  AddWidget(opt_energy_ini);

  opt_energy_max = new SpinButtonWithPicture(_("Max energy"), "menu/max_energy",
                                             option_size,
                                             10, 10,
                                             10, 500);
  AddWidget(opt_energy_max);


  /* some death mode options */

  opt_time_before_death_mode = new SpinButtonWithPicture(_("Duration before death mode"), "menu/timing_death",
							 option_size,
							 200, 50,
							 200, 3000);
  AddWidget(opt_time_before_death_mode);

  opt_damage_during_death_mode = new SpinButtonWithPicture(_("Damage per turn during death mode"), "menu/death_energy",
							   option_size,
							   1, 1,
							   1, 20);
  AddWidget(opt_damage_during_death_mode);

  opt_gravity = new SpinButtonWithPicture(_("Gravity"), "menu/gravity",
							   option_size,
							   10, 5,
							   10, 60);
  AddWidget(opt_gravity);

  LoadGameMode();
}

GameModeEditor::~GameModeEditor()
{
}


void GameModeEditor::LoadGameMode()
{
  Config::GetInstance()->SetGameMode(opt_game_mode->GetValue());
  GameMode * game_mode = GameMode::GetInstance();
  game_mode->Load();

  if (game_mode->allow_character_selection == GameMode::ALWAYS) {
    opt_allow_character_selection->SetChoice(0); // "always"
  } else if (game_mode->allow_character_selection == GameMode::BEFORE_FIRST_ACTION) {
    opt_allow_character_selection->SetChoice(1); // "before_action"
  } else if (game_mode->allow_character_selection == GameMode::NEVER) {
    opt_allow_character_selection->SetChoice(2); // "never"
  } else {
    ASSERT(false);
  }

  opt_duration_turn->SetValue(game_mode->duration_turn);
  opt_energy_ini->SetValue(game_mode->character.init_energy);
  opt_energy_max->SetValue(game_mode->character.max_energy);
  opt_time_before_death_mode->SetValue(game_mode->duration_before_death_mode);
  opt_damage_during_death_mode->SetValue(game_mode->damage_per_turn_during_death_mode);
  opt_gravity->SetValue((int)(game_mode->gravity));

  NeedRedrawing();
}

void GameModeEditor::ValidGameMode() const
{
  GameMode * game_mode = GameMode::GetInstance();
  game_mode->Load();

  if (opt_allow_character_selection->GetValue() == "always") {
    game_mode->allow_character_selection = GameMode::ALWAYS;
  } else if (opt_allow_character_selection->GetValue() == "before_action") {
    game_mode->allow_character_selection = GameMode::BEFORE_FIRST_ACTION;
  } else if (opt_allow_character_selection->GetValue() == "never") {
    game_mode->allow_character_selection = GameMode::NEVER;
  } else {
    ASSERT(false);
  }

  game_mode->duration_turn = opt_duration_turn->GetValue();
  game_mode->character.init_energy = opt_energy_ini->GetValue();
  game_mode->character.max_energy = opt_energy_max->GetValue();
  game_mode->duration_before_death_mode = opt_time_before_death_mode->GetValue();
  game_mode->damage_per_turn_during_death_mode = opt_damage_during_death_mode->GetValue();
  game_mode->gravity = opt_gravity->GetValue();
}

