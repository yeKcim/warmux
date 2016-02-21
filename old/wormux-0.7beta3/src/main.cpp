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
#include <SDL_image.h>
#include "game/config.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "menu/game_menu.h"
#include "menu/infos_menu.h"
#include "menu/main_menu.h"
#include "menu/options_menu.h"
#include "include/action_handler.h"
#include "include/constant.h"
#include "include/global.h"
#include "sound/jukebox.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/random.h"
#include "tool/stats.h"

using namespace Wormux;
AppWormux app;

AppWormux::AppWormux()
{
}

int AppWormux::main (int argc, char **argv)
{
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
  catch (const std::exception &e)
  {
    std::cerr << std::endl
	      << _("C++ exception caught:") << std::endl
	      << e.what() << std::endl
	      << std::endl;
  }
  catch (...)
  {
    std::cerr << std::endl
	      << _("Unexcepted exception caught...") << std::endl
	      << std::endl;
  }

  return 0;
}

void AppWormux::Init(int argc, char **argv)
{
  InitConstants();
  InitI18N();
  DisplayWelcomeMessage();
  InitDebugModes(argc, argv);

  InitRandom();
  action_handler.Init();
  config.Charge();

  InitNetwork(argc, argv);
  InitScreen();
  InitWindow();
  InitFonts();

  DisplayLoadingPicture();
  config.Applique();

  jukebox.Init();
}

void AppWormux::InitNetwork(int argc, char **argv)
{
#ifdef TODO_NETWORK 
  if ((argc == 3) && (strcmp(argv[1],"server")==0)) {
	// wormux server <port>
	network.server_start (argv[2]);
  } else if (argc == 3) {
	// wormux <server_ip> <server_port>
	network.client_connect(argv[1], argv[2]);
  }
#endif
}

void AppWormux::InitScreen()
{
  if ( SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 )
    Error( Format( _("Unable to initialize SDL library: %s"), SDL_GetError() ) );
}

void AppWormux::InitWindow()
{
  std::string txt_version = std::string("Wormux ") + VERSION;
  std::string icon = config.data_dir + "wormux-32.xpm";

  // Open a new window
  app.sdlwindow = NULL;
  video.SetConfig(config.tmp.video.width,
		  config.tmp.video.height,
		  config.tmp.video.fullscreen);

  // Set window caption
  SDL_WM_SetCaption(txt_version.c_str(), NULL);
  SDL_WM_SetIcon(IMG_Load(icon.c_str()), NULL);
}

void AppWormux::DisplayLoadingPicture()
{
  std::string txt_version = _("Version") + std::string(" ") + VERSION;

  // TODO->use ressource handler
  SDL_Surface* loading_image=IMG_Load( (config.data_dir+"menu/img/loading.png").c_str());

  Wormux::global_time.Reset();

  SDL_BlitSurface(loading_image,NULL,app.sdlwindow,NULL);

  Text text1(_("Wormux launching..."), white_color, &global().huge_font(), true); 
  Text text2(txt_version, white_color, &global().huge_font(), true); 
  int x = video.GetWidth()/2;
  int y = video.GetHeight()/2 - 200;
  text1.DrawCenter (x, y);
  y += global().huge_font().GetHeight() + 20;
  text2.DrawCenter (x, y);

  SDL_UpdateRect(app.sdlwindow, 0, 0, 0, 0);
  SDL_Flip(app.sdlwindow);
  SDL_FreeSurface(loading_image);
}

void AppWormux::InitFonts()
{
  if (TTF_Init()==-1)
    Error( Format( _("Initialisation of TTF library failed: %s"), TTF_GetError() ) );
  if (!Font::InitAllFonts())
    Error( _("Unable to initialise the fonts.") );
  createGlobal();
}

void AppWormux::End()
{
  std::cout << std::endl
	    << "[ " << _("Quit Wormux") << " ]" << std::endl;

  config.Sauve();
  destroyGlobal();
  jukebox.End();
  TTF_Quit();
  SDL_Quit();

#ifdef ENABLE_STATS
  SaveStatToXML("stats.xml");
#endif  
  std::cout << "o "
            << _("Please tell us your opinion of Wormux via email:") << " " << EMAIL
            << std::endl;
}

void AppWormux::DisplayWelcomeMessage()
{
  std::cout << "=== " << _("Wormux version ") << VERSION << std::endl;
  std::cout << "=== " << _("Authors:") << ' ';
  for (std::vector<std::string>::iterator it=AUTHORS.begin(),
	 fin=AUTHORS.end();
       it != fin;
       ++it)
  {
    if (it != AUTHORS.begin()) std::cout << ", ";
    std::cout << *it;
  }
  std::cout << std::endl
	    << "=== " << _("Website: ") << WEB_SITE << std::endl
	    << std::endl;

  // Affiche l'absence de garantie sur le jeu
  std::cout << "Wormux version " << VERSION
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
  app.main(argc,argv);
  return EXIT_SUCCESS;
}
