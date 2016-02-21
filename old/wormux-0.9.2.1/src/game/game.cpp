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
 * Init the game, handle drawing and states of the game.
 *****************************************************************************/
#include <iostream>
#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/game_classic.h"
#include "game/game_blitz.h"
#include "game/time.h"
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
#include "interface/loading_screen.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "menu/pause_menu.h"
#include "menu/results_menu.h"
#include "menu/network_menu.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "network/network_server.h"
#include "object/objbox.h"
#include "object/bonus_box.h"
#include "object/medkit.h"
#include "object/objects_list.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/results.h"
#include <WORMUX_random.h>
#include "tool/stats.h"
#include "weapon/weapons_list.h"

#ifdef DEBUG
// Uncomment this to get an image during the game under Valgrind
// DON'T USE THIS IF YOU INTEND TO PLAY NETWORKED GAMES!
//#define USE_VALGRIND
#endif

const uint MAX_WAIT_TIME_WITHOUT_MESSAGE_IN_MS = 500;

std::string Game::current_rules = "none";

Game * Game::GetInstance()
{
  if (singleton == NULL) {

    current_rules = GameMode::GetRef().rules;

    if (GameMode::GetRef().rules == "blitz")
      singleton = new GameBlitz();
    else if (GameMode::GetRef().rules == "classic")
      singleton = new GameClassic();
    else {
      fprintf(stderr, "%s game rules not implemented\n", GameMode::GetRef().rules.c_str());
      exit(1);
    }
  }
  return singleton;
}

bool Game::IsRunning()
{
  if (!singleton)
    return false;

  return (singleton->IsGameLaunched());
}

Game * Game::UpdateGameRules()
{
  const std::string& config_rules = GameMode::GetRef().rules;
  printf("Current rules: %s\n", config_rules.c_str());
  if (singleton != NULL && current_rules != config_rules)
  {
    printf("Rules change! %s -> %s\n", current_rules.c_str(), config_rules.c_str());
    delete singleton;
  }

  return GetInstance();
}


void Game::InitEverything()
{
  int icon_count = Network::GetInstance()->IsLocal() ? 4 : 5;
  LoadingScreen loading_sreen(icon_count);

  Config::GetInstance()->RemoveAllObjectConfigs();

  // Disable sound during the loading of data
  bool enable_sound = JukeBox::GetInstance()->UseEffects();
  JukeBox::GetInstance()->ActiveEffects(false);

  Mouse::GetInstance()->Hide();

  std::cout << "o " << _("Initialisation") << std::endl;
  Time::GetInstance()->Reset();

  // initialize gaming data
  if (Network::GetInstance()->IsGameMaster())
    InitGameData_NetGameMaster();
  else if (Network::GetInstance()->IsLocal())
    RandomSync().InitRandom();

  // GameMode::GetInstance()->Load(); : done in the game menu to adjust some parameters for local games
  // done in action_handler for clients


  // Camera must not shake as the started shaking time could be from a previous game:
  Camera::GetInstance()->ResetShake();

  // Load the map
  std::cout << "o " << _("Initialise map") << std::endl;
  loading_sreen.StartLoading(1, "map_icon", _("Maps"));
  InitMap();

  loading_sreen.StartLoading(2, "weapon_icon", _("Weapons"));
  weapons_list = new WeaponsList(GameMode::GetInstance()->GetWeaponsXml());

  std::cout << "o " << _("Initialise teams") << std::endl;
  loading_sreen.StartLoading(3, "team_icon", _("Teams"));
  InitTeams();

  std::cout << "o " << _("Initialise sounds") << std::endl;
  // Load teams' sound profiles
  loading_sreen.StartLoading(4, "sound_icon", _("Sounds"));
  InitSounds();

  InitInterface();

  // Loading is finished, sound effects can be enabled again
  JukeBox::GetInstance()->ActiveEffects(enable_sound);

  // Waiting for others players
  if (!Network::GetInstance()->IsLocal()) {
    std::cout << "o " << _("Waiting for remote players") << std::endl;
    loading_sreen.StartLoading(5, "network_icon", _("Network"));
    WaitForOtherPlayers();
  }
  ActionHandler::GetInstance()->ExecFrameLessActions();
  ResetUniqueIds();

  fps->Reset();
  IgnorePendingInputEvents();

  ActionHandler::GetInstance()->ExecFrameLessActions();

  m_current_turn = 0;


  SetState(END_TURN, true); // begin with a small pause

  // Reset time at end of initialisation, so that the first player doesn't loose a few seconds.
  Time::GetInstance()->Reset();

  std::cout << std::endl;
  std::cout << "[ " << _("Starting a new game") << " ]" << std::endl;
}

void Game::InitGameData_NetGameMaster()
{
  if (Network::GetInstance()->IsServer()) {
    Network::GetInstanceServer()->RejectIncoming();
  }

  RandomSync().InitRandom();

  SendGameMode();

  Network::GetInstance()->SetState(WNet::NETWORK_LOADING_DATA);
  Network::GetInstance()->SendNetworkState();
}

void Game::EndInitGameData_NetGameMaster()
{
  // Wait for all clients to be ready to play

  // Note that the loop also ends when there is no connected player.
  // That's important if we are connected to a dedicated server.
  while (Network::IsConnected()
         && Network::GetInstance()->GetNbPlayersWithState(Player::STATE_READY) != Network::GetInstance()->GetNbPlayersConnected()) {

    ActionHandler::GetInstance()->ExecFrameLessActions();
    SDL_Delay(200);
  }

  // Before playing we should check that init phase happens correctly on all clients
  Action a(Action::ACTION_NETWORK_CHECK_PHASE1);
  Network::GetInstance()->SendActionToAll(a);

  // Note that the loop also ends when there is no connected player.
  // That's important if we are connected to a dedicated server.
  while (Network::IsConnected()
         && Network::GetInstance()->GetNbPlayersWithState(Player::STATE_CHECKED) != Network::GetInstance()->GetNbPlayersConnected()) {

    ActionHandler::GetInstance()->ExecFrameLessActions();
    SDL_Delay(200);
  }

  // Let's play !
  Network::GetInstance()->SetState(WNet::NETWORK_PLAYING);
  Network::GetInstance()->SendNetworkState();
}

void Game::EndInitGameData_NetClient()
{
  // Tells server that client is ready
  Network::GetInstance()->SetState(WNet::NETWORK_READY_TO_PLAY);
  Network::GetInstance()->SendNetworkState();

  // Waiting for other clients
  std::cout << Network::GetInstance()->GetState() << " : Waiting for people over the network" << std::endl;

  while (Network::IsConnected()
	 && !Network::GetInstance()->IsGameMaster()
	 && Network::GetInstance()->GetState() == WNet::NETWORK_READY_TO_PLAY)
  {
    ActionHandler::GetInstance()->ExecFrameLessActions();
    SDL_Delay(100);
  }
}

void Game::InitMap()
{
  GetWorld().Reset();
  ObjectsList::GetRef().PlaceBarrels();
}

void Game::InitTeams()
{
  // Check the number of teams
  if (GetTeamsList().playing_list.size() < 2)
    Error(_("You need at least two valid teams!"));
  ASSERT (GetTeamsList().playing_list.size() <= GameMode::GetInstance()->max_teams);

  // Load the teams
  ASSERT(weapons_list); // weapons must be initialized before the teams
  GetTeamsList().LoadGamingData(weapons_list);

  GetTeamsList().InitEnergy();

  // Randomize first player
  GetTeamsList().RandomizeFirstPlayer();

  // First "selection" of a weapon -> fix bug 6576
  ActiveTeam().AccessWeapon().Select();

  FOR_ALL_CHARACTERS(team, character)
    (*character).ResetDamageStats();

  ObjectsList::GetRef().PlaceMines();
}

void Game::InitSounds()
{
  FOR_EACH_TEAM(team)
    if ( (**team).GetSoundProfile() != "default" )
      JukeBox::GetInstance()->LoadXML((**team).GetSoundProfile()) ;
}

void Game::InitInterface()
{
  CharacterCursor::GetInstance()->Reset();
  Keyboard::GetInstance()->Reset();

  Interface::GetInstance()->Reset();
  GameMessages::GetInstance()->Reset();

  ParticleEngine::Load();

  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
  Mouse::GetInstance()->CenterPointer();
  chatsession.Clear();
  Camera::GetInstance()->Reset();
}

void Game::WaitForOtherPlayers()
{
  if (Network::GetInstance()->IsGameMaster()) {
    EndInitGameData_NetGameMaster();
  } else {
    EndInitGameData_NetClient();

    // We have been elected as game master (the previous one has been disconnected)
    if (Network::GetInstance()->IsGameMaster())
      EndInitGameData_NetGameMaster();
  }
}

void Game::DisplayError(const std::string &msg)
{
  JukeBox::GetInstance()->Play("default", "menu/error");

  std::cerr << msg << std::endl;

  Question question(Question::WARNING);
  question.Set(msg, true, 0);
  Time::GetInstance()->SetWaitingForUser(true);
  question.Ask();
  Time::GetInstance()->SetWaitingForUser(false);
}

void Game::Start()
{
  Keyboard::GetInstance()->Reset();
  Joystick::GetInstance()->Reset();

  try
  {
    InitEverything();

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
    // thanks to exception mechanism, cancel some things by hand...
    Mouse::GetInstance()->Show();

    std::string err_msg = e.what();
    std::string txt = Format(_("Error:\n%s"), err_msg.c_str());
    std::cout << std::endl << txt << std::endl;
    DisplayError(txt);
  }
}

void Game::UnloadDatas(bool game_finished) const
{
  GetWorld().FreeMem();
  ActiveMap()->FreeData();
  ObjectsList::GetRef().FreeMem();
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

bool Game::IsGameLaunched() const
{
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
  last_clock_update(0),
  isGameLaunched(false),
  current_ObjBox(NULL),
  ask_for_menu(false),
  fps(new FramePerSecond()),
  delay(0),
  time_of_next_frame(0),
  time_of_next_phy_frame(0),
  character_already_chosen(false),
  m_current_turn(0),
  waiting_for_network_text("Waiting for turn master"),
  weapons_list(NULL)
{ }

Game::~Game()
{
  if(fps)
    delete fps;
  if (weapons_list)
    delete weapons_list;
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
  while(SDL_PollEvent(&event)) {

    // Emergency exit
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE && (SDL_GetModState() & KMOD_CTRL))
      AppWormux::EmergencyExit();

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
    Joystick::GetInstance()->HandleKeyEvent(event);
  }

  // Keyboard, Joystick and mouse refresh
  Mouse::GetInstance()->Refresh();
  ActiveTeam().RefreshAI();
  GameMessages::GetInstance()->Refresh();

  if (!IsGameFinished())
    Camera::GetInstance()->Refresh();
}

// ####################################################################
// ####################################################################

bool Game::IsCharacterAlreadyChosen() const
{
  return character_already_chosen;
}

void Game::SetCharacterChosen(bool chosen)
{
  if (character_already_chosen == chosen)
    return;

  character_already_chosen = chosen;
  if (chosen) {
    CharacterCursor::GetInstance()->Hide();
  }
}

void Game::RefreshObject() const
{
  FOR_ALL_CHARACTERS(team,character)
    character->Refresh();

  // Recompute energy of each team
  FOR_EACH_TEAM(team)
    (**team).Refresh();
  GetTeamsList().RefreshEnergy();

  ActiveTeam().AccessWeapon().Manage();
  ObjectsList::GetRef().Refresh();
  ParticleEngine::Refresh();
  CharacterCursor::GetInstance()->Refresh();
}

void Game::Draw ()
{
  // Draw the sky
  StatStart("GameDraw:sky");
  GetWorld().DrawSky();
  StatStop("GameDraw:sky");

  // Draw the map
  StatStart("GameDraw:world");
  GetWorld().Draw();
  StatStop("GameDraw:world");

  // Draw objects
  StatStart("GameDraw:objects");
  ObjectsList::GetRef().Draw();
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
  GetWorld().DrawWater();
  StatStop("GameDraw:water");

  // Draw game messages
  StatStart("GameDraw::game_messages");
  GameMessages::GetInstance()->Draw();
  StatStop("GameDraw::game_messages");

  // Draw optionals
  StatStart("GameDraw:fps_and_map_author_name");
  GetWorld().DrawAuthorName();
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

  if (Time::GetInstance()->GetMSWaitingForNetwork() > MAX_WAIT_TIME_WITHOUT_MESSAGE_IN_MS) {
    Point2i pos;
    pos.x = GetMainWindow().GetWidth()/2;
    pos.y = GetMainWindow().GetHeight()/2;
    std::string text = Format(_("Waiting for %s"), ActiveTeam().GetPlayerName().c_str());
    waiting_for_network_text.SetText(text);
    waiting_for_network_text.DrawCenter(pos);
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

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  return game_finished;
}

bool Game::HasBeenNetworkDisconnected() const
{
  const Network* net          = Network::GetInstance();
  return !net->IsLocal() && (net->GetNbPlayersConnected() == 0);
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
  if (!Time::GetInstance()->IsWaitingForUser()) {
    // If we are waiting for the network then we have already done those steps.
    if (!Time::GetInstance()->IsWaitingForNetwork()) {
      Time::GetInstance()->Increase();

      // Refresh clock value
      RefreshClock();

      // For example the switch of characters can make it necessary to rebuild the body.
      // If no cacluate frame action is sheduled the frame calculation will be skipped and the bodies don't get build.
      // As the draw method needs builded characters we need to build here
      FOR_ALL_CHARACTERS(team,character) {
        character->GetBody()->Build();
        character->GetBody()->RefreshSprites();
      }


      if (Network::GetInstance()->IsTurnMaster()) {
        // The action which verifys the random seed must be the first action sheduled!
        // Otherwise the following could happen:
        // 1. Action C gets sheduled which draws values from the random source.
        // 2. Action V gets sheduled which verifies that random seed is X.
        // 3. Action C gets executed: As a result the random seed has changed to another value Y.
        // 4. Action V gets executed: It fails as the random seed is no longer X but Y.
        RandomSync().Verify();

#ifdef DEBUG
        Action* action = new Action(Action::ACTION_TIME_VERIFY_SYNC);
        action->Push((int)Time::GetInstance()->Read());
        ActionHandler::GetInstance()->NewAction(action);
#endif
      }

      if (Time::GetInstance()->Read() % 1000 == 20 && Network::GetInstance()->IsGameMaster())
        PingClient();
    }
    StatStart("Game:RefreshInput()");
    RefreshInput();
    StatStop("Game:RefreshInput()");
    ActionHandler::GetInstance()->ExecFrameLessActions();

    bool is_turn_master = Network::GetInstance()->IsTurnMaster();
    if (is_turn_master) {
      Time::GetInstance()->SetWaitingForNetwork(false);
      Action *a = new Action(Action::ACTION_GAME_CALCULATE_FRAME);
      ActionHandler::GetInstance()->NewAction(a);
    }
    bool actions_executed = ActionHandler::GetInstance()->ExecActionsForOneFrame();
    ASSERT(actions_executed || !is_turn_master);
    Time::GetInstance()->SetWaitingForNetwork(!actions_executed);

    if (actions_executed) {
      StatStart("Game:RefreshObject()");
      RefreshObject();
      StatStop("Game:RefreshObject()");

      // Refresh the map
      GetWorld().Refresh();

      // Build the characters if necessary so that it does not need to happen while drawing.
      // The build can become necessary again when for example weapons change the movement.
      FOR_ALL_CHARACTERS(team,character) {
        character->GetBody()->Build();
        character->GetBody()->RefreshSprites();
      }
    } else {
      SDL_Delay(1);
      // Do we wait for a player who has left?
      if (ActiveTeam().IsAbandoned()) {
        const std::string & team_id = ActiveTeam().GetId();
        GetTeamsList().DelTeam(team_id);
        if (Network::GetInstance()->network_menu != NULL)
          Network::GetInstance()->network_menu->DelTeamCallback(team_id);
      }
    }

  }

  // try to adjust to max Frame by seconds
  bool draw = time_of_next_frame < SDL_GetTicks();
  // Only display if the physic engine isn't late
  draw = draw && !(Time::GetInstance()->CanBeIncreased() && !Time::GetInstance()->IsWaiting());
#ifdef USE_VALGRIND
  draw = true;
#endif

  if (draw) {
    StatStart("Game:Draw()");
    CallDraw();
    // How many frame by seconds ?
    fps->Refresh();
    StatStop("Game:Draw()");
    uint frame_length =  AppWormux::GetInstance()->video->GetMaxDelay();
    time_of_next_frame = time_of_next_frame + frame_length;

    // The rate at which frames are calculated may differ over time.
    // This if statement assures that time_of_next_frame does not get to far behind
    // as else it would increase the game speed later.
    if (time_of_next_frame < SDL_GetTicks())
      time_of_next_frame = SDL_GetTicks();
  }
  if (!Time::GetInstance()->IsWaiting())
    Time::GetInstance()->LetRealTimePassUntilFrameEnd();
}

bool Game::NewBox()
{
  uint nbr_teams = GetTeamsList().playing_list.size();
  if (nbr_teams <= 1) {
    MSG_DEBUG("box", "There is less than 2 teams in the game");
    return false;
  }

  // if started with "-d box", get one box per turn
  if (!IsLOGGING("box") || Network::IsConnected()) {
    Double boxDropProbability = (1 - pow(.5, 1.0 / nbr_teams));
    MSG_DEBUG("random.get", "Game::NewBox(...) drop box?");
    Double randValue = RandomSync().GetDouble();
    if (randValue > boxDropProbability) {
      return false;
    }
  }

  // Type of box : 1 = MedKit, 2 = Bonus Box.
  ObjBox * box;
  int type;
  MSG_DEBUG("random.get", "Game::NewBox(...) box type?");
  if(RandomSync().GetBool()) {
    box = new Medkit();
    type = 1;
  } else {
    box = new BonusBox(weapons_list->GetRandomWeaponToDrop());
    type = 2;
  }
  // Randomize container
  box->Randomize();

  if (!box->PutRandomly(true, 0, true)) {
    MSG_DEBUG("box", "Missed to put a box");
    delete box;
    return false;
  }

  ObjectsList::GetRef().AddObject(box);
  Camera::GetInstance()->FollowObject(box);
  GameMessages::GetInstance()->Add(_("It's a present!"));
  SetCurrentBox(box);

  return true;
}

void Game::RequestBonusBoxDrop()
{
  ObjBox* current_box = Game::GetInstance()->GetCurrentBox();
  if (current_box != NULL) {
    if (Network::GetInstance()->IsTurnMaster()) {
      Action a(Action::ACTION_DROP_BONUS_BOX);
      Network::GetInstance()->SendActionToAll(a);

      current_box->DropBox();
    } else {
      Action a(Action::ACTION_REQUEST_BONUS_BOX_DROP);
      Network::GetInstance()->SendActionToAll(a);
    }
  }
}

void Game::SetState(game_loop_state_t new_state, bool begin_game)
{
  // already in good state, nothing to do
  if ((state == new_state) && !begin_game) return;

  MSG_DEBUG("game", "Ask for state %d", new_state);

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
    m_current_turn++;
    break;
  }
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
  if (ActiveTeam().GetWeapon().IsOnCooldownFromShot())
  {
    MSG_DEBUG("game.endofturn", "Weapon %s is still active", ActiveTeam().GetWeapon().GetName().c_str());
    return true;
  }

  if (GetMovingObject() != NULL)
    return true;
  return false;
}

// Signal death of a character
void Game::SignalCharacterDeath (const Character *character)
{
  std::string txt;

  ASSERT(IsGameLaunched());

  if (ActiveTeam().GetWeaponType() == Weapon::WEAPON_BASEBALL
      && &ActiveCharacter() != character) {
    txt = Format(_("What a beautiful homerun! %s from %s team is in another world..."),
                 character->GetName().c_str(),
                 character->GetTeam().GetName().c_str());

    JukeBox::GetInstance()->Play(ActiveTeam().GetSoundProfile(), "weapon/baseball_homerun");

  } else if (character->IsGhost()) {
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
void Game::SignalCharacterDamage(const Character *character)
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

  Time::GetInstance()->SetWaitingForUser(true);

  Action a(Action::ACTION_ANNOUNCE_PAUSE);
  Network::GetInstance()->SendActionToAll(a);

  bool exit = false;
  PauseMenu menu(exit);
  menu.Run();

  Time::GetInstance()->SetWaitingForUser(false);

  JukeBox::GetInstance()->Resume();

  return exit;
}

uint Game::GetCurrentTurn()
{
  uint nbr_teams = GetTeamsList().playing_list.size();
  return (m_current_turn+nbr_teams-1)/nbr_teams ;
}

void Game::UpdateTranslation()
{
  weapons_list->UpdateTranslation();
}
