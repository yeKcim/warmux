/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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

#include <algorithm>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <getopt.h>
#ifndef WIN32
#include <signal.h>
#endif
#include <SDL.h>
#include <WORMUX_debug.h>
#include <WORMUX_index_server.h>
#include <WORMUX_random.h>
#include <WORMUX_singleton.h>
#include "game/config.h"
#include "game/game.h"
#include "game/time.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/base.h"
#include "include/constant.h"
#include "map/map.h"
#include "menu/credits_menu.h"
#include "menu/game_menu.h"
#include "menu/help_menu.h"
#include "menu/main_menu.h"
#include "menu/network_connection_menu.h"
#include "menu/options_menu.h"
#include "menu/skin_menu.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#ifdef DEBUG
#include "include/debugmasks.h"
#endif

static MainMenu::menu_item choice = MainMenu::NONE;
static bool skip_menu = false;
static const char* skin = NULL;
static NetworkConnectionMenu::network_menu_action_t net_action = NetworkConnectionMenu::NET_NOTHING;

AppWormux *AppWormux::singleton = NULL;

AppWormux *AppWormux::GetInstance()
{
  if (singleton == NULL)
    {
      singleton = new AppWormux();
    }
  return singleton;
}

AppWormux::AppWormux():
  video(new Video()),
  menu(NULL)
{
  JukeBox::GetInstance()->Init();
  RandomLocal().InitRandom();
  std::cout << "[ " << _("Run game") << " ]" << std::endl;
}

AppWormux::~AppWormux()
{
  delete video;
  ParticleEngine::FreeMem();
  Font::ReleaseInstances();
  BaseSingleton::ReleaseSingletons();
  singleton = NULL;
}

int AppWormux::Main(void)
{
  bool quit = false;

#ifndef DEBUG
  try
#endif
  {
    DisplayLoadingPicture();

    OptionMenu::CheckUpdates();

    Action_Handler_Init();

    do
    {

      if (choice == MainMenu::NONE)
      {
        MainMenu main_menu;
        SetCurrentMenu(&main_menu);
        choice = main_menu.Run();
      }

      ActionHandler::GetInstance()->Flush();

      switch (choice)
      {
        case MainMenu::PLAY:
        {
          GameMenu game_menu;
          SetCurrentMenu(&game_menu);
          game_menu.Run(skip_menu);
          break;
        }
        case MainMenu::NETWORK:
        {
          NetworkConnectionMenu network_connection_menu(net_action);
          SetCurrentMenu(&network_connection_menu);
          network_connection_menu.Run(skip_menu);
          break;
        }
        case MainMenu::HELP:
        {
          HelpMenu help_menu;
          SetCurrentMenu(&help_menu);
          help_menu.Run();
          break;
        }
        case MainMenu::OPTIONS:
        {
          OptionMenu options_menu;
          SetCurrentMenu(&options_menu);
          options_menu.Run();
          break;
        }
        case MainMenu::CREDITS:
        {
          CreditsMenu credits_menu;
          SetCurrentMenu(&credits_menu);
          credits_menu.Run();
          break;
        }
        case MainMenu::QUIT:
          quit = true;
          break;
        case MainMenu::SKIN_VIEWER:
        {
          SkinMenu skin_menu(skin);
          SetCurrentMenu(&skin_menu);
          skin_menu.Run();
          break;
        }
        default:
          break;
      }
      SetCurrentMenu(NULL);
      choice = MainMenu::NONE;
      skip_menu = false;
      net_action = NetworkConnectionMenu::NET_NOTHING;
    }
    while (!quit);

    End();
  }
#ifndef DEBUG
  catch(const std::exception & e)
  {
    std::cerr << std::endl
	 << "C++ exception caught:" << std::endl
	 << e.what() << std::endl << std::endl;
    AppWormux::DisplayError(e.what());
    WakeUpDebugger();
  }
  catch(...)
  {
    std::cerr << std::endl
      << "Unexpected exception caught..." << std::endl << std::endl;
    WakeUpDebugger();
  }
#endif

  return 0;
}

void AppWormux::DisplayLoadingPicture()
{
  Config *config = Config::GetInstance();

  std::string txt_version =
    _("Version") + std::string(" ") + Constants::WORMUX_VERSION;
  std::string filename = config->GetDataDir() + "menu" PATH_SEPARATOR "loading.png";

  Surface surfaceLoading(filename.c_str());
  Sprite loading_image(surfaceLoading, true);

  loading_image.cache.EnableLastFrameCache();
  loading_image.ScaleSize(video->window.GetSize());
  loading_image.Blit(video->window, 0, 0);

  Time::GetInstance()->Reset();

  Text text1(_("Wormux launching..."), white_color,
             Font::FONT_HUGE, Font::FONT_NORMAL, true);
  Text text2(txt_version, white_color, Font::FONT_HUGE, Font::FONT_NORMAL,
             true);

  Point2i windowCenter = video->window.GetSize() / 2;

  text1.DrawCenter(windowCenter);
  text2.DrawCenter(windowCenter
                   + Point2i(0, (*Font::GetInstance(Font::FONT_HUGE, Font::FONT_NORMAL)).GetHeight() + 20));

  video->window.Flip();
}

void AppWormux::SetCurrentMenu(Menu* _menu)
{
  menu = _menu;
}

Menu* AppWormux::GetCurrentMenu() const
{
  return menu;
}

void AppWormux::RefreshDisplay()
{
  if (Game::IsRunning()) {
    GetWorld().DrawSky(true);
    GetWorld().Draw(true);
  }
  else if (GetCurrentMenu()) {
    GetCurrentMenu()->RedrawMenu();
  }
}

void AppWormux::DisplayError(const std::string &msg)
{
  std::cerr << msg << std::endl;

  if (Game::IsRunning()) {
    // nothing to do
  } else if (singleton->GetCurrentMenu()) {
    singleton->GetCurrentMenu()->DisplayError(msg);
  }
}

void AppWormux::ReceiveMsgCallback(const std::string& msg)
{
  if (Game::IsRunning()) {
    //Add message to chat session in Game
    Game::GetInstance()->chatsession.NewMessage(msg);
  } else if (GetCurrentMenu()) {
    GetCurrentMenu()->ReceiveMsgCallback(msg);
  }
}

void AppWormux::End() const
{
  std::cout << std::endl << "[ " << _("Quit Wormux") << " ]" << std::endl;

  /* FIXME calling Config->Save here sucks: it nothing was ever done, it loads
   * the whole stuff just before exiting... This should be moved, but where? */
  Config::GetInstance()->Save();

  JukeBox::GetInstance()->End();

#ifdef ENABLE_STATS
  SaveStatToXML("stats.xml");
#endif
  std::cout << "o " << _("If you found a bug or have a feature request "
                    "send us an email (in english, please):")
    << " " << Constants::EMAIL << std::endl;
}

void AppWormux::EmergencyExit()
{
  delete AppWormux::GetInstance();
  exit(EXIT_SUCCESS);
}

void DisplayWelcomeMessage()
{
  std::cout << "=== " << _("Wormux version ") << Constants::WORMUX_VERSION << std::endl;
  std::cout << "=== " << _("Authors:") << ' ';
  for (std::vector < std::string >::iterator it = Constants::GetInstance()->AUTHORS.begin(),
       fin = Constants::GetInstance()->AUTHORS.end(); it != fin; ++it)
    {
      if (it != Constants::GetInstance()->AUTHORS.begin())
        std::cout << ", ";
      std::cout << *it;
    }
  std::cout << std::endl
    << "=== " << _("Website: ") << Constants::WEB_SITE << std::endl
    << std::endl;

  // print the disclaimer
  std::cout << "Wormux version " << Constants::WORMUX_VERSION
    << ", Copyright (C) 2001-2009 Wormux Team" << std::endl
    << "Wormux comes with ABSOLUTELY NO WARRANTY." << std::endl
    << "This is free software and you are welcome to redistribute it" << std::endl
    << "under certain conditions." << std::endl << std::endl
    << "Read the file COPYING for details." << std::endl << std::endl;

#ifdef DEBUG
  std::cout << "This program was compiled in DEBUG mode (development version)"
       << std::endl << std::endl;
#endif
}

void PrintUsage(const char* cmd_name)
{
  printf("usage: \n");
  printf("%s -h|--help : show this help\n", cmd_name);
  printf("%s -v|--version : show the version\n", cmd_name);
  printf("%s -r|--reset-config : reset the configuration to default\n", cmd_name);
  printf("%s -y|--skin-viewer [team] : start the skin viewer (for development only)\n", cmd_name);
  printf("%s [-p|--play] [-g|--game-mode <game_mode>]"
	 " [-s|--server] [-c|--client [ip]]\n"
	 " [-l [ip/hostname of index server]]\n"
#ifdef DEBUG
	 " [-d|--debug <debug_masks>|all]\n"
#endif
	 , cmd_name);
#ifdef DEBUG
  printf("\nWith :\n");
  printf(" <debug_masks> ::= { %s }\n", used_debug_masks.c_str());
#endif
}

void ParseArgs(int argc, char * argv[])
{
  int c;
  int option_index = 0;
  struct option long_options[] =
    {
      {"help",       no_argument,       NULL, 'h'},
      {"blitz",      no_argument,       NULL, 'b'},
      {"version",    no_argument,       NULL, 'v'},
      {"play",       no_argument,       NULL, 'p'},
      {"client",     optional_argument, NULL, 'c'},
      {"server",     no_argument,       NULL, 's'},
      {"skin-viewer",optional_argument, NULL, 'y'},
      {"game-mode",  required_argument, NULL, 'g'},
      {"debug",      required_argument, NULL, 'd'},
      {"reset-config", no_argument,     NULL, 'r'},
      {NULL,         no_argument,       NULL,  0 }
    };

  while ((c = getopt_long (argc, argv, "hbvpc::l::sy::g:d:",
                           long_options, &option_index)) != -1)
    {
      switch (c)
        {
        case 'h':
	  PrintUsage(argv[0]);
          exit(EXIT_SUCCESS);
          break;
        case 'v':
          DisplayWelcomeMessage();
          exit(EXIT_SUCCESS);
          break;
        case 'p':
          choice = MainMenu::PLAY;
          skip_menu = true;
          break;
        case 'c':
          choice = MainMenu::NETWORK;
          net_action = NetworkConnectionMenu::NET_CONNECT;
          if (optarg)
            {
              Config::GetInstance()->SetNetworkClientHost(optarg);
            }
          skip_menu = true;
          break;
        case 'd':
#ifdef DEBUG
          printf("Debug: %s\n", optarg);
          AddDebugMode(optarg);
#else
	  fprintf(stderr, "Option -d is not available. Wormux has not been compiled with debug option.\n");
#endif
          break;
        case 's':
          choice = MainMenu::NETWORK;
          net_action = NetworkConnectionMenu::NET_HOST;
          skip_menu = true;
          break;
        case 'l':
          if (optarg) IndexServer::GetInstance()->SetAddress(optarg);
          else        IndexServer::GetInstance()->SetAddress("127.0.0.1");
          break;
        case 'y':
          choice = MainMenu::SKIN_VIEWER;
          skin = optarg;
          skip_menu = true;
          break;
	case 'g':
	  printf("Game-mode: %s\n", optarg);
	  Config::GetInstance()->SetGameMode(optarg);
	  break;
	case 'r':
	  {
	    bool r;
	    r = Config::GetInstance()->RemovePersonalConfigFile();
	    if (!r)
	      exit(EXIT_FAILURE);
	    exit(EXIT_SUCCESS);
	  }
	  break;
	default:
	  fprintf(stderr, "Unknow option %c", c);
	  PrintUsage(argv[0]);
	  exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
#ifndef WIN32
  signal(SIGPIPE, SIG_IGN);
#endif
  /* FIXME calling Config::GetInstance here means that there is no need of
   * singleton for Config but simply a global variable. This may look stange
   * but the whole system (directories, translation etc...) is needed, even for
   * the ParseArgs and DisplayWelcomeMessage functions. */
  Config::GetInstance(); // init config first, because it initializes i18n

  ParseArgs(argc, argv);

  DisplayWelcomeMessage();

  AppWormux::GetInstance()->Main();
  delete AppWormux::GetInstance();
  exit(EXIT_SUCCESS);
}
