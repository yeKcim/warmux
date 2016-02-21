/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Classe principale qui gêre le jeu : initialisation, dessin, gestion
 * des différents composants, et boucle de jeu.
 *****************************************************************************/

#include "game.h"
#include <iostream>
#include <SDL.h>
#include <sstream>
#include "time.h"
#include "game_loop.h"
#include "game_mode.h"
#include "../graphic/video.h"
#include "../graphic/fps.h"
#include "../include/app.h"
#include "../interface/cursor.h"
#include "../interface/keyboard.h"
#include "../interface/game_msg.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../menu/results_menu.h"
#include "../object/objects_list.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../team/results.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"
#include "../weapon/weapons_list.h"

Game * Game::singleton = NULL;

Game * Game::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Game();
  }
  return singleton;
}

Game::Game()
{
  isGameLaunched = false;
  endOfGameStatus = false;
}

bool Game::IsGameFinished()
{
  return (NbrRemainingTeams() <= 1);
}

int Game::NbrRemainingTeams()
{
  uint nbr = 0;

  FOR_EACH_TEAM(team){
    if( (**team).NbAliveCharacter() > 0 )
      nbr++;
  }

  return nbr;
}

void Game::MessageLoading()
{
  std::cout << std::endl;
  std::cout << "[ " << _("Starting a new game") << " ]" << std::endl;
}

void Game::MessageEndOfGame()
{
  std::vector<TeamResults*>* results_list = TeamResults::createAllResults();
  const char *winner_name = NULL;

  FOR_EACH_TEAM(team)
  {
    // Determine winner
    if (0 < (**team).NbAliveCharacter())
    {
      winner_name = (**team).GetName().c_str();
      break;
    }
  }

  // Print out results
  if (winner_name)
    jukebox.Play("share","victory");

  //question.Set (txt, true, 0);
  //AskQuestion();
  Mouse::GetInstance()->SetPointer(POINTER_STANDARD);
  ResultsMenu menu(results_list, winner_name);
  menu.Run();

  TeamResults::deleteAllResults(results_list);
}

int Game::AskQuestion (Question &question, bool draw)
{
  Time * global_time = Time::GetInstance();
  global_time->Pause();

  if (draw)
    GameLoop::GetInstance()->Draw ();

  int answer = question.Ask ();

  global_time->Continue();
  return answer;
}

void Game::Start()
{
  bool err=true;
  bool end;
  std::string err_msg;

  try
  {
    GameLoop::GetInstance()->Init ();

    do
    {
      isGameLaunched = true;
      GameLoop::GetInstance()->fps.Reset();

      GameLoop::GetInstance()->Run();

      MSG_DEBUG( "game", "End of game_loop.Run()" );
      isGameLaunched = false;

      if (!IsGameFinished())
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
        end = (AskQuestion(question) == 1);
        jukebox.Resume();
        if(!end)
          world.ToRedrawOnScreen(Rectanglei(Point2i(0,0),AppWormux::GetInstance()->video.window.GetSize()));
      } else {
        end = true;
      }
    } while (!end);
    err = false;
  }
  catch (const std::exception &e)
  {
    err_msg = e.what();
  }

  if (!err)
    if (IsGameFinished())
      MessageEndOfGame();

  UnloadDatas();
  Mouse::GetInstance()->SetPointer(POINTER_STANDARD);

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

void Game::Pause()
{
  Question question;
  if(!network.IsLocal())
    return;

  jukebox.Pause();

  //Pause screen
  question.Set ("", true, 0, "interface/pause_screen");
  AskQuestion(question, false);
  jukebox.Resume();
}

bool Game::IsGameLaunched() const{
  return isGameLaunched;
}

bool Game::GetEndOfGameStatus(){
  return endOfGameStatus;
}

void Game::SetEndOfGameStatus(bool status){
  endOfGameStatus = status;
}
