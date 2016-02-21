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
#include "../graphic/fps.h"
#include "../graphic/video.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../interface/cursor.h"
#include "../interface/game_msg.h"
#include "../interface/interface.h"
#include "../interface/keyboard.h"
#include "../interface/loading_screen.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/wind.h"
#include "../network/network.h"
#include "../network/randomsync.h"
#include "../object/bonus_box.h"
#include "../object/objects_list.h"
#include "../particles/particle.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/stats.h"
#include "../weapon/weapons_list.h"


#define ENABLE_LIMIT_FPS

bool game_fin_partie;

GameLoop * GameLoop::singleton = NULL;

GameLoop * GameLoop::GetInstance()
{
  if (singleton == NULL) {
    singleton = new GameLoop();
  }
  return singleton;
}

GameLoop::GameLoop()
{
  state = PLAYING;
  interaction_enabled = true;
}

void GameLoop::InitGameData_NetServer()
{
  network.client_inited = 1;
  AppWormux * app = AppWormux::GetInstance();
  app->video.SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Server mode");

  ActionHandler * action_handler = ActionHandler::GetInstance();

  Action a_change_state(ACTION_CHANGE_STATE);
  network.SendAction ( &a_change_state );
  network.state = Network::NETWORK_INIT_GAME;

  SendGameMode();

  world.Reset();

  randomSync.Init();

  lst_objects.PlaceBarrels();

  std::cout << "o " << _("Initialise teams") << std::endl;
  teams_list.LoadGamingData(GameMode::GetInstance()->max_characters);

  lst_objects.PlaceMines();
  std::cout << "o " << _("Initialise data") << std::endl;
  CharacterCursor::GetInstance()->Reset();
  Mouse::GetInstance()->Reset();
  fps.Reset();
  Interface::GetInstance()->Reset();
  GameMessages::GetInstance()->Reset();

  // Tells all clients that the server is ready to play
  network.SendAction ( &a_change_state );

  // Wait for all clients to be ready to play
  while (network.state != Network::NETWORK_READY_TO_PLAY)
  {
    action_handler->ExecActions();
    SDL_Delay(200);
  }
  network.SendAction ( &a_change_state );
  network.state = Network::NETWORK_PLAYING;
}

void GameLoop::InitGameData_NetClient()
{
  AppWormux * app = AppWormux::GetInstance();
  app->video.SetWindowCaption( std::string("Wormux ") + Constants::VERSION + " - Client mode");
  ActionHandler * action_handler = ActionHandler::GetInstance();
  std::cout << "o " << _("Initialise teams") << std::endl;

  world.Reset();

  Action a_change_state(ACTION_CHANGE_STATE);

  network.SendAction (&a_change_state);
  while (network.state != Network::NETWORK_READY_TO_PLAY)
  {
    // The server is placing characters on the map
    // We can receive new team / map selection
    action_handler->ExecActions();
    SDL_Delay(100);
  }

  lst_objects.PlaceBarrels();
  teams_list.LoadGamingData(GameMode::GetInstance()->max_characters);
  lst_objects.PlaceMines();

  std::cout << network.state << " : Waiting for people over the network" << std::endl;
  while (network.state != Network::NETWORK_PLAYING)
  {
    // The server waits for everybody to be ready to start
    action_handler->ExecActions();
    SDL_Delay(100);
  }
  std::cout << network.state << " : Run game !" << std::endl;
}

void GameLoop::InitData_Local()
{
  std::cout << "o " << _("Find a random position for characters") << std::endl;
  world.Reset();
  MapsList::GetInstance()->ActiveMap().FreeData();
  lst_objects.PlaceBarrels();
  teams_list.LoadGamingData(0);

  std::cout << "o " << _("Initialise objects") << std::endl;
  lst_objects.PlaceMines();
}

void GameLoop::InitData()
{
  Time::GetInstance()->Reset();

  if (network.IsServer())
    InitGameData_NetServer();
  else if (network.IsClient())
    InitGameData_NetClient();
  else
    InitData_Local();

  CharacterCursor::GetInstance()->Reset();
  Mouse::GetInstance()->Reset();
  Clavier::GetInstance()->Reset();

  fps.Reset();
  if(network.IsConnected())
     chatsession.Reset();
  Interface::GetInstance()->Reset();
  GameMessages::GetInstance()->Reset();
  ParticleEngine::Init();
}

void GameLoop::Init ()
{
  // Display Loading screen
  LoadingScreen::GetInstance()->DrawBackground();

  Game::GetInstance()->MessageLoading();

  // Init all needed data
  std::cout << "o " << _("Initialisation") << std::endl;

  // Load the map
  LoadingScreen::GetInstance()->StartLoading(1, "map_icon", _("Maps"));
  InitData();

  // Init teams
  LoadingScreen::GetInstance()->StartLoading(2, "team_icon", _("Teams"));

  // Teams' creation
  if (teams_list.playing_list.size() < 2)
    Error(_("You need at least two teams to play: "
             "change this in 'Options menu' !"));
  assert (teams_list.playing_list.size() <= GameMode::GetInstance()->max_teams);

  // Initialization of teams' energy
  LoadingScreen::GetInstance()->StartLoading(3, "weapon_icon", _("Weapons"));

  teams_list.InitEnergy();

  // Load teams' sound profiles
  LoadingScreen::GetInstance()->StartLoading(4, "sound_icon", _("Sounds"));

  jukebox.LoadXML("default");
  FOR_EACH_TEAM(team)
    if ( (**team).GetSoundProfile() != "default" )
      jukebox.LoadXML((**team).GetSoundProfile()) ;

  // Begin to play !!
  // Music -> sound should be choosed in map.Init and then we just have to call jukebox.PlayMusic()
  if (jukebox.UseMusic()) jukebox.Play ("share", "music/grenouilles", -1);

  Game::GetInstance()->SetEndOfGameStatus( false );

  Mouse::GetInstance()->CenterPointer();
  Mouse::GetInstance()->SetPointer(POINTER_SELECT);

  // First "selection" of a weapon -> fix bug 6576
  ActiveTeam().AccessWeapon().Select();

  SetState (PLAYING, true);
}

void GameLoop::Refresh()
{
  RefreshClock();
  GameMessages::GetInstance()->Refresh();
  camera.Refresh();

  // Poll and treat keyboard and mouse events
  SDL_Event event;

   while( SDL_PollEvent( &event) )
     {
        if ( event.type == SDL_QUIT)
          {
             std::cout << "SDL_QUIT received ===> exit TODO" << std::endl;
             Game::GetInstance()->SetEndOfGameStatus( true );
             std::cout << "FIN PARTIE" << std::endl;
             return;
          }
        if ( event.type == SDL_MOUSEBUTTONDOWN )
          {
             Mouse::GetInstance()->TraiteClic( &event);
          }
        if ( event.type == SDL_KEYDOWN
        ||   event.type == SDL_KEYUP)
          {
             if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
             {
                  Game::GetInstance()->SetEndOfGameStatus( true );
                  std::cout << "FIN PARTIE" << std::endl;
                  return;
             }

             Clavier::GetInstance()->HandleKeyEvent( &event);
          }
     }

  // How many frame by seconds ?
  fps.Refresh();

  if (!Time::GetInstance()->IsGamePaused())
  {
    // Keyboard and mouse refresh
    if (
        (interaction_enabled && state != END_TURN)
        || (ActiveTeam().GetWeapon().IsActive() && ActiveTeam().GetWeapon().override_keys) // for driving supertux for example
        )
    {
      Mouse::GetInstance()->Refresh();
      Clavier::GetInstance()->Refresh();
    }

    do
    {
      ActionHandler::GetInstance()->ExecActions();
      if(network.sync_lock) SDL_Delay(SDL_TIMESLICE);
    }
    while(network.sync_lock);

    FOR_ALL_CHARACTERS(equipe,ver) ver -> Refresh();

    // Recompute energy of each team
    FOR_EACH_TEAM(team)
      (**team).Refresh();
    teams_list.RefreshEnergy();

    ActiveTeam().AccessWeapon().Manage();
    lst_objects.Refresh();
    ParticleEngine::Refresh();
    CharacterCursor::GetInstance()->Refresh();

  }

  // Refresh the map
  world.Refresh();
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
  FOR_ALL_CHARACTERS(equipe,ver) {
    if (&(*ver) != &ActiveCharacter()) {
      ver -> Draw();
    }
  }

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

  // Draw teams energy
  StatStart("GameDraw::team_energy");
  FOR_EACH_TEAM(team)
    (**team).DrawEnergy();
  StatStop("GameDraw::team_energy");

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

  // Display wind bar
  StatStart("GameDraw:wind_bar");
  wind.Draw();
  StatStop("GameDraw:wind_bar");

  // Draw MsgBox for chat network
  if(network.IsConnected()){
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

void GameLoop::Run()
{
#ifdef ENABLE_LIMIT_FPS
  uint sleep_fps=0;
  uint delay=0;
#endif

  // loop until game is finished
  do
  {
#ifdef ENABLE_LIMIT_FPS
    unsigned int start = SDL_GetTicks();
#endif

    Game::GetInstance()->SetEndOfGameStatus( false );

    // one loop
    StatStart("GameLoop:Refresh()");
    Refresh();
    StatStop("GameLoop:Refresh()");
    StatStart("GameLoop:Draw()");
    CallDraw ();
    StatStop("GameLoop:Draw()");

    // try to adjust to max Frame by seconds
#ifdef ENABLE_LIMIT_FPS
    delay = SDL_GetTicks()-start;

    if (delay < AppWormux::GetInstance()->video.GetSleepMaxFps())
      sleep_fps = AppWormux::GetInstance()->video.GetSleepMaxFps() - delay;
    else
      sleep_fps = 0;
    if(sleep_fps >= SDL_TIMESLICE)
      SDL_Delay(sleep_fps);
#endif
  } while( !Game::GetInstance()->GetEndOfGameStatus() );
}

void GameLoop::RefreshClock()
{
  Time * global_time = Time::GetInstance();
  if (global_time->IsGamePaused()) return;

  if (1000 < global_time->Read() - pause_seconde)
    {
      pause_seconde = global_time->Read();

      switch (state) {

      case PLAYING:
        if (duration <= 1) {
           jukebox.Play("share", "end_turn");
           SetState (END_TURN);
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

          if (Game::GetInstance()->IsGameFinished())
            Game::GetInstance()->SetEndOfGameStatus( true );
          else if (BonusBox::NewBonusBox())
	    break;
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

void GameLoop::SetState(int new_state, bool begin_game)
{
  ActionHandler * action_handler = ActionHandler::GetInstance();

  // already in good state, nothing to do
  if ((state == new_state) && !begin_game) return;

  state = new_state;

  action_handler->ExecActions();

  Interface::GetInstance()->weapons_menu.Hide();

  Time * global_time = Time::GetInstance();
  GameMode * game_mode = GameMode::GetInstance();

  switch (state)
  {
  // Begining of a new turn:
  case PLAYING:
    MSG_DEBUG("game.statechange", "Playing" );
    // Init. le compteur
    duration = game_mode->duration_turn;
    Interface::GetInstance()->UpdateTimer(duration);
    Interface::GetInstance()->EnableDisplayTimer(true);
    pause_seconde = global_time->Read();

    if (network.IsServer() || network.IsLocal())
     wind.ChooseRandomVal();

     character_already_chosen = false;

    // Prepare each character for a new turn
    FOR_ALL_LIVING_CHARACTERS(equipe,ver) ver -> PrepareTurn();

    // Select the next team
    assert (!Game::GetInstance()->IsGameFinished());

    if(network.IsLocal() || network.IsServer())
    {
      do
      {
        teams_list.NextTeam (begin_game);
        action_handler->ExecActions();
      } while (ActiveTeam().NbAliveCharacter() == 0);


      if( game_mode->allow_character_selection==GameMode::CHANGE_ON_END_TURN
       || game_mode->allow_character_selection==GameMode::BEFORE_FIRST_ACTION_AND_END_TURN)
      {
        ActiveTeam().NextCharacter();
      }

      if( network.IsServer() )
      {
        // Tell to clients which character in the team is now playing
        Action playing_char(ACTION_CHANGE_CHARACTER, (int)ActiveCharacter().GetCharacterIndex());
        network.SendAction(&playing_char);

        printf("Action_NextCharacter:\n");
        printf("char_index = %i\n",ActiveCharacter().GetCharacterIndex());
        printf("Playing character : %i %s\n", ActiveCharacter().GetCharacterIndex(), ActiveCharacter().GetName().c_str());
        printf("Playing team : %i %s\n", ActiveCharacter().GetTeamIndex(), ActiveTeam().GetName().c_str());
        printf("Alive characters: %i / %i\n\n",ActiveTeam().NbAliveCharacter(),ActiveTeam().GetNbCharacters());

      }
    }

    action_handler->ExecActions();

//    assert (!ActiveCharacter().IsDead());
    camera.ChangeObjSuivi (&ActiveCharacter(), true, true);
    interaction_enabled = true; // Be sure that we can play !

    // Applying Disease damage and Death mode.
    ApplyDiseaseDamage();
    ApplyDeathMode();

    break;

  // The character have shooted, but can still move
  case HAS_PLAYED:
    MSG_DEBUG("game.statechange", "Has played, now can move");
    duration = game_mode->duration_move_player;
    pause_seconde = global_time->Read();
    Interface::GetInstance()->UpdateTimer(duration);
    CharacterCursor::GetInstance()->Hide();
    break;

  // Little pause at the end of the turn
  case END_TURN:
    MSG_DEBUG("game.statechange", "End of turn");
    ActiveTeam().AccessWeapon().SignalTurnEnd();
    CharacterCursor::GetInstance()->Hide();
    duration = game_mode->duration_exchange_player;
    Interface::GetInstance()->UpdateTimer(duration);
    Interface::GetInstance()->EnableDisplayTimer(false);
    pause_seconde = global_time->Read();

    interaction_enabled = false; // Be sure that we can NOT play !
    if(network.IsServer())
      SyncCharacters(); // Send information about energy and the position of every character
    break;
  }
}

PhysicalObj* GameLoop::GetMovingObject()
{
  if (!ActiveCharacter().IsImmobile()) return &ActiveCharacter();

  FOR_ALL_CHARACTERS(equipe,ver)
  {
    if (!ver -> IsImmobile() && !ver -> IsGhost())
    {
      MSG_DEBUG("game.endofturn", "%s is not ready", (*ver).GetName().c_str())
      return &(*ver);
    }
  }

  FOR_EACH_OBJECT(object)
  {
    if (!object -> ptr ->IsImmobile())
    {
      MSG_DEBUG("game.endofturn", "%s is moving", object-> ptr ->GetName().c_str())
      return object->ptr;
    }
  }

  return ParticleEngine::IsSomethingMoving();
}

bool GameLoop::IsAnythingMoving()
{
  // Is the weapon still active or an object still moving ??
  bool object_still_moving = false;

  if (ActiveTeam().GetWeapon().IsActive()) object_still_moving = true;

  if (!object_still_moving)
  {
    PhysicalObj *obj = GetMovingObject();
    if (obj != NULL)
    {
      camera.ChangeObjSuivi (obj, true, true);
      object_still_moving = true;
    }
  }

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
    if (ActiveTeam().GetWeaponType() == WEAPON_SUICIDE) {
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
  } else if (ActiveTeam().GetWeaponType() == WEAPON_GUN) {
    txt = Format(_("What a shame for %s - he was killed by a simple gun!"),
                 character -> GetName().c_str());
  } else {
    txt = Format(_("%s (%s team) has died."),
                 character -> GetName().c_str(),
                 character -> GetTeam().GetName().c_str());
  }

  GameMessages::GetInstance()->Add (txt);

  // Turn end if the playing character is dead
  if (character == &ActiveCharacter()) SetState (END_TURN);
}

// Signal falling or any king of damage of a character
void GameLoop::SignalCharacterDamage(Character *character)
{
  if (character->IsActiveCharacter())
    {
      SetState (END_TURN);
    }
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
      if (character->GetEnergy() > GameMode::GetInstance()->damage_per_turn_during_death_mode)
        character->SetEnergyDelta(-GameMode::GetInstance()->damage_per_turn_during_death_mode);
      else
        character->SetEnergyDelta(-character->GetEnergy() + 1);
    }
  }
}
