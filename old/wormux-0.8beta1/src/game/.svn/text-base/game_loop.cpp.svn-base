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
 * Game loop : drawing and data handling
 *****************************************************************************/

#include "game_loop.h"
#include <SDL.h>
#include <SDL_image.h>
#include <sstream>
#include <iostream>
#include "config.h"
#include "game.h"
#include "game_mode.h"
#include "time.h"
#include "ai/ai_engine.h"
#include "graphic/fps.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/cursor.h"
#include "interface/game_msg.h"
#include "interface/interface.h"
#include "interface/keyboard.h"
#include "interface/loading_screen.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "network/network.h"
#include "network/network_server.h"
#include "network/randomsync.h"
#include "object/bonus_box.h"
#include "object/medkit.h"
#include "object/objects_list.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/stats.h"
#include "weapon/weapons_list.h"


#ifdef DEBUG
// Uncomment this to get an image during the game under Valgrind
// DON'T USE THIS IF YOU INTEND TO PLAY NETWORKED GAMES!
//#define USE_VALGRIND
#endif


GameLoop * GameLoop::singleton = NULL;

GameLoop * GameLoop::GetInstance()
{
  if (singleton == NULL) {
    singleton = new GameLoop();
  }
  return singleton;
}

GameLoop::GameLoop():
  state(PLAYING),
  pause_seconde(0),
  duration(0),
  current_ObjBox(NULL),
  give_objbox(true),
  fps(),
  character_already_chosen(false),
  chatsession()
{ }


void GameLoop::Init()
{
  fps.Reset();
  IgnorePendingInputEvents();
  camera.Reset();

  ActionHandler::GetInstance()->ExecActions();

  FOR_ALL_CHARACTERS(team, character)
    (*character).ResetDamageStats();

  SetState(END_TURN, true); // begin with a small pause
}

// ####################################################################
// ####################################################################

// ignore all pending events
// useful after loading screen
void GameLoop::IgnorePendingInputEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event));
}

void GameLoop::RefreshInput()
{
  // Poll and treat keyboard and mouse events
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    if ( event.type == SDL_QUIT) {
      std::cout << "SDL_QUIT received ===> exit TODO" << std::endl;
      Game::GetInstance()->UserWantEndOfGame();
      std::cout << _("END OF GAME") << std::endl;
      return;
    }

    if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE) {
      Game::GetInstance()->UserWantEndOfGame();
      std::cout << _("END OF GAME") << std::endl;
      return;
    }

    // Mouse event
    if (Mouse::GetInstance()->HandleClic(event))
      continue;

    // Keyboard event
    Keyboard::GetInstance()->HandleKeyEvent(event);
  }

  // Keyboard and mouse refresh
  Mouse::GetInstance()->Refresh();
  Keyboard::GetInstance()->Refresh();
  AIengine::GetInstance()->Refresh();

  // Execute action
  do {
    ActionHandler::GetInstance()->ExecActions();
    if(Network::GetInstance()->sync_lock) SDL_Delay(SDL_TIMESLICE);
  } while(Network::GetInstance()->sync_lock);

  GameMessages::GetInstance()->Refresh();

  if (!Game::GetInstance()->IsGameFinished())
    camera.Refresh();
}

// ####################################################################
// ####################################################################

void GameLoop::RefreshObject()
{
  FOR_ALL_CHARACTERS(team,character)
    character->Refresh();

  // Recompute energy of each team
  FOR_EACH_TEAM(team)
    (**team).Refresh();
  teams_list.RefreshEnergy();

  ActiveTeam().AccessWeapon().Manage();
  lst_objects.Refresh();
  ParticleEngine::Refresh();
  CharacterCursor::GetInstance()->Refresh();
}

void GameLoop::Draw ()
{
  // Draw the sky
  StatStart("GameDraw:sky");
  world.DrawSky();
  StatStop("GameDraw:sky");

  // Draw the map
  StatStart("GameDraw:world");
  world.Draw();
  StatStop("GameDraw:world");

  // Draw the characters
  StatStart("GameDraw:characters");
  FOR_ALL_CHARACTERS(team,character)
    if (!character->IsActiveCharacter())
      character->Draw();

  StatStart("GameDraw:particles_behind_active_character");
  ParticleEngine::Draw(false);
  StatStop("GameDraw:particles_behind_active_character");

  StatStart("GameDraw:active_character");
  ActiveCharacter().Draw();
  if (!ActiveCharacter().IsDead() && state != END_TURN) {
        ActiveTeam().crosshair.Draw();
        ActiveTeam().AccessWeapon().Draw();
  }
  StatStop("GameDraw:active_character");
  StatStop("GameDraw:characters");

  // Draw objects
  StatStart("GameDraw:objects");
  lst_objects.Draw();
  ParticleEngine::Draw(true);
  StatStart("GameDraw:objects");

  // Draw arrow on top of character
  StatStart("GameDraw:arrow_character");
  CharacterCursor::GetInstance()->Draw();
  StatStop("GameDraw:arrow_character");

  // Draw waters
  StatStart("GameDraw:water");
  world.DrawWater();
  StatStop("GameDraw:water");

  // Draw game messages
  StatStart("GameDraw::game_messages");
  GameMessages::GetInstance()->Draw();
  StatStop("GameDraw::game_messages");

  // Draw optionals
  StatStart("GameDraw:fps_and_map_author_name");
  world.DrawAuthorName();
  fps.Draw();
  StatStop("GameDraw:fps_and_map_author_name");

  StatStop("GameDraw:other");

  // Draw the interface (current team's information, weapon's ammo)
  StatStart("GameDraw:interface");
  Interface::GetInstance()->Draw ();
  StatStop("GameDraw:interface");

  // Draw MsgBox for chat network
  if(Network::GetInstance()->IsConnected()){
    StatStart("GameDraw:chatsession");
    chatsession.Show();
    StatStop("GameDraw:chatsession");
  }

  // Add one frame to the fps counter ;-)
  fps.AddOneFrame();

  // Draw the mouse pointer
  StatStart("GameDraw:mouse_pointer");
  Mouse::GetInstance()->Draw();
  StatStart("GameDraw:mouse_pointer");
}

void GameLoop::CallDraw()
{
  Draw();
  StatStart("GameDraw:flip()");
  AppWormux::GetInstance()->video.Flip();
  StatStop("GameDraw:flip()");
}

void GameLoop::PingClient()
{
  Action * a = new Action(Action::ACTION_NETWORK_PING);
  ActionHandler::GetInstance()->NewAction(a);
}

// ####################################################################
// ####################################################################

void GameLoop::Run()
{
  // Time to wait between 2 loops
  int delay = 0;
  // Time to display the next frame
  uint time_of_next_frame = 0;
  // Time to display the compute next physic engine frame
  uint time_of_next_phy_frame = 0;

  // loop until game is finished
  do
  {
    // Refresh clock value
    RefreshClock();
    time_of_next_phy_frame = Time::GetInstance()->Read() + Time::GetInstance()->GetDelta();

    if(Time::GetInstance()->Read() % 1000 == 20 && Network::GetInstance()->IsServer())
      PingClient();
    StatStart("GameLoop:RefreshInput()");
    RefreshInput();
    StatStop("GameLoop:RefreshInput()");
    StatStart("GameLoop:RefreshObject()");
    RefreshObject();
    StatStop("GameLoop:RefreshObject()");

    // Refresh the map
    world.Refresh();

    // try to adjust to max Frame by seconds
#ifndef USE_VALGRIND
    if (time_of_next_frame < Time::GetInstance()->ReadRealTime()) {
      // Only display if the physic engine isn't late
      if (time_of_next_phy_frame > Time::GetInstance()->ReadRealTime())
      {
#endif
        StatStart("GameLoop:Draw()");
        CallDraw();
        // How many frame by seconds ?
        fps.Refresh();
        StatStop("GameLoop:Draw()");
        time_of_next_frame += AppWormux::GetInstance()->video.GetSleepMaxFps();
#ifndef USE_VALGRIND
      }
    }
#endif

    delay = time_of_next_phy_frame - Time::GetInstance()->ReadRealTime();
    if (delay >= 0)
      SDL_Delay(delay);
  } while( !Game::GetInstance()->IsGameFinished()
	   && !Time::GetInstance()->IsGamePaused());

  // the game is finished but we won't go at the results screen to fast!
  if (Game::GetInstance()->NbrRemainingTeams() <= 1) {
    EndOfGame();
  }
}

void GameLoop::EndOfGame()
{
  int delay = 0;
  uint time_of_next_frame = SDL_GetTicks();
  uint previous_time_frame = 0;

  SetState(END_TURN);
  duration = GameMode::GetInstance()->duration_exchange_player + 2;
  GameMessages::GetInstance()->Add (_("And the winner is..."));

  while (duration >= 1 ) {
    // Refresh clock value
    RefreshClock();
    RefreshInput();
    if(previous_time_frame < Time::GetInstance()->Read()) {
      RefreshObject();
    } else {
      previous_time_frame = Time::GetInstance()->Read();
    }

    // Refresh the map
    world.Refresh();

    // try to adjust to max Frame by seconds
    time_of_next_frame += AppWormux::GetInstance()->video.GetSleepMaxFps();
    if (time_of_next_frame > SDL_GetTicks()) {
      CallDraw();
      // How many frame by seconds ?
      fps.Refresh();
    }
    delay = time_of_next_frame - SDL_GetTicks();
    if (delay >= 0)
      SDL_Delay(delay);
  }
}

void GameLoop::RefreshClock()
{
  Time * global_time = Time::GetInstance();
  if (global_time->IsGamePaused()) return;
  global_time->Refresh();

  if (1000 < global_time->Read() - pause_seconde)
    {
      pause_seconde = global_time->Read();

      switch (state) {

      case PLAYING:
        if (duration <= 1) {
           jukebox.Play("share", "end_turn");
           SetState(END_TURN);
        } else {
          duration--;
          Interface::GetInstance()->UpdateTimer(duration);
        }
        break;

      case HAS_PLAYED:
        if (duration <= 1) {
          SetState (END_TURN);
        } else {
          duration--;
          Interface::GetInstance()->UpdateTimer(duration);
        }
        break;

      case END_TURN:
        if (duration <= 1) {

          if (IsAnythingMoving()) {
	    duration = 1;
	    // Hack to be sure that nothing is moving since enough time
	    // it avoids giving hand to another team during the end of an explosion for example
	    break;
	  }

	  if (Game::GetInstance()->IsGameFinished()) {
	    duration--;
	    break;
	  }

          if (give_objbox && ObjBox::NewBox()) {
            give_objbox = false;
	    break;
          }
	  else {
	    ActiveTeam().AccessWeapon().Deselect();
	    SetState(PLAYING);
            break;
          }
        } else {
          duration--;
        }
        break;
      } // switch
    }// if
}

void GameLoop::SetCurrentBox(ObjBox * current_box)
{
  current_ObjBox = current_box;
}

ObjBox * GameLoop::GetCurrentBox() const
{
  return current_ObjBox;
}

// Begining of a new turn
void GameLoop::__SetState_PLAYING()
{
  MSG_DEBUG("game.statechange", "Playing" );

  // Center the cursor
  Mouse::GetInstance()->CenterPointer();

  // initialize counter
  duration = GameMode::GetInstance()->duration_turn;
  Interface::GetInstance()->UpdateTimer(duration);
  Interface::GetInstance()->EnableDisplayTimer(true);
  pause_seconde = Time::GetInstance()->Read();

  if (Network::GetInstance()->IsServer() || Network::GetInstance()->IsLocal())
    wind.ChooseRandomVal();

  character_already_chosen = false;

  // Prepare each character for a new turn
  FOR_ALL_LIVING_CHARACTERS(team,character)
    character->PrepareTurn();

  // Select the next team
  assert (!Game::GetInstance()->IsGameFinished());

  if (Network::GetInstance()->IsLocal() || Network::GetInstance()->IsServer())
    {
      teams_list.NextTeam();

      if ( GameMode::GetInstance()->allow_character_selection==GameMode::CHANGE_ON_END_TURN
	   || GameMode::GetInstance()->allow_character_selection==GameMode::BEFORE_FIRST_ACTION_AND_END_TURN)
	{
	  ActiveTeam().NextCharacter();
	}

      if ( Network::GetInstance()->IsServer() )
	{
	  // Tell to clients which character in the team is now playing
	  Action playing_char(Action::ACTION_GAMELOOP_CHANGE_CHARACTER);
	  playing_char.StoreActiveCharacter();
	  Network::GetInstance()->SendAction(&playing_char);

	  printf("Action_ChangeCharacter:\n");
	  printf("char_index = %i\n",ActiveCharacter().GetCharacterIndex());
	  printf("Playing character : %i %s\n", ActiveCharacter().GetCharacterIndex(), ActiveCharacter().GetName().c_str());
	  printf("Playing team : %i %s\n", ActiveCharacter().GetTeamIndex(), ActiveTeam().GetName().c_str());
	  printf("Alive characters: %i / %i\n\n",ActiveTeam().NbAliveCharacter(),ActiveTeam().GetNbCharacters());

	}
    }

  camera.FollowObject (&ActiveCharacter(), true, true);
  give_objbox = true; //hack make it so no more than one objbox per turn

  // Applying Disease damage and Death mode.
  ApplyDiseaseDamage();
  ApplyDeathMode();
}

void GameLoop::__SetState_HAS_PLAYED()
{
  MSG_DEBUG("game.statechange", "Has played, now can move");
  duration = GameMode::GetInstance()->duration_move_player;
  pause_seconde = Time::GetInstance()->Read();
  Interface::GetInstance()->UpdateTimer(duration);
  CharacterCursor::GetInstance()->Hide();
}

void GameLoop::__SetState_END_TURN()
{
  MSG_DEBUG("game.statechange", "End of turn");
  ActiveTeam().AccessWeapon().SignalTurnEnd();
  CharacterCursor::GetInstance()->Hide();
  duration = GameMode::GetInstance()->duration_exchange_player;
  Interface::GetInstance()->UpdateTimer(duration);
  Interface::GetInstance()->EnableDisplayTimer(false);
  pause_seconde = Time::GetInstance()->Read();
}

void GameLoop::Really_SetState(game_loop_state_t new_state)
{
  // already in good state, nothing to do
  if (state == new_state) return;
  state = new_state;

  Interface::GetInstance()->weapons_menu.Hide();

  switch (state)
  {
  // Begining of a new turn:
  case PLAYING:  

    if (Network::GetInstance()->IsServer()) {
      // Send information about energy and position of every characters
      // A character can be hurted during the END_OF_TURN...
      SyncCharacters();
    }

    __SetState_PLAYING();
    break;

  // The character have shooted, but can still move
  case HAS_PLAYED:
    __SetState_HAS_PLAYED();
    break;

  // Little pause at the end of the turn
  case END_TURN:
    __SetState_END_TURN();

    if (Network::GetInstance()->IsServer())
      SyncCharacters(); // Send information about energy and position of every characters
    break;
  }
}

void GameLoop::SetState(game_loop_state_t new_state, bool begin_game)
{
  if (!Network::GetInstance()->IsServer() && !Network::GetInstance()->IsLocal())
    return;

  // already in good state, nothing to do
  if ((state == new_state) && !begin_game) return;

  Action *a = new Action(Action::ACTION_GAMELOOP_SET_STATE, new_state);
  ActionHandler::GetInstance()->NewAction(a);
}

PhysicalObj* GameLoop::GetMovingObject()
{
  if (!ActiveCharacter().IsImmobile()) return &ActiveCharacter();

  FOR_ALL_CHARACTERS(team,character)
  {
    if (!character->IsImmobile() && !character->IsGhost())
    {
      MSG_DEBUG("game.endofturn", "%s is not ready", character->GetName().c_str());
      return &(*character);
    }
  }

  FOR_EACH_OBJECT(object)
  {
    if (!(*object)->IsImmobile())
    {
      MSG_DEBUG("game.endofturn", "%s is moving", (*object)->GetName().c_str());
      return (*object);
    }
  }

  return ParticleEngine::IsSomethingMoving();
}

bool GameLoop::IsAnythingMoving()
{
  // Is the weapon still active or an object still moving ??
  bool object_still_moving = false;

  if (ActiveTeam().GetWeapon().IsInUse())
  {
    MSG_DEBUG("game.endofturn", "Weapon %s is still active", ActiveTeam().GetWeapon().GetName().c_str());
    object_still_moving = true;
  }

  if (!object_still_moving)
    if (GetMovingObject() != NULL)
      object_still_moving = true;

  return object_still_moving;
}

// Signal death of a character
void GameLoop::SignalCharacterDeath (Character *character)
{
  std::string txt;

  if (!Game::GetInstance()->IsGameLaunched())
    return;

  if (character -> IsDrowned()) {
    txt = Format(_("%s has fallen in water."), character -> GetName().c_str());

  } else if (&ActiveCharacter() == character) { // Active Character is dead
    CharacterCursor::GetInstance()->Hide();

    // Is this a suicide ?
    if (ActiveTeam().GetWeaponType() == Weapon::WEAPON_SUICIDE) {
      txt = Format(_("%s commits suicide !"), character -> GetName().c_str());

      // Dead in moving ?
    } else if (state == PLAYING) {
      txt = Format(_("%s has fallen off the map!"),
                   character -> GetName().c_str());
       jukebox.Play(ActiveTeam().GetSoundProfile(), "out");

      // The playing character killed hisself
    } else {
      txt = Format(_("%s is dead because he is clumsy!"),
                   character -> GetName().c_str());
    }
  } else if (!ActiveCharacter().IsDead()
             && character->GetTeam().IsSameAs(ActiveTeam()) ) {
    txt = Format(_("%s is a psychopath, he has killed a member of %s team!"),
                 ActiveCharacter().GetName().c_str(), character -> GetName().c_str());
  } else if (ActiveTeam().GetWeaponType() == Weapon::WEAPON_GUN) {
    txt = Format(_("What a shame for %s - he was killed by a simple gun!"),
                 character -> GetName().c_str());
  } else {
    txt = Format(_("%s (%s) has died."),
                 character -> GetName().c_str(),
                 character -> GetTeam().GetName().c_str());
  }

  GameMessages::GetInstance()->Add (txt);

  // Turn end if the playing character is dead
  // or if there is only one team alive
  if (character->IsActiveCharacter() || Game::GetInstance()->IsGameFinished())
    SetState(END_TURN);
}

// Signal falling or any kind of damage of a character
void GameLoop::SignalCharacterDamage(Character *character)
{
  if (character->IsActiveCharacter())
    SetState(END_TURN);
}

// Apply Disease damage
void GameLoop::ApplyDiseaseDamage()
{
  FOR_ALL_LIVING_CHARACTERS(team, character) {
    if (character->IsDiseased()) {
      character->SetEnergyDelta(-character->GetDiseaseDamage());
      character->DecDiseaseDuration();
    }
  }
}

// Reduce energy of each character if we are in death mode
void GameLoop::ApplyDeathMode ()
{
  if(Time::GetInstance()->Read() > GameMode::GetInstance()->duration_before_death_mode * 1000)
  {
    GameMessages::GetInstance()->Add (_("Hurry up, you are too slow !!"));
    FOR_ALL_LIVING_CHARACTERS(team, character)
    {
      // If the character energy is lower than damage
      // per turn we reduce the character's health to 1
      if (static_cast<uint>(character->GetEnergy()) >
          GameMode::GetInstance()->damage_per_turn_during_death_mode)
        character->SetEnergyDelta(-GameMode::GetInstance()->damage_per_turn_during_death_mode);
      else
        character->SetEnergy(1);
    }
  }
}
