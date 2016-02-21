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
#include <iostream>
#include "game/game.h"
#include "game/config.h"
#include "game/time.h"
#include "ai/ai_engine.h"
#include "map/camera.h"
#include "character/character.h"
#include "game/game_init.h"
#include "game/game_mode.h"
#include "graphic/fps.h"
#include "graphic/video.h"
#include "gui/question.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "interface/cursor.h"
#include "interface/interface.h"
#include "interface/keyboard.h"
#include "interface/joystick.h"
#include "interface/mouse.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "menu/pause_menu.h"
#include "menu/results_menu.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "object/objbox.h"
#include "object/bonus_box.h"
#include "object/medkit.h"
#include "object/objects_list.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/results.h"
#include "tool/i18n.h"
#include "tool/random.h"
#include "tool/stats.h"

#ifdef DEBUG
// Uncomment this to get an image during the game under Valgrind
// DON'T USE THIS IF YOU INTEND TO PLAY NETWORKED GAMES!
//#define USE_VALGRIND
#endif


Game * Game::singleton = NULL;

Game * Game::GetInstance()
{
  if (singleton == NULL) {
    singleton = new Game();
  }
  return singleton;
}

void Game::MessageLoading() const
{
  GameInit loading_sreen; /* displays the loading screen stuff */

  std::cout << std::endl;
  std::cout << "[ " << _("Starting a new game") << " ]" << std::endl;
}

void Game::Start()
{
  Keyboard::GetInstance()->Reset();
  Joystick::GetInstance()->Reset();

  MessageLoading();

  try
  {
    jukebox.PlayMusic(ActiveMap()->ReadMusicPlaylist());

    isGameLaunched = true;

    Run();

    isGameLaunched = false;

    MSG_DEBUG( "game", "End of game_loop.Run()" );
    jukebox.StopAll();

    UnloadDatas();  

    Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
    jukebox.PlayMusic("menu");

  }
  catch (const std::exception &e)
  {
    Question question;
    std::string err_msg = e.what();
    std::string txt = Format(_("Error:\n%s"), err_msg.c_str());
    std::cout << std::endl << txt << std::endl;
    question.Set (txt, true, 0);
    Time::GetInstance()->Pause();
    question.Ask();
    Time::GetInstance()->Continue();
  }

}

void Game::UnloadDatas() const
{
  world.FreeMem();
  lst_objects.FreeMem();
  ParticleEngine::Stop();
  GetTeamsList().UnloadGamingData();
  jukebox.StopAll();
}

bool Game::IsGameLaunched() const{
  return isGameLaunched;
}

// ####################################################################

uint Game::last_unique_id = 0;

void Game::ResetUniqueIds()
{
  last_unique_id = 0;
}

std::string Game::GetUniqueId()
{
  char buffer[16];
  snprintf(buffer, 16, "%#x", last_unique_id);
  last_unique_id++;
  return std::string(buffer);
}

// ####################################################################


Game::Game():
  isGameLaunched(false),
  want_end_of_game(false),
  state(PLAYING),
  pause_seconde(0),
  duration(0),
  current_ObjBox(NULL),
  give_objbox(true),
  fps(new FramePerSecond()),
  delay(0),
  time_of_next_frame(0),
  time_of_next_phy_frame(0),
  character_already_chosen(false)
{ }

Game::~Game()
{
  if(fps)
    delete fps;
}

void Game::Init()
{
  ResetUniqueIds();

  chatsession.Clear();
  fps->Reset();
  IgnorePendingInputEvents();
  Camera::GetInstance()->Reset();

  ActionHandler::GetInstance()->ExecActions();

  FOR_ALL_CHARACTERS(team, character)
    (*character).ResetDamageStats();

  SetState(END_TURN, true); // begin with a small pause
}

// ####################################################################
// ####################################################################

// ignore all pending events
// useful after loading screen
void Game::IgnorePendingInputEvents() const
{
  SDL_Event event;
  while(SDL_PollEvent(&event));
}

void Game::RefreshInput()
{
  // Poll and treat keyboard and mouse events
  SDL_Event event;
  bool refresh_joystick =  Joystick::GetInstance()->GetNumberOfJoystick() > 0;
  while(SDL_PollEvent(&event)) {
    if ( event.type == SDL_QUIT) {
      std::cout << "SDL_QUIT received ===> exit TODO" << std::endl;
      UserWantEndOfGame();
      std::cout << _("END OF GAME") << std::endl;
      return;
    }

    // Mouse event
    if (Mouse::GetInstance()->HandleClic(event))
      continue;

    // Keyboard event
    Keyboard::GetInstance()->HandleKeyEvent(event);
    // Joystick event
    if(refresh_joystick && !Config::GetInstance()->IsJoystickDisable())
      Joystick::GetInstance()->HandleKeyEvent(event);
  }

  // Keyboard, Joystick and mouse refresh
  if(!Config::GetInstance()->IsMouseDisable())
    Mouse::GetInstance()->Refresh();
  Keyboard::GetInstance()->Refresh();
  if(refresh_joystick && !Config::GetInstance()->IsJoystickDisable())
    Joystick::GetInstance()->Refresh();
  AIengine::GetInstance()->Refresh();

  // Execute action
  do {
    ActionHandler::GetInstance()->ExecActions();
    if(Network::GetInstance()->sync_lock) SDL_Delay(SDL_TIMESLICE);
  } while(Network::GetInstance()->sync_lock);

  GameMessages::GetInstance()->Refresh();

  if (!IsGameFinished())
    Camera::GetInstance()->Refresh();
}

// ####################################################################
// ####################################################################

void Game::RefreshObject() const
{
  FOR_ALL_CHARACTERS(team,character)
    character->Refresh();

  // Recompute energy of each team
  FOR_EACH_TEAM(team)
    (**team).Refresh();
  GetTeamsList().RefreshEnergy();

  ActiveTeam().AccessWeapon().Manage();
  lst_objects.Refresh();
  ParticleEngine::Refresh();
  CharacterCursor::GetInstance()->Refresh();
}

void Game::Draw ()
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
  fps->Draw();
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
  fps->AddOneFrame();

  // Draw the mouse pointer
  StatStart("GameDraw:mouse_pointer");
  Mouse::GetInstance()->Draw();
  StatStart("GameDraw:mouse_pointer");
}

void Game::CallDraw()
{
  Draw();
  StatStart("GameDraw:flip()");
  AppWormux::GetInstance()->video->Flip();
  StatStop("GameDraw:flip()");
}

void Game::PingClient() const
{
  Action * a = new Action(Action::ACTION_NETWORK_PING);
  ActionHandler::GetInstance()->NewAction(a);
}

// ####################################################################
// ####################################################################

void Game::Run()
{
  // Time to wait between 2 loops
  delay = 0;
  // Time to display the next frame
  time_of_next_frame = 0;
  // Time to display the compute next physic engine frame
  time_of_next_phy_frame = 0;

  want_end_of_game = false;

  // loop until game is finished
  do
  {
    MainLoop();
    if (want_end_of_game)
      if ((want_end_of_game = AskQuit()))
        break;

//     if (Time::GetInstance()->IsGamePaused())
//       DisplayPause();

  } while(!IsGameFinished());

  // the game is finished but we won't go at the results screen to fast!
  if (IsGameFinished()) {
    EndOfGame();
  }
  // * When debug is disabled : only show the result menu if game
  // have 'regularly' finished (only one survivor or timeout reached)
  // * When debug is disabled : still show the result menu everytime the game
  // is quit during local games (so we can still the result menu often).
  // For network game only show the result if the game is regularly finished
  // (elsewise when someone if someone quit the game before the end, it appears
  // as disconnected only when if finnishes viewing the f*cking result menu)
#ifndef DEBUG
  if (IsGameFinished())
#else
  if (IsGameFinished() || Network::GetInstance()->IsLocal())
#endif
    MessageEndOfGame();

}

void Game::MessageEndOfGame() const
{
  std::vector<TeamResults*>* results_list = TeamResults::createAllResults();

  Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
  ResultsMenu menu(*results_list);
  menu.Run();

  TeamResults::deleteAllResults(results_list);
}


void Game::EndOfGame()
{
  Network::GetInstance()->SetTurnMaster(true);
  SetState(END_TURN);
  duration = GameMode::GetInstance()->duration_exchange_player + 2;
  GameMessages::GetInstance()->Add (_("And the winner is..."));

  while (duration >= 1 ) {
    MainLoop();
  }
}

void Game::MainLoop()
{
  // Refresh clock value
  RefreshClock();
  time_of_next_phy_frame = Time::GetInstance()->Read() + Time::GetInstance()->GetDelta();

  if(Time::GetInstance()->Read() % 1000 == 20 && Network::GetInstance()->IsServer())
    PingClient();
  StatStart("Game:RefreshInput()");
  RefreshInput();
  StatStop("Game:RefreshInput()");
  StatStart("Game:RefreshObject()");
  RefreshObject();
  StatStop("Game:RefreshObject()");

  // Refresh the map
  world.Refresh();

  // try to adjust to max Frame by seconds
#ifndef USE_VALGRIND
  if (time_of_next_frame < Time::GetInstance()->ReadRealTime()) {
    // Only display if the physic engine isn't late
    if (time_of_next_phy_frame > Time::GetInstance()->ReadRealTime())
    {
#endif
      StatStart("Game:Draw()");
      CallDraw();
      // How many frame by seconds ?
      fps->Refresh();
      StatStop("Game:Draw()");
      time_of_next_frame += AppWormux::GetInstance()->video->GetMaxDelay();
#ifndef USE_VALGRIND
    }
  }
#endif

  delay = time_of_next_phy_frame - Time::GetInstance()->ReadRealTime();
  if (delay >= 0)
    SDL_Delay(delay);
}

void Game::RefreshClock()
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
	  if (duration <= 5) {
	    jukebox.Play("share", "time/bip");
	  }
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

          if (IsGameFinished()) {
            duration--;
            break;
          }

          if (Network::GetInstance()->IsTurnMaster() && give_objbox && world.IsOpen()) {
            NewBox();
            give_objbox = false;
            break;
          }
          else {
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

bool Game::NewBox()
{
  uint nbr_teams = GetTeamsList().playing_list.size();
  if(nbr_teams <= 1) {
    MSG_DEBUG("box", "There is less than 2 teams in the game");
    return false;
  }
  // .7 is a magic number to get the probability of boxes falling once every round close to .333
  double randValue = Random::GetDouble();
  if(randValue > (1 - pow(.5, 1.0 / nbr_teams))) {
    return false;
  }

  // Type of box : 1 = MedKit, 2 = Bonus Box.
  ObjBox * box;
  int type;
  if(Random::GetBool()) {
    box = new Medkit();
    type = 1;
  } else {
    box = new BonusBox();
    type = 2;
  }
  // Randomize contain
  box->Randomize();
  // Storing value of bonus box and send it over network.
  Action * a = new Action(Action::ACTION_NEW_BONUS_BOX);
  a->Push(type);
  if(!box->PutRandomly(true, 0, false)) {
    MSG_DEBUG("box", "Missed to put a box");
    delete box;
  } else {
    /* We only randomize value. The real box will be inserted into world later
       using action handling (see include/action_handler.cpp */
    box->StoreValue(a);
    ActionHandler::GetInstance()->NewAction(a);
    delete box; 
    return true;
  }
  return false;
}

uint Game::GetRemainingTime() const
{
  return duration;
}

void Game::AddNewBox(ObjBox * box)
{
  lst_objects.AddObject(box);
  Camera::GetInstance()->FollowObject(box, true);
  GameMessages::GetInstance()->Add(_("It's a present!"));
  SetCurrentBox(box);
}

// Begining of a new turn
void Game::__SetState_PLAYING()
{
  MSG_DEBUG("game.statechange", "Playing" );

  // initialize counter
  duration = GameMode::GetInstance()->duration_turn;
  Interface::GetInstance()->UpdateTimer(duration);
  Interface::GetInstance()->EnableDisplayTimer(true);
  pause_seconde = Time::GetInstance()->Read();

  if (Network::GetInstance()->IsTurnMaster() || Network::GetInstance()->IsLocal())
    wind.ChooseRandomVal();

  character_already_chosen = false;

  // Prepare each character for a new turn
  FOR_ALL_LIVING_CHARACTERS(team,character)
    character->PrepareTurn();

  // Select the next team
  ASSERT (!IsGameFinished());

  if (Network::GetInstance()->IsTurnMaster() || Network::GetInstance()->IsLocal())
    {
      GetTeamsList().NextTeam();

      if ( GameMode::GetInstance()->allow_character_selection==GameMode::CHANGE_ON_END_TURN
           || GameMode::GetInstance()->allow_character_selection==GameMode::BEFORE_FIRST_ACTION_AND_END_TURN)
        {
          ActiveTeam().NextCharacter();
        }

      Camera::GetInstance()->FollowObject (&ActiveCharacter(), true);

      if ( Network::GetInstance()->IsTurnMaster() )
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

      // Are we turn master for next turn ?
      if (ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI())
        Network::GetInstance()->SetTurnMaster(true);
      else
        Network::GetInstance()->SetTurnMaster(false);
    }

  give_objbox = true; //hack make it so no more than one objbox per turn
}

void Game::__SetState_HAS_PLAYED()
{
  MSG_DEBUG("game.statechange", "Has played, now can move");
  duration = GameMode::GetInstance()->duration_move_player;
  pause_seconde = Time::GetInstance()->Read();
  Interface::GetInstance()->UpdateTimer(duration);
  CharacterCursor::GetInstance()->Hide();
}

void Game::__SetState_END_TURN()
{
  MSG_DEBUG("game.statechange", "End of turn");
  ActiveTeam().AccessWeapon().SignalTurnEnd();
  ActiveTeam().AccessWeapon().Deselect();
  CharacterCursor::GetInstance()->Hide();
  duration = GameMode::GetInstance()->duration_exchange_player;
  Interface::GetInstance()->UpdateTimer(duration);
  Interface::GetInstance()->EnableDisplayTimer(false);
  pause_seconde = Time::GetInstance()->Read();

  // Applying Disease damage and Death mode.
  ApplyDiseaseDamage();
  ApplyDeathMode();
}

void Game::Really_SetState(game_loop_state_t new_state)
{
  // already in good state, nothing to do
  if (state == new_state) return;
  state = new_state;

  Interface::GetInstance()->weapons_menu.Hide();

  switch (state)
  {
  // Begining of a new turn:
  case PLAYING:
    __SetState_PLAYING();
    break;

  // The character have shooted, but can still move
  case HAS_PLAYED:
    __SetState_HAS_PLAYED();
    break;

  // Little pause at the end of the turn
  case END_TURN:
    __SetState_END_TURN();
    break;
  }
}

void Game::SetState(game_loop_state_t new_state, bool begin_game) const
{
  if (begin_game && 
      (Network::GetInstance()->IsServer() || Network::GetInstance()->IsLocal()))
    Network::GetInstance()->SetTurnMaster(true);

  if (!Network::GetInstance()->IsTurnMaster())
    return;

  // already in good state, nothing to do
  if ((state == new_state) && !begin_game) return;

  // Send information about energy and position of every characters
  if (Network::GetInstance()->IsTurnMaster())
    SyncCharacters();

  Action *a = new Action(Action::ACTION_GAMELOOP_SET_STATE);
  int seed = randomSync.GetRand();
  a->Push(seed);
  a->Push(new_state);
  ActionHandler::GetInstance()->NewAction(a);
}

PhysicalObj* Game::GetMovingObject() const
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

bool Game::IsAnythingMoving() const
{
  // Is the weapon still active or an object still moving ??
  if (ActiveTeam().GetWeapon().IsInUse())
  {
    MSG_DEBUG("game.endofturn", "Weapon %s is still active", ActiveTeam().GetWeapon().GetName().c_str());
    return true;
  }

  if (GetMovingObject() != NULL)
    return true;

  return false;
}

// Signal death of a character
void Game::SignalCharacterDeath (const Character *character) const
{
  std::string txt;

  ASSERT(IsGameLaunched());

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
    txt = Format(_("%s is a psychopath, he has killed a member of the %s team!"),
                 ActiveCharacter().GetName().c_str(), character->GetTeam().GetName().c_str());
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
  if (character->IsActiveCharacter() || IsGameFinished())
    SetState(END_TURN);
}

// Signal falling or any kind of damage of a character
void Game::SignalCharacterDamage(const Character *character) const
{
  if (character->IsActiveCharacter())
    SetState(END_TURN);
}

// Apply Disease damage
void Game::ApplyDiseaseDamage() const
{
  FOR_ALL_LIVING_CHARACTERS(team, character) {
    if (character->IsDiseased()) {
      character->SetEnergyDelta(-(int)character->GetDiseaseDamage());
      character->DecDiseaseDuration();
    }
  }
}

// Reduce energy of each character if we are in death mode
void Game::ApplyDeathMode () const
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
        // Don't report damage to the active character, it's not the responsible for this damage
        character->SetEnergyDelta(-(int)GameMode::GetInstance()->damage_per_turn_during_death_mode, false);
      else
        character->SetEnergy(1);
    }
  }
}

bool Game::IsGameFinished() const
{
  return (NbrRemainingTeams() <= 1);
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

bool Game::AskQuit() const
{
//   Question question;
//   const char *msg = _("Do you really want to quit? (Y/N)");
//   question.Set (msg, true, 0, "interface/quit_screen");

//   {
//     /* Tiny fix by Zygmunt Krynicki <zyga@zyga.dyndns.org> */
//     /* Let's find out what the user would like to press ... */
//     const char *key_x_ptr = strchr (msg, '/');
//     char key_x;
//     if (key_x_ptr && key_x_ptr > msg) /* it's there and it's not the first char */
//       key_x = tolower(key_x_ptr[-1]);
//     else
//       abort();
//     if (!isalpha(key_x)) /* sanity check */
//       abort();

//     question.add_choice(SDLK_a + (int)key_x - 'a', 1);
//   }
  jukebox.Pause();
  Time::GetInstance()->Pause();

  bool exit = false;
  PauseMenu menu(exit);
  menu.Run();

  //bool exit = (question.Ask() == 1);

  Time::GetInstance()->Continue();
  jukebox.Resume();

  return exit;
}

void Game::DisplayPause() const
{
  Question question;
  if(!Network::GetInstance()->IsLocal())
    return;

  // Pause screen
  question.Set("", false, 0, "interface/pause_screen");
  question.add_choice(Keyboard::GetInstance()->GetKeyAssociatedToAction(ManMachineInterface::KEY_PAUSE), 1);
  question.add_choice(Keyboard::GetInstance()->GetKeyAssociatedToAction(ManMachineInterface::KEY_QUIT), 1);

  jukebox.Pause();
  Time::GetInstance()->Pause();
  question.Ask();
  Time::GetInstance()->Continue();
  jukebox.Resume();
}


