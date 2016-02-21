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
 * Game loop : drawing and data handling
 *****************************************************************************/

#include "game/game_init.h"
#include <SDL.h>
#include <iostream>
#include "game/game.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "character/character.h"
#include "include/action_handler.h"
#include "interface/cursor.h"
#include "interface/game_msg.h"
#include "interface/interface.h"
#include "interface/keyboard.h"
#include "interface/mouse.h"
#include "game/config.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "network/network.h"
#include "network/network_server.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/i18n.h"

void GameInit::InitGameData_NetServer()
{
  Network::GetInstanceServer()->RejectIncoming();

  randomSync.Init();

  GameMode::GetInstance()->Load();
  SendGameMode();

  Network::GetInstance()->SetState(Network::NETWORK_LOADING_DATA);
  Network::GetInstance()->SendNetworkState();
}

void GameInit::EndInitGameData_NetServer()
{
  // Wait for all clients to be ready to play
  while (Network::IsConnected()
         && Network::GetInstanceServer()->GetNbReadyPlayers() + 1  != Network::GetInstanceServer()->GetNbConnectedPlayers())
  {
    ActionHandler::GetInstance()->ExecActions();
    SDL_Delay(200);
  }

  // Before playing we should check that init phase happens correctly on all clients
  Action a(Action::ACTION_NETWORK_CHECK_PHASE1);
  Network::GetInstance()->SendAction(&a);

  while (Network::IsConnected()
         && Network::GetInstanceServer()->GetNbCheckedPlayers() + 1  != Network::GetInstanceServer()->GetNbConnectedPlayers())
    {
      ActionHandler::GetInstance()->ExecActions();
      SDL_Delay(200);
    }

  // Let's play !
  Network::GetInstance()->SetState(Network::NETWORK_PLAYING);
  Network::GetInstance()->SendNetworkState();
}

void GameInit::EndInitGameData_NetClient()
{
  // Tells server that client is ready
  Network::GetInstance()->SetState(Network::NETWORK_READY_TO_PLAY);
  Network::GetInstance()->SendNetworkState();

  // Waiting for other clients
  std::cout << Network::GetInstance()->GetState() << " : Waiting for people over the network" << std::endl;
  while (Network::GetInstance()->GetState() == Network::NETWORK_READY_TO_PLAY
         && Network::IsConnected())
  {
    ActionHandler::GetInstance()->ExecActions();
    SDL_Delay(100);
  }
}

void GameInit::InitMap()
{
  std::cout << "o " << _("Initialise map") << std::endl;

  Camera::GetInstance()->ResetShake();
  loading_sreen.StartLoading(1, "map_icon", _("Maps"));
  world.Reset();
  MapsList::GetInstance()->ActiveMap()->FreeData();

  lst_objects.PlaceBarrels();
}

void GameInit::InitTeams()
{
  std::cout << "o " << _("Initialise teams") << std::endl;

  loading_sreen.StartLoading(2, "team_icon", _("Teams"));

  // Check the number of teams
  if (GetTeamsList().playing_list.size() < 2)
    Error(_("You need at least two valid teams !"));
  ASSERT (GetTeamsList().playing_list.size() <= GameMode::GetInstance()->max_teams);

  // Load the teams
  GetTeamsList().LoadGamingData();

  // Initialization of teams' energy
  loading_sreen.StartLoading(3, "weapon_icon", _("Weapons")); // use fake message...
  GetTeamsList().InitEnergy();

  // Randomize first player
  GetTeamsList().RandomizeFirstPlayer();

  lst_objects.PlaceMines();
}

void GameInit::InitSounds()
{
  std::cout << "o " << _("Initialise sounds") << std::endl;

  // Load teams' sound profiles
  loading_sreen.StartLoading(4, "sound_icon", _("Sounds"));

  JukeBox::GetInstance()->LoadXML("default");
  FOR_EACH_TEAM(team)
    if ( (**team).GetSoundProfile() != "default" )
      JukeBox::GetInstance()->LoadXML((**team).GetSoundProfile()) ;
}

void GameInit::InitData()
{
  std::cout << "o " << _("Initialisation") << std::endl;
  Time::GetInstance()->Reset();

  // initialize gaming data
  if (Network::GetInstance()->IsServer())
    InitGameData_NetServer();
  else if (Network::GetInstance()->IsLocal())
    randomSync.Init();

  // GameMode::GetInstance()->Load(); : done in the game menu to adjust some parameters for local games
  // done in action_handler for clients

  // Load the map
  InitMap();

  // Init teams
  InitTeams();

  InitSounds();
}

GameInit::GameInit():
  loading_sreen()
{
  Config::GetInstance()->RemoveAllObjectConfigs();

  // Disable sound during the loading of data
  bool enable_sound = JukeBox::GetInstance()->UseEffects();
  JukeBox::GetInstance()->ActiveEffects(false);

  Mouse::GetInstance()->Hide();

  // Init all needed data
  InitData();

  CharacterCursor::GetInstance()->Reset();
  Keyboard::GetInstance()->Reset();

  Interface::GetInstance()->Reset();
  GameMessages::GetInstance()->Reset();

  ParticleEngine::Load();

  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
  Mouse::GetInstance()->CenterPointer();

  // First "selection" of a weapon -> fix bug 6576
  ActiveTeam().AccessWeapon().Select();

  // Loading is finished, sound effects can be enabled again
  JukeBox::GetInstance()->ActiveEffects(enable_sound);

  // Waiting for others players
  if  (Network::GetInstance()->IsServer())
    EndInitGameData_NetServer();
  else if (Network::GetInstance()->IsClient())
    EndInitGameData_NetClient();

  Game::GetInstance()->Init();

  // Reset time at end of initialisation, so that the first player doesn't loose a few seconds.
  Time::GetInstance()->Reset();
}
