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

#include "menu/options_menu.h"

#include "include/app.h"
#include "game/game_mode.h"
#include "game/config.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/box.h"
#include "gui/list_box.h"
#include "gui/combo_box.h"
#include "gui/check_box.h"
#include "gui/picture_widget.h"
#include "gui/picture_text_cbox.h"
#include "gui/spin_button_picture.h"
#include "gui/list_box_w_label.h"

#include "map/maps_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/string_tools.h"
#include "tool/resource_manager.h"
#include <sstream>

const uint SOUND_Y = 10;
const uint SOUND_W = 530;
const uint SOUND_H = 200;

const uint GRAPHIC_W = 530;
const uint GRAPHIC_H = 330;

OptionMenu::OptionMenu() :
  Menu("menu/bg_option")
{
  AppWormux * app = AppWormux::GetInstance();
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  Point2i stdSize(140, -1);

  /* Graphic options */
  Box * graphic_options = new HBox(GRAPHIC_H);
  graphic_options->AddWidget(new PictureWidget(Point2i(40, 136), "menu/video_label"));

  Box * top_n_bottom_graphic_options = new VBox(GRAPHIC_W - 40,false);
  Box * top_graphic_options = new HBox(GRAPHIC_H / 2 - 20, false);
  Box * bottom_graphic_options = new HBox(GRAPHIC_H / 2 - 20, false);
  top_graphic_options->SetMargin(25);
  bottom_graphic_options->SetMargin(25);

  // Various options
  opt_display_wind_particles = new PictureTextCBox(_("Wind particles?"), "menu/display_wind_particles", stdSize);
  top_graphic_options->AddWidget(opt_display_wind_particles);

  opt_display_energy = new PictureTextCBox(_("Player energy?"), "menu/display_energy", stdSize);
  top_graphic_options->AddWidget(opt_display_energy);

  opt_display_name = new PictureTextCBox(_("Player's name?"), "menu/display_name", stdSize);
  top_graphic_options->AddWidget(opt_display_name);

  full_screen = new PictureTextCBox(_("Fullscreen?"), "menu/fullscreen", stdSize);
  bottom_graphic_options->AddWidget(full_screen);

  opt_max_fps = new SpinButtonWithPicture(_("Maximum FPS"), "menu/fps",
					  stdSize,
					  50, 5,
					  20, 50);
  bottom_graphic_options->AddWidget(opt_max_fps);


  // Get available video resolution
  const std::list<Point2i>& video_res = app->video->GetAvailableConfigs(); 
  std::list<Point2i>::const_iterator mode;
  std::vector<std::pair<std::string, std::string> > video_resolutions;
  std::string current_resolution;

  for (mode = video_res.begin(); mode != video_res.end(); ++mode)
    {
      std::ostringstream ss;
      std::string text;
      ss << mode->GetX() << "x" << mode->GetY() ;
      text = ss.str();
      if (app->video->window.GetWidth() == mode->GetX() && app->video->window.GetHeight() == mode->GetY())
	current_resolution = text;

      video_resolutions.push_back (std::pair<std::string, std::string>(text, text));
  }
  cbox_video_mode = new ComboBox(_("Resolution"), "menu/resolution", stdSize,
				 video_resolutions, current_resolution);
  bottom_graphic_options->AddWidget(cbox_video_mode);

  top_n_bottom_graphic_options->AddWidget(top_graphic_options);
  top_n_bottom_graphic_options->AddWidget(bottom_graphic_options);
  graphic_options->AddWidget(top_n_bottom_graphic_options);

  widgets.AddWidget(graphic_options);

  /* Language selection */
  Box * language_options = new HBox(GRAPHIC_H);
  language_options->AddWidget(new PictureWidget(Point2i(40, 136), "menu/config_label"));
  widgets.AddWidget(language_options);
  lbox_languages = new ListBoxWithLabel(_("Language"), stdSize);
  language_options->AddWidget(lbox_languages);

  /* Sound options */
  Box * sound_options = new HBox(SOUND_H);
  sound_options->AddWidget(new PictureWidget(Point2i(40, 138), "menu/audio_label"));

  Box * all_sound_options = new HBox(SOUND_H-20,false);
  all_sound_options->SetMargin(25);
  all_sound_options->SetBorder(Point2i(10,10));

  opt_music = new PictureTextCBox(_("Music?"), "menu/music_enable", stdSize);
  all_sound_options->AddWidget(opt_music);

  opt_sound_effects = new PictureTextCBox(_("Sound effects?"), "menu/sound_effects_enable", stdSize);
  all_sound_options->AddWidget(opt_sound_effects);


  // Generate sound mode list
  uint current_freq = jukebox.GetFrequency();
  std::vector<std::pair<std::string, std::string> > sound_freqs;
  std::string current_sound_freq;
  sound_freqs.push_back (std::pair<std::string, std::string> ("11025", "11 kHz"));
  sound_freqs.push_back (std::pair<std::string, std::string> ("22050", "22 kHz"));
  sound_freqs.push_back (std::pair<std::string, std::string> ("44100", "44 kHz"));

  if (current_freq == 44100)
    current_sound_freq = "44100";
  else if (current_freq == 22050)
    current_sound_freq = "22050";
  else
    current_sound_freq = "11025";

  cbox_sound_freq = new ComboBox(_("Sound frequency"), "menu/sound_frequency",
				 stdSize, sound_freqs, current_sound_freq);
  all_sound_options->AddWidget(cbox_sound_freq);

  sound_options->AddWidget(all_sound_options);
  widgets.AddWidget(sound_options);

  /* Center the widgets */
  uint center_x = app->video->window.GetWidth()/2;

  sound_options->SetXY(center_x - sound_options->GetSizeX()/2, SOUND_Y);

  language_options->SetXY(center_x - (graphic_options->GetSizeX() + language_options->GetSizeX() + 20)/2,
			  sound_options->GetPositionY() + sound_options->GetSizeY() + 10);

  graphic_options->SetXY(language_options->GetPositionX() + language_options->GetSizeX() + 10, 
			 language_options->GetPositionY());
  
  // Values initialization

  resource_manager.UnLoadXMLProfile(res);

  Config * config = Config::GetInstance();

  opt_max_fps->SetValue(app->video->GetMaxFps());
  opt_display_wind_particles->SetValue(config->GetDisplayWindParticles());
  opt_display_energy->SetValue(config->GetDisplayEnergyCharacter());
  opt_display_name->SetValue(config->GetDisplayNameCharacter());
  full_screen->SetValue(app->video->IsFullScreen());

  // Setting language selection
  lbox_languages->AddItem(config->GetLanguage() == "",    _("(system language)"),  "");
  lbox_languages->AddItem(config->GetLanguage() == "bg",    "Български",           "bg");
  lbox_languages->AddItem(config->GetLanguage() == "bs",    "Bosanski",            "bs");
  lbox_languages->AddItem(config->GetLanguage() == "ca",    "Català",              "ca");
  lbox_languages->AddItem(config->GetLanguage() == "cpf",   "Créole",              "cpf");
  lbox_languages->AddItem(config->GetLanguage() == "da",    "Dansk",               "da");
  lbox_languages->AddItem(config->GetLanguage() == "de",    "Deutsch",             "de");
  lbox_languages->AddItem(config->GetLanguage() == "eo",    "Esperanto",           "eo");
  lbox_languages->AddItem(config->GetLanguage() == "en",    "English",             "en");
  lbox_languages->AddItem(config->GetLanguage() == "es",    "Castellano",          "es");
  lbox_languages->AddItem(config->GetLanguage() == "fi",    "Suomi",               "fi");
  lbox_languages->AddItem(config->GetLanguage() == "fr",    "Français",            "fr");
  lbox_languages->AddItem(config->GetLanguage() == "gl",    "Galego",              "gl");
  lbox_languages->AddItem(config->GetLanguage() == "hu",    "Magyar",              "hu");
  lbox_languages->AddItem(config->GetLanguage() == "it",    "Italiano",            "it");
  lbox_languages->AddItem(config->GetLanguage() == "lv",    "latviešu valoda",     "lv");
  lbox_languages->AddItem(config->GetLanguage() == "nb",    "Norsk",               "nb");
  lbox_languages->AddItem(config->GetLanguage() == "nl",    "Nederlands",          "nl");
  lbox_languages->AddItem(config->GetLanguage() == "pl",    "Polski",              "pl");
  lbox_languages->AddItem(config->GetLanguage() == "pt",    "Português",           "pt");
  lbox_languages->AddItem(config->GetLanguage() == "pt_BR", "Português do Brasil", "pt_BR");
  lbox_languages->AddItem(config->GetLanguage() == "ro",    "Română",              "ro");
  lbox_languages->AddItem(config->GetLanguage() == "ru",    "Pусский язык",        "ru");
  lbox_languages->AddItem(config->GetLanguage() == "sk",    "Slovenčina",          "sk");
  lbox_languages->AddItem(config->GetLanguage() == "sl",    "Slovenščina",         "sl");
  lbox_languages->AddItem(config->GetLanguage() == "sv",    "Svenska",             "sv");
  lbox_languages->AddItem(config->GetLanguage() == "tr",    "Türkçe",              "tr");
  lbox_languages->AddItem(config->GetLanguage() == "zh_CN", "汉语 (hànyǔ)",        "zh_CN");
  lbox_languages->AddItem(config->GetLanguage() == "zh_TW", "闽语 (mǐnyǔ)",        "zh_TW");

  opt_music->SetValue(jukebox.UseMusic());
  opt_sound_effects->SetValue(jukebox.UseEffects());
}

OptionMenu::~OptionMenu()
{
}

void OptionMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

void OptionMenu::OnClick(const Point2i &/*mousePosition*/, int /*button*/)
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
  config->SetSoundFrequency(cbox_sound_freq->GetIntValue());

  AppWormux * app = AppWormux::GetInstance();
  app->video->SetMaxFps(opt_max_fps->GetValue());
  // Video mode
  std::string s_mode = cbox_video_mode->GetValue();

  int w, h;
  sscanf(s_mode.c_str(),"%dx%d", &w, &h);
  app->video->SetConfig(w, h, full_screen->GetValue());

  uint x = app->video->window.GetWidth() / 2;
  uint y = app->video->window.GetHeight() - 50;

  SetActionButtonsXY(x, y);

  // Language
  std::string s_language = lbox_languages->ReadValue();
  config->SetLanguage(s_language);

  // Sound
  jukebox.ActiveMusic(opt_music->GetValue());
  jukebox.ActiveEffects(opt_sound_effects->GetValue());
  std::string sfreq = cbox_sound_freq->GetValue();
  long freq;
  if (str2long(sfreq,freq)) jukebox.SetFrequency(freq);

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

void OptionMenu::Draw(const Point2i &/*mousePosition*/)
{
}

