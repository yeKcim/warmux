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
#include "../object/bonus_box.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../weapon/weapons_list.h"

using namespace Wormux;

Game game;  // TODO: global variables are bad

Game::Game()
{
  //initialise = false;
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
  bool winner_found = false;

  std::string txt(_("End of the game!"));
  txt += " - ";

  FOR_EACH_TEAM(equipe)
  {
    if (0 < (**equipe).NbAliveCharacter())
    {
      winner_found = true;
      txt += Format(_("%s team has won."), (**equipe).GetName().c_str());
      break;
    }
  }
  if (winner_found) 
    jukebox.Play("share","victory");
  else
    txt += _("The game has ended as a draw.");
  std::cout << txt << std::endl;

  question.Init (txt, true, 0);
  AskQuestion();
}

int Game::AskQuestion (bool draw)
{
  global_time.Pause();

  if (draw) 
    game_loop.Draw ();
  
  question.PoseQuestion ();

  global_time.Continue(); 
  return question.reponse;
}

void Game::Start()
{
  bool err=true;
  bool end;
  std::string err_msg;

  try
  {
    InitGame ();

    do
    {
      isGameLaunched = true;
      image_par_seconde.Reset();
      
      game_loop.Run();
     
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
  caisse.FreeMem();
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
