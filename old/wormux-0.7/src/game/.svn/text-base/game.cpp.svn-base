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
#include "../interface/cursor.h"
#include "../interface/keyboard.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
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
  std::cout << "Loading game... => Splashscreen is TODO" << std::endl;
}

void Game::MessageEndOfGame()
{
  const char *Nobody                    = _("Nobody");
  const char *winner_name               = NULL;
  int         value_all_most_violent    = 0;
  const char *name_all_most_violent     = Nobody;
  int         value_all_most_useless    = 0x0FFFFFFF;
  const char *name_all_most_useless     = Nobody;
  int         value_all_most_usefull    = 0;
  const char *name_all_most_usefull     = Nobody;
  int         value_all_most_traitor    = 0;
  const char *name_all_most_traitor     = Nobody;

  std::string txt("");

  FOR_EACH_TEAM(equipe)
  {
    int         value_team_most_violent = 0;
    const char *name_team_most_violent  = Nobody;
    int         value_team_most_useless = 0x0FFFFFFF;
    const char *name_team_most_useless  = Nobody;
    int         value_team_most_usefull = 0;
    const char *name_team_most_usefull  = Nobody;
    int         value_team_most_traitor = 0;
    const char *name_team_most_traitor  = Nobody;

    // Search best/worst performers
    FOR_EACH_CHARACTER(*(equipe), ver)
    {
      // Most damage in one shot
      if (ver->GetMostDamage() > value_team_most_violent)
      {
        value_team_most_violent = ver->GetMostDamage();
        name_team_most_violent  = ver->GetName().c_str();
      }
      // Most damage overall to other teams
      if (ver->GetOtherDamage() > value_team_most_usefull)
      {
        value_team_most_usefull = ver->GetOtherDamage();
        name_team_most_usefull  = ver->GetName().c_str();
      }
      // Least damage overall to other teams
      if (ver->GetOtherDamage() < value_team_most_useless)
      {
        value_team_most_useless = ver->GetOtherDamage();
        name_team_most_useless  = ver->GetName().c_str();
      }
      // Most damage overall to his own team
      if (ver->GetOwnDamage() > value_team_most_traitor)
      {
        value_team_most_traitor = ver->GetOwnDamage();
        name_team_most_traitor  = ver->GetName().c_str();
      }
    }

    // Print out results
    txt += Format(_("Team %s results:\n"), (**equipe).GetName().c_str());
    txt += Format(_("  Most violent  :  %s (%i).\n"), name_team_most_violent, value_team_most_violent);
    txt += Format(_("  Most usefull  :  %s (%i).\n"), name_team_most_usefull, value_team_most_usefull);
    txt += Format(_("  Most useless  :  %s (%i).\n"), name_team_most_useless, value_team_most_useless);
    txt += Format(_("  Most sold-out :  %s (%i).\n"), name_team_most_traitor, value_team_most_traitor);

    // Set all team best
    if (value_team_most_violent > value_all_most_violent)
    {
      value_all_most_violent = value_team_most_violent;
      name_all_most_violent = name_team_most_violent;
    }
    // Most damage overall to other teams
    if (value_team_most_usefull > value_all_most_usefull)
    {
      value_all_most_usefull = value_team_most_usefull;
      name_all_most_usefull = name_team_most_usefull;
    }
    // Least damage overall to other teams
    if (value_team_most_useless < value_all_most_useless)
    {
      value_all_most_useless = value_team_most_useless;
      name_all_most_useless = name_team_most_useless;
    }
    // Most damage overall to his own team
    if (value_team_most_traitor > value_all_most_traitor)
    {
      value_all_most_traitor = value_team_most_traitor;
      name_all_most_traitor = name_team_most_traitor;
    }

    // Determine winner
    if (0 < (**equipe).NbAliveCharacter())
    {
      winner_name = (**equipe).GetName().c_str();
      break;
    }
  }
 
  // Print out results
  txt += _("All-Team results:\n");
  txt += Format(_("  Most violent  :  %s (%i).\n"), name_all_most_violent, value_all_most_violent);
  txt += Format(_("  Most usefull  :  %s (%i).\n"), name_all_most_usefull, value_all_most_usefull);
  txt += Format(_("  Most useless  :  %s (%i).\n"), name_all_most_useless, value_all_most_useless);
  txt += Format(_("  Most sold-out :  %s (%i).\n"), name_all_most_traitor, value_all_most_traitor);

  txt += _("End of the game!\n");
  if (winner_name)
  {
    jukebox.Play("share","victory");
    txt += Format(_("%s team has won.\n"), winner_name);
  }
  else
  {
    txt += _("The game has ended as a draw.\n");
  }
  std::cout << txt << std::endl;

  question.Init (txt, true, 0);
  AskQuestion();
}

int Game::AskQuestion (bool draw)
{
  Time * global_time = Time::GetInstance();
  global_time->Pause();

  if (draw) 
    GameLoop::GetInstance()->Draw ();
  
  question.PoseQuestion ();

  global_time->Continue(); 
  return question.reponse;
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
        const char *msg = _("Do you really want to quit? (Y/N)");
        question.Init (msg, true, 0);
	
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
	   question.choix.push_back ( Question::choix_t(SDLK_a + (int)key_x - 'a', 1) );
	}
	
        jukebox.Pause();
        end = (AskQuestion() == 1);
        jukebox.Resume();
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

  world.FreeMem();
  jukebox.StopAll();
   
  if (err)
  {
    std::string txt = Format(_("Error:\n%s"), err_msg.c_str());
    std::cout << std::endl << txt << std::endl;
    question.Init (txt, true, 0);
    AskQuestion (false);
  }
}

void Game::Pause()
{
  jukebox.Pause();
  question.Init (_("Pause"), true, 0);
  AskQuestion();
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
