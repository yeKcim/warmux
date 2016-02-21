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

#if defined (MAEMO) || defined(ANDROID)
#  define USE_MISC_TAB  0
#else
#  define USE_MISC_TAB  1
#endif

class ButtonPic;
class ItemBox;
class CheckBox;
class ComboBox;
class CustomTeam;
class SpinButtonWithPicture;
class TextBox;
class ControlConfig;
class CheckBox;
class VBox;
class SocialPanel;

class SocialWidget;

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

#ifndef HAVE_TOUCHSCREEN
  ComboBox *cbox_video_mode;
  CheckBox *full_screen;
#endif
  SpinButtonWithPicture *opt_max_fps;
  ComboBox *opt_quality;

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
#if USE_MISC_TAB
  CheckBox *opt_updates;
# ifndef HAVE_TOUCHSCREEN
  CheckBox *opt_lefthanded_mouse;
  CheckBox *opt_scroll_on_border;
  SpinButtonWithPicture * opt_scroll_border_size;
# endif
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

  /* Social */
#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
  SocialPanel *social_panel;
#endif

#ifdef ENABLE_NLS
  void AddLanguageItem(const char* label, const char* value, uint fsize);
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
