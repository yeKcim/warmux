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
 * Options menu
 *****************************************************************************/

#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "menu.h"
#include <vector>


class ButtonPic;
class ItemBox;
class CheckBox;
class ComboBox;
class CustomTeam;
class SpinButtonWithPicture;
class TextBox;
class ControlConfig;

class OptionMenu : public Menu
{
private:
  /* Graphic options controllers */
#ifdef ENABLE_NLS
  ItemBox *lbox_languages;
#endif
  SpinButtonWithPicture *opt_wind_particles_percentage;
  CheckBox *opt_display_energy;
  CheckBox *opt_display_name;
#ifndef HAVE_HANDHELD
  CheckBox *opt_display_multisky;
#endif

#ifndef HAVE_HANDHELD
  ComboBox *cbox_video_mode;
#endif
  SpinButtonWithPicture *opt_max_fps;

  /* Sound options controllers */
#ifndef HAVE_HANDHELD
  ComboBox *cbox_sound_freq;
#endif
  uint initial_vol_mus;
  uint initial_vol_eff;
  SpinButtonWithPicture *volume_music;
  SpinButtonWithPicture *volume_effects;
  CheckBox *music_cbox;
  CheckBox *effects_cbox;
  CheckBox *warn_cbox;

  /* Misc options controllers */
  CheckBox *opt_updates;
#ifndef HAVE_TOUCHSCREEN
  CheckBox *full_screen;
  CheckBox *opt_lefthanded_mouse;
  CheckBox *opt_scroll_on_border;
  SpinButtonWithPicture * opt_scroll_border_size;
#endif

  void SaveOptions();
  void OnClickUp(const Point2i &mousePosition, int button);
  static uint fromVolume(uint vol);
  static uint toVolume(uint level);

  /* Teams controllers */

  ItemBox *lbox_teams;
  Button *add_team;
  Button *delete_team;
  CustomTeam  *selected_team;
  TextBox *tbox_team_name;
  Label *team_name;
  std::vector<TextBox *> tbox_character_name_list;

#ifdef ENABLE_NLS
  void AddLanguageItem(const char* label, const char* value);
#endif
  bool TeamInfoValid();
  void AddTeam();
  void DeleteTeam();
  void LoadTeam();
  void ReloadTeamList();
  bool SaveTeam();
  void SelectTeam();

  /* Controls config */
  ControlConfig *controls;

  bool signal_ok();
  bool signal_cancel();

public:
  OptionMenu();
  static void CheckUpdates();
};

#endif
