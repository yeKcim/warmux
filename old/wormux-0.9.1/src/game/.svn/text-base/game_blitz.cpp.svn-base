/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Specialization of Game methods for the blitz mode.
 *****************************************************************************/

#include "character/character.h"
#include "game/game_blitz.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "include/action_handler.h"
#include "interface/cursor.h"
#include "interface/interface.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "network/network.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"

// Should be read from game mode
GameBlitz::GameBlitz()
  : counter(0)
{ }

void GameBlitz::EndOfGame()
{
  SetState(END_TURN);
  GameMessages::GetInstance()->Add (_("And the winner is..."));

  counter = GameMode::GetInstance()->duration_exchange_player + 2;
  while (counter)
    MainLoop();
}

GameBlitz::time_iterator GameBlitz::GetCurrentTeam()
{
  time_iterator cur = times.find(&ActiveTeam());
  ASSERT(cur != times.end());
  return cur;
}

GameBlitz::time_iterator GameBlitz::KillTeam(GameBlitz::time_iterator cur)
{
  FOR_EACH_LIVING_CHARACTER(cur->first, character)
  {
    character->Die();
  }
  GameMessages::GetInstance()->Add (Format(_("%s team was fragged down."), cur->first->GetName().c_str()));
  cur->second = 0;
  times.erase(cur);
  return times.end();
}

bool GameBlitz::Run()
{
  // Make sure map is empty
  times.clear();
  FOR_EACH_TEAM(team)
  {
    times[*team] = GameMode::GetInstance()->duration_turn;
  }

  counter = 0;;
  return Game::Run();
}

void GameBlitz::RefreshClock()
{
  Time * global_time = Time::GetInstance();

  if (1000 < global_time->Read() - last_clock_update) {
    last_clock_update = global_time->Read();

    if (counter) {
      counter--;
    } else {
      time_iterator cur = GetCurrentTeam();

      uint duration = cur->second;

      switch (state) {

      case PLAYING:
        if (duration <= 1) {
          JukeBox::GetInstance()->Play("default", "end_turn");
          cur = KillTeam(cur);
          SetState(END_TURN);
        } else {
          duration--;

	  if (duration == 12) {
	    countdown_sample.Play("default", "countdown-end_turn");
	  }

	  if (duration > 10) {
	    Interface::GetInstance()->UpdateTimer(duration, false, false);
	  } else {
	    Interface::GetInstance()->UpdateTimer(duration, true, false);
	  }
        }
        break;

      case HAS_PLAYED:
        if (duration <= 1) {
          cur = KillTeam(cur);
        } else {
          duration--;
          Interface::GetInstance()->UpdateTimer(duration, false, false);
        }
        SetState(END_TURN);
        break;

      case END_TURN:
        if (IsAnythingMoving() && duration<2) {
          duration = 1;
          break;
        }

        if (IsGameFinished())
          break;

        if (give_objbox && GetWorld().IsOpen()) {
          NewBox();
          give_objbox = false;
          break;
        }
        else {
          SetState(PLAYING);
          break;
        }
      } // switch

      if (cur != times.end())
        cur->second = duration;
    }// if !counter
  }
}

uint GameBlitz::GetRemainingTime() const
{
  return times.find(&ActiveTeam())->second;
}

// Beginning of a new turn
void GameBlitz::__SetState_PLAYING()
{
  MSG_DEBUG("game.statechange", "Playing" );

  Wind::GetRef().ChooseRandomVal();

  SetCharacterChosen(false);

  // Prepare each character for a new turn
  FOR_ALL_LIVING_CHARACTERS(team,character)
    character->PrepareTurn();

  // Select the next team
  ASSERT (!IsGameFinished());
  GetTeamsList().NextTeam();

  // initialize counter
  Interface::GetInstance()->UpdateTimer(GetCurrentTeam()->second, false, true);
  Interface::GetInstance()->EnableDisplayTimer(true);
  last_clock_update = Time::GetInstance()->Read();

  give_objbox = true; //hack: make it so that no more than one objbox per turn
}

void GameBlitz::__SetState_HAS_PLAYED()
{
  MSG_DEBUG("game.statechange", "Has played, now can move");
  last_clock_update = Time::GetInstance()->Read();
  CharacterCursor::GetInstance()->Hide();
}

void GameBlitz::__SetState_END_TURN()
{
  MSG_DEBUG("game.statechange", "End of turn");
  ActiveTeam().AccessWeapon().SignalTurnEnd();
  ActiveTeam().AccessWeapon().Deselect();
  CharacterCursor::GetInstance()->Hide();
  last_clock_update = Time::GetInstance()->Read();
  // Ensure the clock sprite isn't NULL:
  Interface::GetInstance()->UpdateTimer(0, false, true);

  // Applying Disease damage and Death mode.
  ApplyDiseaseDamage();
}

bool GameBlitz::IsGameFinished() const
{
  uint num = 0;

  for (std::map<Team*, uint>::const_iterator it = times.begin(); it != times.end(); ++it) {
    if (it->second != 0 && it->first->NbAliveCharacter())
      num++;
  }

  // If more than one team with time left > 0 and alive character, game not finished
  return (num < 2);
}
