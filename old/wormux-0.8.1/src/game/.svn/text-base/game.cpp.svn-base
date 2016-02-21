/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
#include "ai/ai_engine.h"
#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_classic.h"
#include "game/game_blitz.h"
#include "game/time.h"
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
#include "map/camera.h"
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

std::string Game::current_mode_name = "none";

Game * Game::GetInstance()
{
  if (singleton == NULL)
  {
    if (current_mode_name == "none")
      current_mode_name = Config::GetInstance()->GetGameMode();

    if (current_mode_name == "classic" || current_mode_name == "unlimited")
      singleton = new GameClassic();
    else if (current_mode_name == "blitz")
    {
      //printf(">>>> Starting in blitz!\n");
      singleton = new GameBlitz();
    }
    else
    {
      fprintf(stderr, "%s game mode not implemented\n", current_mode_name.c_str());
      exit(1);
    }
  }
  return singleton;
}

Game * Game::UpdateGameMode()
{
  const std::string& config_mode_name = Config::GetInstance()->GetGameMode();
  printf("Current mode: %s\n", config_mode_name.c_str());
  if (singleton != NULL && current_mode_name != config_mode_name)
  {
    printf("Mode change! -> %s %s\n", config_mode_name.c_str(), current_mode_name.c_str());
    delete singleton;
  }

  current_mode_name = config_mode_name;
  return GetInstance();
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
    JukeBox::GetInstance()->PlayMusic(ActiveMap()->ReadMusicPlaylist());

    bool game_finished = Run();

    MSG_DEBUG( "game", "End of game_loop.Run()" );
    JukeBox::GetInstance()->StopAll();

    UnloadDatas(game_finished);

    Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
    JukeBox::GetInstance()->PlayMusic("menu");

  }
  catch (const std::exception &e)
  {
    Question question(Question::WARNING);
    std::string err_msg = e.what();
    std::string txt = Format(_("Error:\n%s"), err_msg.c_str());
    std::cout << std::endl << txt << std::endl;
    question.Set (txt, true, 0);
    Time::GetInstance()->Pause();
    question.Ask();
    Time::GetInstance()->Continue();
  }

}

void Game::UnloadDatas(bool game_finished) const
{
  world.FreeMem();
  ActiveMap()->FreeData();
  lst_objects.FreeMem();
  ParticleEngine::Stop();

  if (!Network::IsConnected() || !game_finished) {
    // Fix bug #10613: ensure all teams are reseted as local teams
    FOR_EACH_TEAM(team)
      (**team).SetDefaultPlayingConfig();
  }

  if (Network::IsConnected()) {
    if (!game_finished) {
      // the user has asked for the end of game
      // if it's a network game, it's time to disconnect!!
      Network::Disconnect();

      // Fix bug #10613: ensure all teams are reseted as local teams
      FOR_EACH_TEAM(team)
	(**team).SetDefaultPlayingConfig();
    }
    // else: we will start a new round!
  } else {

    // Fix bug #10613: ensure all teams are reseted as local teams
    FOR_EACH_TEAM(team)
      (**team).SetDefaultPlayingConfig();
  }

  GetTeamsList().UnloadGamingData();

  JukeBox::GetInstance()->StopAll();
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
  state(PLAYING),
  give_objbox(true),
  pause_seconde(0),
  isGameLaunched(false),
  current_ObjBox(NULL),
  ask_for_menu(false),
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
  while(SDL_PollEvent(&event)) { ; }
}

void Game::RefreshInput()
{
  // Poll and treat keyboard and mouse events
  SDL_Event event;
  bool refresh_joystick =  Joystick::GetInstance()->GetNumberOfJoystick() > 0;
  while(SDL_PollEvent(&event)) {
    if ( event.type == SDL_QUIT) {
      std::cout << "SDL_QUIT received ===> exit TODO" << std::endl;
      UserAsksForMenu();
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
    if (Network::GetInstance()->sync_lock) SDL_Delay(SDL_TIMESLICE);
  } while (Network::GetInstance()->sync_lock &&
	   !HasBeenNetworkDisconnected());

  Network::GetInstance()->sync_lock = false;

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

  // Draw objects
  StatStart("GameDraw:objects");
  lst_objects.Draw();
  ParticleEngine::Draw(true);
  StatStart("GameDraw:objects");

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

  // Draw the interface (current team information, weapon ammo)
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

bool Game::Run()
{
  bool game_finished = false;
  isGameLaunched = true;

  // Time to wait between 2 loops
  delay = 0;
  // Time to display the next frame
  time_of_next_frame = 0;
  // Time to display the compute next physic engine frame
  time_of_next_phy_frame = 0;

  // loop until game is finished
  do
    {
      ask_for_menu = false;
      MainLoop();

      if (ask_for_menu && MenuQuitPause())
        break;

  } while(!IsGameFinished());

  // the game is finished but we won't go at the results screen too fast!
  if (IsGameFinished()) {
    EndOfGame();
    game_finished = true;
  }

  isGameLaunched = false;

  // * When debug is disabled : only show the result menu if game
  //   have 'regularly' finished (only one survivor or timeout reached)
  // * When debug is disabled : still show the result menu everytime the game
  //   is quit during local games (so we can still the result menu often).
  // For network game only, show the result if the game is regularly finished
  // (elsewise when someone quits the game before the end, it appears
  // as disconnected only when if finishes viewing the result menu)
#ifndef DEBUG
  if (game_finished)
#else
  if (game_finished || Network::GetInstance()->IsLocal())
#endif
    MessageEndOfGame();

  return game_finished;
}

bool Game::HasBeenNetworkDisconnected() const
{
  const Network* net          = Network::GetInstance();
  return !net->IsLocal() && net->cpu.empty();
}

void Game::MessageEndOfGame() const
{
  bool disconnected = HasBeenNetworkDisconnected();

  if (disconnected)
  {
    Question question(Question::WARNING);
    question.Set(_("The game was interrupted because you got disconnected."), true, 0);
    question.Ask();
  }

  Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);

  std::vector<TeamResults*>* results_list = TeamResults::createAllResults();
  ResultsMenu menu(*results_list, disconnected);
  menu.Run();

  TeamResults::deleteAllResults(results_list);
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

bool Game::NewBox()
{
  uint nbr_teams = GetTeamsList().playing_list.size();
  if(nbr_teams <= 1) {
    MSG_DEBUG("box", "There is less than 2 teams in the game");
    return false;
  }

  // if started with "-d box", get one box per turn
  if (!IsLOGGING("box")) {
    // .7 is a magic number to get the probability of boxes falling once every round close to .333
    double randValue = RandomLocal().GetDouble();
    if(randValue > (1 - pow(.5, 1.0 / nbr_teams))) {
      return false;
    }
  }

  // Type of box : 1 = MedKit, 2 = Bonus Box.
  ObjBox * box;
  int type;
  if(RandomLocal().GetBool()) {
    box = new Medkit();
    type = 1;
  } else {
    box = new BonusBox();
    type = 2;
  }
  // Randomize container
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

void Game::AddNewBox(ObjBox * box)
{
  lst_objects.AddObject(box);
  Camera::GetInstance()->FollowObject(box, true, true);
  GameMessages::GetInstance()->Add(_("It's a present!"));
  SetCurrentBox(box);
}


void Game::Really_SetState(game_loop_state_t new_state)
{
  // already in good state, nothing to do
  if (state == new_state) return;
  state = new_state;

  Interface::GetInstance()->weapons_menu.Hide();

  switch (state)
  {
  // Beginning of a new turn:
  case PLAYING:
    __SetState_PLAYING();
    break;

  // The character has shot, but can still move
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
  // ONLY at the beginning of a new turn!
  // (else you can send unstable information of a character which is moving)
  // See bug #10668
  if (Network::GetInstance()->IsTurnMaster() && new_state == PLAYING)
    SyncCharacters();

  MSG_DEBUG("game", "Ask for state %d", new_state);

  Action *a = new Action(Action::ACTION_GAMELOOP_SET_STATE);
  int seed = RandomSync().GetRand();
  a->Push(seed);
  a->Push(new_state);
  ActionHandler::GetInstance()->NewAction(a);
}

PhysicalObj* Game::GetMovingObject() const
{
  if (!ActiveCharacter().IsImmobile())
  {
    MSG_DEBUG("game.endofturn", "Active character (%s) is not ready", ActiveCharacter().GetName().c_str());
    return &ActiveCharacter();
  }

  FOR_ALL_CHARACTERS(team,character)
  {
    if (!character->IsImmobile() && !character->IsGhost())
    {
      MSG_DEBUG("game.endofturn", "Character (%s) is not ready", character->GetName().c_str());
      return &(*character);
    }
  }

  FOR_EACH_OBJECT(object)
  {
    if (!(*object)->IsImmobile())
    {
      MSG_DEBUG("game.endofturn", "Object (%s) is moving", (*object)->GetName().c_str());
      return (*object);
    }
  }

  PhysicalObj *obj = ParticleEngine::IsSomethingMoving();
  if (obj != NULL)
    {
      MSG_DEBUG("game.endofturn", "ParticleEngine (%s) is moving", obj->GetName().c_str());
      return obj;
    }
  return NULL;
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
  if (character->IsGhost()) {
    txt = Format(_("%s from %s team has fallen off the map!"),
                 character->GetName().c_str(),
                 character->GetTeam().GetName().c_str());
    JukeBox::GetInstance()->Play(ActiveTeam().GetSoundProfile(), "out");

  } else if (character->IsDrowned() ) {
    txt = Format(_("%s from %s team has fallen into the water!"),
                 character->GetName().c_str(),
                 character->GetTeam().GetName().c_str());

  } else if (&ActiveCharacter() == character) { // Active Character is dead
    CharacterCursor::GetInstance()->Hide();

    // Is this a suicide ?
    if (ActiveTeam().GetWeaponType() == Weapon::WEAPON_SUICIDE) {
      txt = Format(_("%s from %s team commited suicide!"),
                   character->GetName().c_str(),
                   character->GetTeam().GetName().c_str());

      // Dead while moving ?
    } else if (state == PLAYING) {
      txt = Format(_("%s from %s team has hit the ground too hard!"),
                   character->GetName().c_str(),
                   character->GetTeam().GetName().c_str());
       JukeBox::GetInstance()->Play(ActiveTeam().GetSoundProfile(), "out");

      // The playing character killed himself
    } else {
      txt = Format(_("%s from %s team is dead because he is clumsy!"),
                   character->GetName().c_str(),
                   character->GetTeam().GetName().c_str());
    }
  }
  // Did the active player kill someone of his own team ?
  else if ( character->GetTeam().IsSameAs(ActiveTeam()) ) {
    if (ActiveCharacter().IsDead()) {
      txt = Format(_("%s took a member of the %s team to the grave with him!"),
                   ActiveCharacter().GetName().c_str(),
                   character->GetTeam().GetName().c_str());
    } else {
      txt = Format(_("%s is a psychopath, he has killed a member of the %s team!"),
                   ActiveCharacter().GetName().c_str(),
                   character->GetTeam().GetName().c_str());
    }
  } else if (ActiveTeam().GetWeaponType() == Weapon::WEAPON_GUN) {
    txt = Format(_("What a shame for %s - he was killed by a simple gun!"),
                 character->GetName().c_str());
  } else {
    txt = Format(_("%s from %s team has died."),
                 character->GetName().c_str(),
                 character->GetTeam().GetName().c_str());
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
  MSG_DEBUG("game.endofturn", "%s has been hurt", character->GetName().c_str());

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

int Game::NbrRemainingTeams() const
{
  uint nbr = 0;

  FOR_EACH_TEAM(team){
    if( (**team).NbAliveCharacter() > 0 )
      nbr++;
  }

  return nbr;
}

bool Game::MenuQuitPause() const
{
  JukeBox::GetInstance()->Pause();

  if (!Network::IsConnected()) // partial bugfix of #10679
    Time::GetInstance()->Pause();

  bool exit = false;
  PauseMenu menu(exit);
  menu.Run();

  if (!Network::IsConnected()) // partial bugfix of #10679
    Time::GetInstance()->Continue();

  JukeBox::GetInstance()->Resume();

  return exit;
}


