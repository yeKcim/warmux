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
 *  Starting file. (the 'main' function is here.)
 *****************************************************************************/

#include "include/app.h"
#include <algorithm>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <SDL.h>
#include "game/config.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "menu/game_menu.h"
#include "menu/infos_menu.h"
#include "menu/main_menu.h"
#include "menu/options_menu.h"
#include "network/network.h"
#include "include/action_handler.h"
#include "include/constant.h"
#include "sound/jukebox.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/random.h"
#include "tool/stats.h"

AppWormux * AppWormux::singleton = NULL;

AppWormux * AppWormux::GetInstance() {
  if (singleton == NULL) {
    singleton = new AppWormux();
  }
  return singleton;
}

AppWormux::AppWormux(){
}

int AppWormux::main (int argc, char **argv){
  bool quit = false;

  try {
    Init(argc, argv);
    do {
      Main_Menu main_menu;
      menu_item choix;

      StatStart("Main:Menu");
      choix = main_menu.Run();
      StatStop("Main:Menu");

      switch (choix)
        {
        case menuPLAY:
	  {
	    GameMenu game_menu;
	    game_menu.Run();
	    break;
	  }
        case menuOPTIONS:
          {
            OptionMenu options_menu;
            options_menu.Run();
            break;
          }
        case menuQUIT:
          quit = true; 
        default:
          break;
        }
    } while (!quit);

    End();
  }
  catch (const std::exception &e){
    std::cerr << std::endl
	      << _("C++ exception caught:") << std::endl
	      << e.what() << std::endl
	      << std::endl;
    WakeUpDebugger();
  }
  catch (...){
    std::cerr << std::endl
	      << _("Unexcepted exception caught...") << std::endl
	      << std::endl;
    WakeUpDebugger();
  }

  return 0;
}

void AppWormux::Init(int argc, char **argv){
  Config * config = Config::GetInstance();
  config->Init();

  InitI18N();
  DisplayWelcomeMessage();
  InitDebugModes(argc, argv);

  ActionHandler::GetInstance()->Init();
  config->Load();

  InitNetwork(argc, argv);
  video.InitWindow();
  InitFonts();

  DisplayLoadingPicture();
  config->Apply();

  jukebox.Init();
}

void AppWormux::InitNetwork(int argc, char **argv){
  if (argc >= 3 && strcmp(argv[1],"server")==0) {
	// wormux server <port>
	network.Init();
	network.server_start (argv[2]);
  } else if (argc >= 3 && strcmp(argv[1], "--add-debug-mode") != 0) {
	// wormux <server_ip> <server_port>
	network.Init();
	network.client_connect(argv[1], argv[2]);
  }
}

void AppWormux::DisplayLoadingPicture(){
  Config * config = Config::GetInstance();

  std::string txt_version = _("Version") + std::string(" ") + Constants::VERSION;
  std::string filename = config->GetDataDir() 
    + PATH_SEPARATOR + "menu"
    + PATH_SEPARATOR + "img"
    + PATH_SEPARATOR + "loading.png";

  Surface surfaceLoading = Surface( filename.c_str() );
  Sprite loading_image = Sprite( surfaceLoading );

  loading_image.cache.EnableLastFrameCache();
  loading_image.ScaleSize( video.window.GetSize() );
  loading_image.Blit( video.window, 0, 0 );

  Time::GetInstance()->Reset();

  Text text1(_("Wormux launching..."), white_color, Font::GetInstance(Font::FONT_HUGE), true); 
  Text text2(txt_version, white_color, Font::GetInstance(Font::FONT_HUGE), true); 
  
  Point2i windowCenter = video.window.GetSize() / 2;
  
  text1.DrawCenter( windowCenter );
  text2.DrawCenter( windowCenter + Point2i(0, (*Font::GetInstance(Font::FONT_HUGE)).GetHeight() + 20 ));

  video.window.Flip();
}

void AppWormux::InitFonts(){
  if( TTF_Init() == -1 )
    Error( Format( _("Initialisation of TTF library failed: %s"), TTF_GetError() ) );
}

void AppWormux::End(){
  std::cout << std::endl
	    << "[ " << _("Quit Wormux") << " ]" << std::endl;

  Config::GetInstance()->Save();
  jukebox.End();
  delete Config::GetInstance();
  delete Time::GetInstance();
  delete Constants::GetInstance();
  TTF_Quit();

#ifdef ENABLE_STATS
  SaveStatToXML("stats.xml");
#endif  
  std::cout << "o "
            << _("Please tell us your opinion of Wormux via email:") << " " << Constants::EMAIL
            << std::endl;
}

void AppWormux::DisplayWelcomeMessage(){
  std::cout << "=== " << _("Wormux version ") << Constants::VERSION << std::endl;
  std::cout << "=== " << _("Authors:") << ' ';
  for( std::vector<std::string>::iterator it=Constants::AUTHORS.begin(),
       fin=Constants::AUTHORS.end();
       it != fin;
       ++it)
  {
    if( it != Constants::AUTHORS.begin() )
      std::cout << ", ";
    std::cout << *it;
  }
  std::cout << std::endl
	    << "=== " << _("Website: ") << Constants::WEB_SITE << std::endl
	    << std::endl;

  // Affiche l'absence de garantie sur le jeu
  std::cout << "Wormux version " << Constants::VERSION
	    << ", Copyright (C) 2001-2004 Lawrence Azzoug"
	    << std::endl
	    << "Wormux comes with ABSOLUTELY NO WARRANTY." << std::endl
            << "This is free software, and you are welcome to redistribute it" << std::endl
            << "under certain conditions." << std::endl
	    << std::endl
	    << "Read COPYING file for details." << std::endl
	    << std::endl;

#ifdef DEBUG
  std::cout << "!!! This program was compiled in DEBUG mode (development version) !!!" << std::endl
	    << std::endl;
#endif

  std::cout << "[ " << _("Run game") << " ]" << std::endl;
}

int main (int argc, char **argv)
{
  AppWormux::GetInstance()->main(argc,argv);
  delete AppWormux::GetInstance();
  exit (EXIT_SUCCESS);
}
