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
 * Options menu
 *****************************************************************************/

#include "options_menu.h"
//-----------------------------------------------------------------------------

#include "../include/app.h"
#include "../graphic/video.h"
#include "../team/teams_list.h"
#include "../game/game_mode.h"
#include "../map/maps_list.h"
#include "../game/config.h"
#include "../tool/i18n.h"
#include "../tool/string_tools.h"
#include "../include/global.h"

using namespace Wormux;
using namespace std;

//-----------------------------------------------------------------------------

const uint GAME_X = 20;
const uint GAME_Y = 20;
const uint GAME_W = 230;

const uint SOUND_X = GAME_X+GAME_W+30;
const uint SOUND_Y = GAME_Y;
const uint SOUND_W = 230;

const uint GRAPHIC_X = SOUND_X+SOUND_W+30;
const uint GRAPHIC_Y = GAME_Y;
const uint GRAPHIC_W = 230;

const uint NBR_VER_MIN = 1;
const uint NBR_VER_MAX = 10;
const uint TPS_TOUR_MIN = 10;
const uint TPS_TOUR_MAX = 120;
const uint TPS_FIN_TOUR_MIN = 1;
const uint TPS_FIN_TOUR_MAX = 10;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

OptionMenu::OptionMenu() : Menu("menu/bg_option")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  //-----------------------------------------------------------------------------
  // Widget creation
  //-----------------------------------------------------------------------------

  /* Grapic options */
  graphic_options = new VBox(GRAPHIC_X, GRAPHIC_Y, GRAPHIC_W);
  graphic_options->AddWidget(new Label(_("Graphic options"), 0, 0, 0, global().normal_font()));

  lbox_video_mode = new ListBox(0, 0, 0, 80);
  graphic_options->AddWidget(lbox_video_mode);

  opt_max_fps = new SpinButton(_("Maximum number of FPS:"), 0, 0, 0,
			       50, 5,
			       20, 120);
  graphic_options->AddWidget(opt_max_fps);

  full_screen = new CheckBox(_("Fullscreen?"), 0, 0, 0); 
  graphic_options->AddWidget(full_screen);

  opt_display_wind_particles = new CheckBox(_("Display wind particles?"), 0, 0, 0); 
  graphic_options->AddWidget(opt_display_wind_particles);

  opt_display_energy = new CheckBox(_("Display player energy?"), 0, 0, 0); 
  graphic_options->AddWidget(opt_display_energy);

  opt_display_name = new CheckBox(_("Display player's name?"), 0, 0, 0); 
  graphic_options->AddWidget(opt_display_name);

  /* Sound options */
  sound_options = new VBox(SOUND_X, SOUND_Y, SOUND_W);
  sound_options->AddWidget(new Label(_("Sound options"), 0, 0, 0, global().normal_font()));

  lbox_sound_freq = new ListBox(0, 0, 0, 80);
  sound_options->AddWidget(lbox_sound_freq);

  opt_music = new CheckBox(_("Music?"), 0, 0, 0);
  sound_options->AddWidget(opt_music);

  opt_sound_effects = new CheckBox(_("Sound effects?"), 0, 0, 0);
  sound_options->AddWidget(opt_sound_effects);
  
  /* Game options */
  game_options = new VBox(GAME_X, GAME_Y, GAME_W);
  game_options->AddWidget(new Label(_("Game options"), 0, 0, 0, global().normal_font()));

  opt_duration_turn = new SpinButton(_("Duration of a turn:"), 0, 0, 0,
				     TPS_TOUR_MIN, 5,
				     TPS_TOUR_MIN, TPS_TOUR_MAX);
  game_options->AddWidget(opt_duration_turn);

  opt_duration_end_turn = new SpinButton(_("Duration of the end of a turn:"), 0, 0, 0,
					 TPS_FIN_TOUR_MIN, 1,
					 TPS_FIN_TOUR_MIN, TPS_FIN_TOUR_MAX);
  game_options->AddWidget(opt_duration_end_turn);

  opt_nb_characters = new SpinButton(_("Number of players per team:"), 0, 0, 0,
				 4, 1,
				 NBR_VER_MIN, NBR_VER_MAX);
  game_options->AddWidget(opt_nb_characters);

  opt_energy_ini = new SpinButton(_("Initial energy:"), 0,0,0,
				      100, 5,
				      50, 200);
  
  game_options->AddWidget(opt_energy_ini);


  //-----------------------------------------------------------------------------
  // Values initialization
  //-----------------------------------------------------------------------------

  //Generate video mode list
  SDL_Rect **modes;

  /* Get available fullscreen/hardware modes */
  modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

  /* Check is there are any modes available */
  if(modes == (SDL_Rect **)0){
    std::ostringstream ss;
    ss << app.sdlwindow->w << "x" << app.sdlwindow->h ;
    lbox_video_mode->AddItem(false,"No modes available!", ss.str());
  } else {
    for(int i=0;modes[i];++i) {
      if (modes[i]->w < 800 || modes[i]->h < 600) break; 
      std::ostringstream ss;
      ss << modes[i]->w << "x" << modes[i]->h ;
      if (modes[i]->w == app.sdlwindow->w && modes[i]->h == app.sdlwindow->h)
	lbox_video_mode->AddItem(true, ss.str(), ss.str());
      else
	lbox_video_mode->AddItem(false, ss.str(), ss.str());
    }
  }

  // Generate sound mode list
  uint current_freq = jukebox.GetFrequency();
  lbox_sound_freq->AddItem (current_freq == 11025, "11 kHz", "11025");
  lbox_sound_freq->AddItem (current_freq == 22050, "22 kHz", "22050");
  lbox_sound_freq->AddItem (current_freq == 44100, "44 kHz", "44100");

  resource_manager.UnLoadXMLProfile( res);

  opt_max_fps->SetValue (video.GetMaxFps());
  opt_display_wind_particles->SetValue (config.display_wind_particles);
  opt_display_energy->SetValue (config.display_energy_character);
  opt_display_name->SetValue (config.display_name_character);
  full_screen->SetValue (video.IsFullScreen());
  opt_duration_turn->SetValue(game_mode.duration_turn);
  opt_duration_end_turn->SetValue(game_mode.duration_move_player);
  opt_nb_characters->SetValue(game_mode.max_characters);
  opt_energy_ini->SetValue(game_mode.character.init_energy);

  opt_music->SetValue( jukebox.UseMusic() );
  opt_sound_effects->SetValue( jukebox.UseEffects() );

}

//-----------------------------------------------------------------------------

OptionMenu::~OptionMenu()
{
  delete graphic_options;
  delete sound_options;
  delete game_options;
}

//-----------------------------------------------------------------------------

void OptionMenu::OnClic ( int x, int y, int button )
{     
  if (graphic_options->Clic (x,y,button)) {
  } else if (sound_options->Clic (x,y,button)) {
  } else if (game_options->Clic (x,y,button)) {
  }
}

//-----------------------------------------------------------------------------

void OptionMenu::SaveOptions()
{
  // Save values
  config.display_wind_particles = opt_display_wind_particles->GetValue();
  config.display_energy_character = opt_display_energy->GetValue();
  config.display_name_character = opt_display_name->GetValue();

  game_mode.duration_turn = opt_duration_turn->GetValue() ;
  game_mode.duration_move_player = opt_duration_end_turn->GetValue() ;
  game_mode.max_characters = opt_nb_characters->GetValue() ;

  game_mode.character.init_energy = opt_energy_ini->GetValue() ;

  video.SetMaxFps(opt_max_fps->GetValue());
  // Video mode
  std::string s_mode = lbox_video_mode->ReadValue();
  int w, h;
  sscanf(s_mode.c_str(),"%dx%d", &w, &h);
  video.SetConfig(w, h, full_screen->GetValue());
  
  uint x = (video.GetWidth()/2);
  uint y = video.GetHeight()-50;

  SetActionButtonsXY(x, y);
   
  // Sound
  jukebox.ActiveMusic( opt_music->GetValue() );
  jukebox.ActiveEffects( opt_sound_effects->GetValue() );
  std::string sfreq = lbox_sound_freq->ReadValue();
  long freq;
  if (str2long(sfreq,freq)) jukebox.SetFrequency (freq);
  
  jukebox.Init(); // commit modification on sound options
   
  //Save options in XML
  config.Sauve();
}

//-----------------------------------------------------------------------------

void OptionMenu::__sig_ok()
{
  SaveOptions();
}

//-----------------------------------------------------------------------------

void OptionMenu::__sig_cancel()
{
  // Nothing to do
}

//-----------------------------------------------------------------------------

void OptionMenu::Draw(int mouse_x, int mouse_y)
{   
  graphic_options->Draw(mouse_x, mouse_y);
  sound_options->Draw(mouse_x,mouse_y);
  game_options->Draw(mouse_x,mouse_y);
}

//-----------------------------------------------------------------------------
