/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "options_menu.h"

#include "include/app.h"
#include "game/game_mode.h"
#include "game/config.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "map/maps_list.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/string_tools.h"
#include <sstream>

const uint SOUND_X = 30;
const uint SOUND_Y = 30;
const uint SOUND_W = 530;
const uint SOUND_H = 170;

const uint GRAPHIC_X = 30;
const uint GRAPHIC_Y = SOUND_Y + SOUND_H + 30;
const uint GRAPHIC_W = 530;
const uint GRAPHIC_H = 240;

OptionMenu::OptionMenu() :
  Menu("menu/bg_option")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  Rectanglei stdRect (0, 0, 140, 30);

  /* Grapic options */
  Box * graphic_options = new HBox( Rectanglei(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W, GRAPHIC_H));

  graphic_options->AddWidget(new PictureWidget(Rectanglei(0,0,40,136), "menu/video_label"));

  Box * top_n_bottom_graphic_options = new VBox( Rectanglei(0, 0, GRAPHIC_W-40, GRAPHIC_H),false);

  Box * top_graphic_options = new HBox ( Rectanglei(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W, GRAPHIC_H/2 - 20), false);
  Box * bottom_graphic_options = new HBox ( Rectanglei(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W, GRAPHIC_H/2 - 20), false);
  top_graphic_options->SetMargin(25);
  bottom_graphic_options->SetMargin(25);

  opt_display_wind_particles = new PictureTextCBox(_("Wind particles?"), "menu/display_wind_particles", stdRect);
  top_graphic_options->AddWidget(opt_display_wind_particles);

  opt_display_energy = new PictureTextCBox(_("Player energy?"), "menu/display_energy", stdRect);
  top_graphic_options->AddWidget(opt_display_energy);

  opt_display_name = new PictureTextCBox(_("Player's name?"), "menu/display_name", stdRect);
  top_graphic_options->AddWidget(opt_display_name);

  full_screen = new PictureTextCBox(_("Fullscreen?"), "menu/fullscreen", stdRect);
  bottom_graphic_options->AddWidget(full_screen);

  opt_max_fps = new SpinButtonBig(_("Maximum FPS"), stdRect,
			       50, 5,
			       20, 120);
  bottom_graphic_options->AddWidget(opt_max_fps);

  lbox_video_mode = new ListBoxWithLabel(_("Resolution"), stdRect );
  bottom_graphic_options->AddWidget(lbox_video_mode);

  top_n_bottom_graphic_options->AddWidget(top_graphic_options);
  top_n_bottom_graphic_options->AddWidget(bottom_graphic_options);
  graphic_options->AddWidget(top_n_bottom_graphic_options);

  widgets.AddWidget(graphic_options);

  /* Sound options */
  Box * sound_options = new HBox( Rectanglei(SOUND_X, SOUND_Y, SOUND_W, SOUND_H));
  sound_options->AddWidget(new PictureWidget(Rectanglei(0,0,40,138), "menu/audio_label"));

  Box * all_sound_options = new HBox( Rectanglei(SOUND_X, SOUND_Y, SOUND_W, SOUND_H-20),false);
  all_sound_options->SetMargin(25);
  all_sound_options->SetBorder(Point2i(10,10));

  opt_music = new PictureTextCBox(_("Music?"), "menu/music_enable", stdRect);
  all_sound_options->AddWidget(opt_music);

  opt_sound_effects = new PictureTextCBox(_("Sound effects?"), "menu/sound_effects_enable", stdRect);
  all_sound_options->AddWidget(opt_sound_effects);

  lbox_sound_freq = new ListBoxWithLabel(_("Sound frequency"), stdRect );
  all_sound_options->AddWidget(lbox_sound_freq);

  sound_options->AddWidget(all_sound_options);
  widgets.AddWidget(sound_options);

  /* Center the widgets */
  AppWormux * app = AppWormux::GetInstance();
  uint center_x = app->video.window.GetWidth()/2;

  graphic_options->SetXY(center_x - graphic_options->GetSizeX()/2, graphic_options->GetPositionY());
  sound_options->SetXY(center_x - sound_options->GetSizeX()/2, sound_options->GetPositionY());

  // Values initialization

  // Get available video resolution
  std::list<Point2i>& video_res = app->video.GetAvailableConfigs();
  std::list<Point2i>::iterator mode;

  for(mode=video_res.begin(); mode!=video_res.end(); ++mode) {
      std::ostringstream ss;
      bool is_current;
      std::string text;
      ss << mode->GetX() << "x" << mode->GetY() ;
      text = ss.str();
      if (app->video.window.GetWidth() == mode->GetX() && app->video.window.GetHeight() == mode->GetY())
      {
          ss << " " << _("(current)");
          is_current = true;
      } else {
          is_current = false;
      }
      lbox_video_mode->AddItem(is_current, ss.str(), text);
  }

  // Generate sound mode list
  uint current_freq = jukebox.GetFrequency();
  lbox_sound_freq->AddItem (current_freq == 11025, "11 kHz", "11025");
  lbox_sound_freq->AddItem (current_freq == 22050, "22 kHz", "22050");
  lbox_sound_freq->AddItem (current_freq == 44100, "44 kHz", "44100");

  resource_manager.UnLoadXMLProfile( res);

  Config * config = Config::GetInstance();

  opt_max_fps->SetValue (app->video.GetMaxFps());
  opt_display_wind_particles->SetValue (config->GetDisplayWindParticles());
  opt_display_energy->SetValue (config->GetDisplayEnergyCharacter());
  opt_display_name->SetValue (config->GetDisplayNameCharacter());
  full_screen->SetValue (app->video.IsFullScreen());


  opt_music->SetValue( jukebox.UseMusic() );
  opt_sound_effects->SetValue( jukebox.UseEffects() );
}

OptionMenu::~OptionMenu()
{
}

void OptionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

void OptionMenu::OnClick(const Point2i &mousePosition, int button)
{

}

void OptionMenu::SaveOptions()
{
  // Save values
  Config * config = Config::GetInstance();
  config->SetDisplayWindParticles(opt_display_wind_particles->GetValue());
  config->SetDisplayEnergyCharacter(opt_display_energy->GetValue());
  config->SetDisplayNameCharacter(opt_display_name->GetValue());

  // Sound settings
  config->SetSoundEffects(opt_sound_effects->GetValue());
  config->SetSoundMusic(opt_music->GetValue());
  config->SetSoundFrequency(lbox_sound_freq->ReadIntValue());

  AppWormux * app = AppWormux::GetInstance();
  app->video.SetMaxFps(opt_max_fps->GetValue());
  // Video mode
  std::string s_mode = lbox_video_mode->ReadValue();
  int w, h;
  sscanf(s_mode.c_str(),"%dx%d", &w, &h);
  app->video.SetConfig(w, h, full_screen->GetValue());

  uint x = app->video.window.GetWidth() / 2;
  uint y = app->video.window.GetHeight() - 50;

  SetActionButtonsXY(x, y);

  // Sound
  jukebox.ActiveMusic( opt_music->GetValue() );
  jukebox.ActiveEffects( opt_sound_effects->GetValue() );
  std::string sfreq = lbox_sound_freq->ReadValue();
  long freq;
  if (str2long(sfreq,freq)) jukebox.SetFrequency (freq);

  jukebox.Init(); // commit modification on sound options

  //Save options in XML
  config->Save();
}

bool OptionMenu::signal_ok()
{
  SaveOptions();
  return true;
}

bool OptionMenu::signal_cancel()
{
  return true;
}

void OptionMenu::Draw(const Point2i &mousePosition)
{
}

