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
 * Init the game, handle drawing and states of the game.
 *****************************************************************************/

#include "game.h"
#include <iostream>
#include <SDL.h>
#include <sstream>
#include "time.h"
#include "game_init.h"
#include "game_loop.h"
#include "game_mode.h"
#include "graphic/video.h"
#include "graphic/fps.h"
#include "include/app.h"
#include "interface/cursor.h"
#include "interface/keyboard.h"
#include "interface/game_msg.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "menu/results_menu.h"
#include "network/network.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/results.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "weapon/weapons_list.h"

Game * Game::singleton = NULL;

Game * Game::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Game();
  }
  return singleton;
}


Game::Game():
  isGameLaunched(false),
  want_end_of_game(false)
{
}

bool Game::IsGameFinished() const
{
  return (NbrRemainingTeams() <= 1 || want_end_of_game );
}

int Game::NbrRemainingTeams() const
{
  uint nbr = 0;

  FOR_EACH_TEAM(team){
    if( (**team).NbAliveCharacter() > 0 )
      nbr++;
  }

  return nbr;
}

void Game::MessageLoading() const
{
  std::cout << std::endl;
  std::cout << "[ " << _("Starting a new game") << " ]" << std::endl;
}

void Game::MessageEndOfGame() const
{
  std::vector<TeamResults*>* results_list = TeamResults::createAllResults();

  Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
  ResultsMenu menu(results_list);
  menu.Run();

  TeamResults::deleteAllResults(results_list);
}

int Game::AskQuestion (Question &question, bool draw)
{
  Time::GetInstance()->Pause();

  if (draw)
    GameLoop::GetInstance()->Draw ();

  int answer = question.Ask ();

  Time::GetInstance()->Continue();
  return answer;
}

void Game::Start()
{
  bool err = true;
  bool end = false;
  std::string err_msg;
  want_end_of_game = false;

  try
  {
    jukebox.PlayMusic(ActiveMap().ReadMusicPlaylist());
    GameInit::Init ();

    do
    {
      isGameLaunched = true;
      GameLoop::GetInstance()->Run();

      MSG_DEBUG( "game", "End of game_loop.Run()" );
      isGameLaunched = false;

      if (Time::GetInstance()->IsGamePaused()){
        DisplayPause();
      } else {
        if (want_end_of_game)
          end = DisplayQuit();
        else
          end = true;
      }

      if (!end)
      {
        world.ToRedrawOnScreen(Rectanglei(Point2i(0,0),AppWormux::GetInstance()->video.window.GetSize()));
	Keyboard::GetInstance()->Reset();
	ActiveCharacter().StopPlaying();
	want_end_of_game = false;
      }

    } while (!end);
    err = false;
  }
  catch (const std::exception &e)
  {
    err_msg = e.what();
  }

  jukebox.StopAll();
  
  if (!err)
  // * When debug is disabled : only show the result menu if game
  // have 'regularly' finished (only one survivor or timeout reached)
  // * When debug is disabled : still show the result menu everytime the game
  // is quit during local games (so we can still the result menu often).
  // For network game only show the result if the game is regularly finished
  // (elsewise when someone if someone quit the game before the end, it appears
  // as disconnected only when if finnishes viewing the f*cking result menu)
#ifndef DEBUG
  if (IsGameFinished() && !want_end_of_game)
#else
  if (IsGameFinished() && (!want_end_of_game || Network::GetInstance()->IsLocal()))
#endif
      MessageEndOfGame();

  UnloadDatas();
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
  jukebox.PlayMusic("menu");

  if (err)
  {
    Question question;
    std::string txt = Format(_("Error:\n%s"), err_msg.c_str());
    std::cout << std::endl << txt << std::endl;
    question.Set (txt, true, 0);
    AskQuestion (question, false);
  }
}

void Game::UnloadDatas()
{
  world.FreeMem();
  lst_objects.FreeMem();
  ParticleEngine::Stop();
  teams_list.UnloadGamingData();
  jukebox.StopAll();
}

void Game::TogglePause()
{
  if(Time::GetInstance()->IsGamePaused())
    Time::GetInstance()->Continue();
  else
    Time::GetInstance()->Pause();
}

void Game::DisplayPause()
{
  Question question;
  if(!Network::GetInstance()->IsLocal())
    return;

  jukebox.Pause();

  //Pause screen
  question.Set ("", false, 0, "interface/pause_screen");
  question.add_choice(Keyboard::GetInstance()->GetKeyAssociatedToAction(Keyboard::KEY_PAUSE),
		      1
		      );
  AskQuestion(question, false);
  jukebox.Resume();
}

bool Game::DisplayQuit()
{
  Question question;
  const char *msg = _("Do you really want to quit? (Y/N)");
  question.Set (msg, true, 0, "interface/quit_screen");

  {
    /* Tiny fix by Zygmunt Krynicki <zyga@zyga.dyndns.org> */
    /* Let's find out what the user would like to press ... */
    char *key_x_ptr = strchr (msg, '/');
    char key_x;
    if (key_x_ptr && key_x_ptr > msg) /* it's there and it's not the first char */
      key_x = tolower(key_x_ptr[-1]);
    else
      abort();
    if (!isalpha(key_x)) /* sanity check */
      abort();

    question.add_choice(SDLK_a + (int)key_x - 'a', 1);
  }

  jukebox.Pause();
  bool exit = (AskQuestion(question) == 1);
  jukebox.Resume();

  return exit;
}

bool Game::IsGamePaused() const{
  return Time::GetInstance()->IsGamePaused();
}

bool Game::IsGameLaunched() const{
  return isGameLaunched;
}

