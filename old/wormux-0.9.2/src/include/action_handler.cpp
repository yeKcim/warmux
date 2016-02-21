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
 * Wormux action handler.
 *****************************************************************************/

#include <iostream>
#include <SDL_mutex.h>
#include <WORMUX_debug.h>
#include <WORMUX_distant_cpu.h>
#include <WORMUX_team_config.h>

#include "action_handler.h"
#include "character/character.h"
#include "character/body.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/time.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/interface.h"
#include "interface/game_msg.h"
#include "network/chat.h"
#include "network/network.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "menu/network_menu.h"
#include "network/randomsync.h"
#include "network/network.h"
#include "network/network_server.h"
#include "network/chatlogger.h"
#include "object/bonus_box.h"
#include "object/medkit.h"
#include "object/objbox.h"
#include "team/macro.h"
#include "team/team.h"
#include "sound/jukebox.h"
#include "weapon/construct.h"
#include "weapon/explosion.h"
#include "weapon/gnu.h"
#include "weapon/grapple.h"
#include "weapon/weapon.h"
#include "weapon/weapon_launcher.h"
#include "weapon/weapons_list.h"

// #############################################################################
// #############################################################################
// A client may receive an Action, that is intended to be handled only by the
// server, because the server stupidely repeat it to all clients.
// BUT a server must never receive an Action which concern only clients.

static void FAIL_IF_GAMEMASTER(Action *a)
{
  if (!a->GetCreator()) {
    fprintf(stderr, "%s", ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());
  }

  ASSERT(a->GetCreator());

  if (Network::GetInstance()->IsGameMaster()) {
    fprintf(stderr,
	    "Game Master received an action (%s) that is normally sent only by the game master only to a client,"
	    " we are going to force disconnection of evil client: %s",
	    ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str(),
	    a->GetCreator()->ToString().c_str());

    a->GetCreator()->ForceDisconnection();
  }
}

// #############################################################################
// #############################################################################

static void Action_Network_ClientChangeState (Action *a)
{
  if (!Network::GetInstance()->IsGameMaster())
    return;
  int player_id = a->PopInt();
  WNet::net_game_state_t client_state = (WNet::net_game_state_t)a->PopInt();

  MSG_DEBUG("network.game_state", "game master (%s) <-- player %d (%s)",
    WNet::GetGameStateAsString(Network::GetInstance()->GetState()),
    player_id,
    WNet::GetGameStateAsString(client_state));

  switch (Network::GetInstance()->GetState()) {
  case WNet::NO_NETWORK:
  case WNet::NETWORK_MENU_INIT:
    a->GetCreator()->GetPlayer(player_id)->SetState(Player::STATE_INITIALIZED);
    ASSERT(client_state == WNet::NETWORK_MENU_OK);
    break;

  case WNet::NETWORK_LOADING_DATA:
    a->GetCreator()->GetPlayer(player_id)->SetState(Player::STATE_READY);
    ASSERT(client_state == WNet::NETWORK_READY_TO_PLAY);
    break;

  case WNet::NETWORK_PLAYING:
  case WNet::NETWORK_NEXT_GAME:
    if (client_state == WNet::NETWORK_MENU_OK) {
      a->GetCreator()->GetPlayer(player_id)->SetState(Player::STATE_INITIALIZED);
    } else if (client_state == WNet::NETWORK_NEXT_GAME) {
      a->GetCreator()->GetPlayer(player_id)->SetState(Player::STATE_NEXT_GAME);
    } else {
      ASSERT(false);
    }
    break;

  default:
    NET_ASSERT(false)
      {
        if(a->GetCreator()) a->GetCreator()->ForceDisconnection();
        return;
      }
    break;
  }
}

static void Action_Network_MasterChangeState (Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  WNet::net_game_state_t server_state = (WNet::net_game_state_t)a->PopInt();

  MSG_DEBUG("network.game_state", "game master (%s) --> player %d (%s)",
    WNet::GetGameStateAsString(server_state),
    Network::GetInstance()->GetPlayer().GetId(),
    WNet::GetGameStateAsString(Network::GetInstance()->GetState()));

  switch (Network::GetInstance()->GetState()) {
  case WNet::NETWORK_MENU_OK:
    if (server_state == WNet::NETWORK_LOADING_DATA)
      Network::GetInstance()->SetState(WNet::NETWORK_LOADING_DATA);
    ASSERT(server_state == WNet::NETWORK_LOADING_DATA
	   || server_state == WNet::NETWORK_NEXT_GAME);
    break;

  case WNet::NETWORK_READY_TO_PLAY:
    Network::GetInstance()->SetState(WNet::NETWORK_PLAYING);
    ASSERT(server_state == WNet::NETWORK_PLAYING);
    break;

  case WNet::NETWORK_NEXT_GAME:
    break;

  case WNet::NETWORK_PLAYING:
    if (Game::GetInstance()->IsGameFinished())
      break;

  default:
    NET_ASSERT(false)
      {
	if(a->GetCreator()) a->GetCreator()->ForceDisconnection();
	return;
      }
  }
}

static void Action_Network_Set_GameMaster(Action */*a*/)
{
  Network::GetInstance()->SetGameMaster();

  if (Network::GetInstance()->network_menu != NULL) {
    Network::GetInstance()->network_menu->SetGameMasterCallback();
  }

  // Switching network state
  switch (Network::GetInstance()->GetState()) {
  case WNet::NO_NETWORK:
  case WNet::NETWORK_MENU_INIT:
    break;
  case WNet::NETWORK_MENU_OK:
    Network::GetInstance()->SetState(WNet::NETWORK_MENU_INIT);
    break;
  case WNet::NETWORK_LOADING_DATA:
  case WNet::NETWORK_READY_TO_PLAY:
  case WNet::NETWORK_PLAYING:
  case WNet::NETWORK_NEXT_GAME:
    break;
  }
}

static void Action_Network_Check_Phase1 (Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  int player_id = Network::GetInstance()->GetPlayer().GetId();
  Action b(Action::ACTION_NETWORK_CHECK_PHASE2);
  b.Push(player_id);
  b.Push(ActiveMap()->GetRawName());
  b.Push(int(ActiveMap()->ReadImgGround().ComputeCRC()));

  TeamsList::iterator it = GetTeamsList().playing_list.begin();
  for (; it != GetTeamsList().playing_list.end() ; ++it) {
    b.Push((*it)->GetId());
  }

  // send information to the server
  Network::GetInstance()->SendActionToAll(b);
}

enum net_error {
  WRONG_MAP_NAME,
  WRONG_MAP_CRC,
  WRONG_TEAM
};

static std::string NetErrorId_2_String(enum net_error error)
{
  std::string s;

  switch (error) {
  case WRONG_MAP_NAME:
    s = _("Wrong map name!");
    break;
  case WRONG_MAP_CRC:
    s = _("Wrong map CRC!");
    break;
  case WRONG_TEAM:
    s = _("Wrong team!");
    break;
  }
  return s;
}

static void Action_Network_Disconnect_On_Error(Action *a)
{
  enum net_error error = (enum net_error)a->PopInt();
  AppWormux::DisplayError(NetErrorId_2_String(error));
  Network::Disconnect();
}

static void DisconnectOnError(enum net_error error)
{
  Action a(Action::ACTION_NETWORK_DISCONNECT_ON_ERROR);
  a.Push(int(error));
  Network::GetInstance()->SendActionToAll(a);
  Network::Disconnect();
}

static void Error_in_Network_Check_Phase2 (Action *a, enum net_error error)
{
  std::string str = Format(_("Error initializing network: Client %s does not agree with you!! - %s"),
			   a->GetCreator()->GetAddress().c_str(),
			   NetErrorId_2_String(error).c_str());
  std::cerr << str << std::endl;
  DisconnectOnError(error);
  AppWormux::DisplayError(str);
}

static void Action_Network_Check_Phase2 (Action *a)
{
  // Game master receives information from the client
  if (!Network::GetInstance()->IsGameMaster())
    return;

  int player_id = a->PopInt();
  // Check the map name
  std::string map = a->PopString();
  if (map != ActiveMap()->GetRawName()) {
    std::cerr << map << " != " << ActiveMap()->GetRawName() << std::endl;
    Error_in_Network_Check_Phase2(a, WRONG_MAP_NAME);
    return;
  }

  // Check the map CRC
  int crc = int(ActiveMap()->ReadImgGround().ComputeCRC());
  int remote_crc = a->PopInt();
  if (crc != remote_crc) {
    std::cerr << map << " is different (crc=" << crc << ", remote crc="<< remote_crc << ")" << std::endl;
    Error_in_Network_Check_Phase2(a, WRONG_MAP_CRC);
    return;
  }

  // Check the teams
  std::string team;
  TeamsList::iterator it = GetTeamsList().playing_list.begin();
  for (; it != GetTeamsList().playing_list.end() ; ++it) {
    team = a->PopString();
    if (team != (*it)->GetId()) {
      Error_in_Network_Check_Phase2(a, WRONG_TEAM);
      return;
    }
  }

  // this client has been checked, it's ok :-)
  a->GetCreator()->GetPlayer(player_id)->SetState(Player::STATE_CHECKED);
}

// ########################################################

static void Action_Player_ChangeWeapon (Action *a)
{
  JukeBox::GetInstance()->Play("default", "change_weapon");
  ActiveTeam().SetWeapon(static_cast<Weapon::Weapon_type>(a->PopInt()));
}

static void Action_Player_ChangeCharacter (Action *a)
{
  JukeBox::GetInstance()->Play("default", "character/change_in_same_team");
  int char_no = a->PopInt();
  Character * c = ActiveTeam().FindByIndex(char_no);
  ActiveTeam().SelectCharacter(c);
  Camera::GetInstance()->FollowObject(&ActiveCharacter(),true);
}

static void Action_Game_CalculateFrame (Action */*a*/)
{
  // Nothing to do here:
  // It is just an indication for the action handler
  // that the frame is complete.
}

static void Action_DropBonusBox (Action */*a*/)
{
  ObjBox* current_box = Game::GetInstance()->GetCurrentBox();
  if (current_box != NULL) {
    current_box->DropBox();
  }
}

static void Action_RequestBonusBoxDrop (Action */*a*/)
{
  if (Network::GetInstance()->IsTurnMaster())
    Game::GetInstance()->RequestBonusBoxDrop();
}

// ########################################################

static void Action_Rules_SetGameMode (Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  std::string game_mode_name = a->PopString();
  std::string game_mode = a->PopString();
  std::string game_mode_objects = a->PopString();

  MSG_DEBUG("game_mode", "Receiving game_mode: %s", game_mode_name.c_str());

  GameMode::GetInstance()->LoadFromString(game_mode_name,
                                          game_mode,
                                          game_mode_objects);
}

void SendGameMode()
{
  ASSERT(Network::GetInstance()->IsGameMaster());

  Action a(Action::ACTION_RULES_SET_GAME_MODE);

  std::string game_mode_name = "network(";
  game_mode_name += GameMode::GetInstance()->GetName();
  game_mode_name += ")";

  a.Push(game_mode_name);

  std::string game_mode;
  std::string game_mode_objects;

  GameMode::GetInstance()->ExportToString(game_mode, game_mode_objects);
  a.Push(game_mode);
  a.Push(game_mode_objects);

  MSG_DEBUG("game_mode", "Sending game_mode: %s", game_mode_name.c_str());

  Network::GetInstance()->SendActionToAll(a);
}

// ########################################################

static void Action_ChatMessage (Action *a)
{
  std::string nickname = a->PopString();
  std::string message = a->PopString();

  ChatLogger::GetInstance()->LogMessage(nickname+"> "+message);
  AppWormux::GetInstance()->ReceiveMsgCallback(nickname+"> "+message);
}

static void Action_AnnouncePause(Action *a)
{
  DistantComputer * computer = a->GetCreator();
  std::string computer_name = computer->ToString();
  std::string message = Format(_("%s needs a pause."), computer_name.c_str());
  AppWormux::GetInstance()->ReceiveMsgCallback(message);
}

static void _Action_SelectMap(Action *a)
{
  std::string map_name = a->PopString();

  if (map_name != "random") {
    MapsList::GetInstance()->SelectMapByName(map_name);
  } else {
    MapsList::GetInstance()->SelectRandomMapByName(a->PopString());
  }

  if (Network::GetInstance()->network_menu != NULL) {
    Network::GetInstance()->network_menu->ChangeMapCallback();
  }
}

static Player* _Action_GetPlayer(Action *a, uint player_id)
{
  Player *player = NULL;

  if (Network::IsConnected()) {
    if (a->GetCreator()) {
      player = a->GetCreator()->GetPlayer(player_id);
      ASSERT(player);
    } else {
      player = &(Network::GetInstance()->GetPlayer());
      ASSERT(player->GetId() == player_id);
    }
  }

  return player;
}

static void _Action_AddTeam(Action *a, Player* player)
{
  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());
  the_team.ai = a->PopString();

  MSG_DEBUG("action_handler.menu", "+ %s", the_team.id.c_str());

  bool local_team = (!Network::IsConnected() || player == &(Network::GetInstance()->GetPlayer()));

  GetTeamsList().AddTeam(the_team, local_team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->AddTeamCallback(the_team.id);

  ASSERT (player);
  player->AddTeam(the_team);
}

static void Action_Game_Info(Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  if (Network::GetInstance()->GetState() != WNet::NO_NETWORK)
    return;

  Network::GetInstance()->SetState(WNet::NETWORK_MENU_INIT);

  _Action_SelectMap(a);

  uint nb_players = a->PopInt();
  for (uint i = 0; i < nb_players; i++) {
    uint player_id = a->PopInt();


    ASSERT (a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL);
    ASSERT (player_id != Network::GetInstance()->GetPlayer().GetId());
    a->GetCreator()->AddPlayer(player_id);

    uint nb_teams = a->PopInt();

    Player* player = _Action_GetPlayer(a, player_id);
    for (uint j = 0; j < nb_teams; j++) {
      _Action_AddTeam(a, player);
    }
  }
}

static void Action_Game_SetMap (Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  _Action_SelectMap(a);
}

static void Action_Game_AddTeam (Action *a)
{
  uint player_id = a->PopInt();
  ASSERT(Network::GetInstance()->network_menu->HasOpenTeamSlot());

  Player * player = Network::GetInstance()->LockRemoteHostsAndGetPlayer(player_id);
  if (player) {
    _Action_AddTeam(a, player);
  }
  Network::GetInstance()->UnlockRemoteHosts();
}

static void Action_Game_UpdateTeam (Action *a)
{
  uint player_id = a->PopInt();
  std::string old_team_id = a->PopString();

  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());
  the_team.ai = a->PopString();

  GetTeamsList().UpdateTeam(old_team_id, the_team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->UpdateTeamCallback(old_team_id, the_team.id);

  Player* player = _Action_GetPlayer(a, player_id);
  if (player) {
    player->UpdateTeam(old_team_id, the_team);
  }
}

static void _Action_DelTeam(Player *player, const std::string& team_id)
{
  if (player) {
    player->RemoveTeam(team_id);
  }

  MSG_DEBUG("action_handler.menu", "- %s", team_id.c_str());

  GetTeamsList().DelTeam(team_id);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->DelTeamCallback(team_id);
}

static void Action_Game_DelTeam (Action *a)
{
  uint player_id = a->PopInt();
  std::string team_id = a->PopString();

  Player * player = Network::GetInstance()->LockRemoteHostsAndGetPlayer(player_id);
  if (player) {
    _Action_DelTeam(player, team_id);
  }
  Network::GetInstance()->UnlockRemoteHosts();
}

static inline void add_team_config_to_action(Action& a, const ConfigTeam& team)
{
  a.Push(team.id);
  a.Push(team.player_name);
  a.Push(int(team.nb_characters));
  a.Push(team.ai);
}

void ActionHandler::NewRequestTeamAction(const ConfigTeam & team)
{
  int player_id = int(Network::GetInstance()->GetPlayer().GetId());
  Action * a = new Action(Action::ACTION_GAME_REQUEST_TEAM);
  a->Push(player_id);
  add_team_config_to_action(*a, team);
  NewAction(a);
}

static void Action_Game_RequestTeam(Action *a)
{
  if (!Network::GetInstance()->IsGameMaster())
    return;

  if (!Network::GetInstance()->network_menu->HasOpenTeamSlot())
    return;

  int player_id = a->PopInt();
  const std::string team_id = a->PopString();
  const std::string player_name = a->PopString();
  int nb_characters = a->PopInt();
  const std::string ai = a->PopString();

  Player * player = Network::GetInstance()->LockRemoteHostsAndGetPlayer(player_id);
  if (player) {
    if (int(player->GetTeams().size()) < GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer()) {
      Team * team = Network::GetInstance()->network_menu->FindUnusedTeam(team_id);
      Action action_to_send(Action::ACTION_GAME_ADD_TEAM);
      action_to_send.Push(player_id);
      action_to_send.Push(team->GetId());
      action_to_send.Push(player_name);
      action_to_send.Push(nb_characters);
      action_to_send.Push(ai);

      Network::GetInstance()->SendActionToAll(action_to_send);
      // Add the team before progressing further actions,
      // as the next action could request another team which would be then the same team.
      Action_Game_AddTeam(&action_to_send); // empties the actions stack...
    }
  }
  Network::GetInstance()->UnlockRemoteHosts();
}

static void Action_Game_RequestTeamRemoval(Action *a)
{
  if (!Network::GetInstance()->IsGameMaster())
    return;

  int player_id = a->PopInt();
  Player * player = Network::GetInstance()->LockRemoteHostsAndGetPlayer(player_id);
  if (player->GetNbTeams() > 0) {
    const std::string& team_id = player->GetTeams().back().id;

    Action action_to_send(Action::ACTION_GAME_DEL_TEAM);
    action_to_send.Push(player_id);
    action_to_send.Push(team_id);
    Network::GetInstance()->SendActionToAll(action_to_send);
    // Remove the team before progressing further actions,
    // as the next action could request another team which could then fail as the team slots are full.
    Action_Game_DelTeam(&action_to_send); // empties the actions stack...
  }
  Network::GetInstance()->UnlockRemoteHosts();
}


void WORMUX_DisconnectPlayer(Player& player)
{
  std::list<ConfigTeam>::iterator it = player.owned_teams.begin();
  while (it != player.owned_teams.end()) {
    std::string team_id = it->id;

    if (Game::IsRunning()) {
      player.RemoveTeam(team_id);
      int unused_buffer;
      Team* the_team = GetTeamsList().FindById(team_id, unused_buffer);
      the_team->Abandon();
    } else {
      _Action_DelTeam(&player, team_id);
    }
    it = player.owned_teams.begin();
  }
}

// ########################################################

static void Action_Character_Jump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ActiveCharacter().Jump();
}

static void Action_Character_HighJump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ActiveCharacter().HighJump();
}

static void Action_Character_BackJump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ActiveCharacter().BackJump();
}

static void Action_Character_StartMovingLeft(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().AddLRMoveIntention(slowly ? INTENTION_MOVE_LEFT_SLOWLY : INTENTION_MOVE_LEFT);
}

static void Action_Character_StopMovingLeft(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().RemoveLRMoveIntention(slowly ? INTENTION_MOVE_LEFT_SLOWLY : INTENTION_MOVE_LEFT);
}

static void Action_Character_StartMovingRight(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().AddLRMoveIntention(slowly ? INTENTION_MOVE_RIGHT_SLOWLY : INTENTION_MOVE_RIGHT);
}

static void Action_Character_StopMovingRight(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().RemoveLRMoveIntention(slowly ? INTENTION_MOVE_RIGHT_SLOWLY : INTENTION_MOVE_RIGHT);
}

static void Action_Character_StartMovingUp(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().AddUDMoveIntention(slowly ? INTENTION_MOVE_UP_SLOWLY : INTENTION_MOVE_UP);
  if (Network::GetInstance()->IsTurnMaster()) {
    HideGameInterface();
    ActiveTeam().crosshair.Show();
  }
}

static void Action_Character_StopMovingUp(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().RemoveUDMoveIntention(slowly ? INTENTION_MOVE_UP_SLOWLY : INTENTION_MOVE_UP);
}

static void Action_Character_StartMovingDown(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().AddUDMoveIntention(slowly ? INTENTION_MOVE_DOWN_SLOWLY : INTENTION_MOVE_DOWN);
  if (Network::GetInstance()->IsTurnMaster()) {
    HideGameInterface();
    ActiveTeam().crosshair.Show();
  }
}

static void Action_Character_StopMovingDown(Action *a)
{
  bool slowly = a->PopInt();
  ActiveCharacter().RemoveUDMoveIntention(slowly ? INTENTION_MOVE_DOWN_SLOWLY : INTENTION_MOVE_DOWN);
}

static void Action_Weapon_StartShooting(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StartShooting();
}

static void Action_Weapon_StopShooting(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StopShooting();
}

static void Action_Weapon_SetTarget (Action *a)
{
  MSG_DEBUG("action_handler", "Set target by clicking");

  ActiveTeam().AccessWeapon().ChooseTarget (a->PopPoint2i());
}

static void Action_Weapon_SetTimeout (Action *a)
{
  WeaponLauncher* launcher = dynamic_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(launcher != NULL)
  {
    return;
  }
  launcher->GetProjectile()->SetTimeOut(a->PopInt());
}

static void Action_Weapon_StartMovingLeft(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StartMovingLeft();
}

static void Action_Weapon_StopMovingLeft(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StopMovingLeft();
}

static void Action_Weapon_StartMovingRight(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StartMovingRight();
}

static void Action_Weapon_StopMovingRight(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StopMovingRight();
}

static void Action_Weapon_StartMovingUp(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StartMovingUp();
}

static void Action_Weapon_StopMovingUp(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StopMovingUp();
}

static void Action_Weapon_StartMovingDown(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StartMovingDown();
}

static void Action_Weapon_StopMovingDown(Action */*a*/)
{
  ActiveTeam().AccessWeapon().StopMovingDown();
}

static void Action_Weapon_Construction (Action *a)
{
  Construct* construct_weapon = dynamic_cast<Construct*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(construct_weapon != NULL)
  {
    return;
  }

  construct_weapon->SetAngle(a->PopDouble());
}

// ########################################################

static void Action_Network_RandomInit (Action *a)
{
  MSG_DEBUG("random", "Initialization from network");
  RandomSync().SetSeed(a->PopInt());
}

static void Action_Network_VerifyRandomSync(Action *a)
{
  uint local_seed = RandomSync().GetSeed();
  uint remote_seed = (uint)(a->PopInt());
  MSG_DEBUG("random.verify","Verify seed: %d (local) == %d (remote)", local_seed, remote_seed);

  ASSERT(remote_seed == local_seed);

  if (local_seed != remote_seed)
    RandomSync().SetSeed(remote_seed);
}

static void Action_Time_VerifySync(Action *a)
{
  uint local_time = Time::GetInstance()->Read();
  uint remote_time = (uint)(a->PopInt());
  MSG_DEBUG("time.verify","Verify time: %d (local) == %d (remote)", local_time, remote_time);
  ASSERT(local_time == remote_time);
}

// Nothing to do here. Just for time synchronisation
static void Action_Network_Ping(Action */*a*/)
{
}

// ########################################################

static void _Info_ConnectHost(const std::string& hostname, const std::string& nicknames)
{
  // For translators: extended in "<nickname> (<host>) just connected
  std::string msg = Format("%s (%s) just connected", nicknames.c_str(), hostname.c_str());
  std::cerr << msg.c_str() << std::endl;

  ChatLogger::LogMessageIfOpen(msg);

  if (Game::IsRunning())
    GameMessages::GetInstance()->Add(msg);
  else if (Network::GetInstance()->network_menu != NULL)
    //Network Menu
    AppWormux::GetInstance()->ReceiveMsgCallback(msg);

  if (Config::GetInstance()->GetWarnOnNewPlayer())
    JukeBox::GetInstance()->Play("default", "menu/newcomer");
}

static inline void add_player_info_to_action(Action& a, const Player& player)
{
  a.Push(int(player.GetId()));
  a.Push(int(player.GetNbTeams()));

  std::list<ConfigTeam>::const_iterator team;
  for (team = player.GetTeams().begin(); team != player.GetTeams().end(); team++) {
    add_team_config_to_action(a, *team);
  }
}

void SendInitialGameInfo(DistantComputer* client, int added_player_id)
{
  // we have to tell this new computer
  // what teams / maps have already been selected

  MSG_DEBUG("network", "Server: Sending map information");

  Action a(Action::ACTION_GAME_INFO);
  MapsList::GetInstance()->FillActionMenuSetMap(a);

  MSG_DEBUG("network", "Server: Sending teams information");

  // count the number of players
  int nb_players = 1;

  std::list<DistantComputer*>& hosts = Network::GetInstance()->LockRemoteHosts();
  std::list<DistantComputer*>::const_iterator it;
  std::list<Player>::const_iterator player;

  for (it = hosts.begin(); it != hosts.end(); it++) {
    const std::list<Player>& players = (*it)->GetPlayers();

    for (player = players.begin(); player != players.end(); player++) {
      if (int(player->GetId()) != added_player_id) {
        nb_players++;
      }
    }
  }

  a.Push(nb_players);

  // Teams infos of each player
  add_player_info_to_action(a, Network::GetInstance()->GetPlayer());

  for (it = hosts.begin(); it != hosts.end(); it++) {
    const std::list<Player>& players = (*it)->GetPlayers();

    for (player = players.begin(); player != players.end(); player++) {
      if (int(player->GetId()) != added_player_id) {
        add_player_info_to_action(a, (*player));
      }
    }
  }

  Network::GetInstance()->UnlockRemoteHosts();

  Network::GetInstance()->SendActionToOne(a, client);
}

// Only used to notify clients that someone connected to the server
static void Action_Info_ClientConnect(Action *a)
{
  int player_id = a->PopInt();
  std::string hostname = a->PopString();
  std::string nicknames = a->PopString();

  ASSERT (a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL);
  a->GetCreator()->AddPlayer(player_id);


  _Info_ConnectHost(hostname, nicknames);

  if (Network::GetInstance()->IsGameMaster()) {
    SendInitialGameInfo(a->GetCreator(), player_id);
  }
}

void WORMUX_ConnectHost(DistantComputer& host)
{
  std::string hostname = host.GetAddress();
  std::string nicknames = host.GetNicknames();

  _Info_ConnectHost(hostname, nicknames);

  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_INFO_CLIENT_CONNECT);
    ASSERT(host.GetPlayers().size() == 1);
    int player_id = host.GetPlayers().back().GetId();
    a.Push(player_id);
    a.Push(hostname);
    a.Push(nicknames);

    Network::GetInstance()->SendActionToAllExceptOne(a, &host);
  }
}

static void _Info_DisconnectHost(const std::string& hostname, const std::string& nicknames)
{
  // For translators: extended in "<nickname> (<host>) just disconnected
  std::string msg = Format("%s (%s) just disconnected", nicknames.c_str(), hostname.c_str());
  fprintf(stderr, "%s\n", msg.c_str());

  ChatLogger::LogMessageIfOpen(msg);

  if (Game::IsRunning())
    GameMessages::GetInstance()->Add(msg);
  else if (Network::GetInstance()->network_menu != NULL)
    //Network Menu
    AppWormux::GetInstance()->ReceiveMsgCallback(msg);
}

// Used to notify clients that someone disconnected from the server
static void Action_Info_ClientDisconnect(Action *a)
{
  std::string hostname = a->PopString();
  std::string nicknames = a->PopString();

  _Info_DisconnectHost(hostname, nicknames);

  int nb_players = a->PopInt();
  for (int i = 0; i < nb_players; i++) {
    int player_id = a->PopInt();
    a->GetCreator()->DelPlayer(player_id);
  }
}

void WORMUX_DisconnectHost(DistantComputer& host)
{
  std::string hostname = host.GetAddress();
  std::string nicknames = host.GetNicknames();

  _Info_DisconnectHost(hostname, nicknames);

  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_INFO_CLIENT_DISCONNECT);
    a.Push(hostname);
    a.Push(nicknames);
    a.Push(int(host.GetPlayers().size()));

    std::list<Player>::const_iterator player_it;
    for (player_it = host.GetPlayers().begin(); player_it != host.GetPlayers().end(); player_it++) {
      a.Push(int(player_it->GetId()));
    }
    Network::GetInstance()->SendActionToAll(a); // host is already removed from the list
  }
}
// ########################################################
// ########################################################
// ########################################################

void Action_Handler_Init()
{
  ActionHandler::GetInstance()->Lock();

  // ########################################################
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_CLIENT_CHANGE_STATE, "NETWORK_client_change_state", &Action_Network_ClientChangeState);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_MASTER_CHANGE_STATE, "NETWORK_master_change_state", &Action_Network_MasterChangeState);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_CHECK_PHASE1, "NETWORK_check1", &Action_Network_Check_Phase1);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_CHECK_PHASE2, "NETWORK_check2", &Action_Network_Check_Phase2);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_DISCONNECT_ON_ERROR, "NETWORK_disconnect_on_error", &Action_Network_Disconnect_On_Error);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_SET_GAME_MASTER, "NETWORK_set_game_master", &Action_Network_Set_GameMaster);

  // ########################################################
  ActionHandler::GetInstance()->Register (Action::ACTION_PLAYER_CHANGE_WEAPON, "PLAYER_change_weapon", &Action_Player_ChangeWeapon);
  ActionHandler::GetInstance()->Register (Action::ACTION_PLAYER_CHANGE_CHARACTER, "PLAYER_change_character", &Action_Player_ChangeCharacter);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_CALCULATE_FRAME, "GAME_calculate_frame", &Action_Game_CalculateFrame);

  // ########################################################
  // To be sure that rules will be the same on each computer
  ActionHandler::GetInstance()->Register (Action::ACTION_RULES_SET_GAME_MODE, "RULES_set_game_mode", &Action_Rules_SetGameMode);

  // ########################################################
  // Chat message
  ActionHandler::GetInstance()->Register (Action::ACTION_CHAT_MESSAGE, "chat_message", Action_ChatMessage);
  ActionHandler::GetInstance()->Register (Action::ACTION_ANNOUNCE_PAUSE, "chat_announce_pause", Action_AnnouncePause);

  // Initial information about the game: map, teams already selected, ...
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_INFO, "GAME_info", &Action_Game_Info);

  // Map selection in network menu
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_SET_MAP, "GAME_set_map", &Action_Game_SetMap);

  // Teams selection in network menu
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_ADD_TEAM, "GAME_add_team", &Action_Game_AddTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_DEL_TEAM, "GAME_del_team", &Action_Game_DelTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_UPDATE_TEAM, "GAME_update_team", &Action_Game_UpdateTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_REQUEST_TEAM, "GAME_request_team", &Action_Game_RequestTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_REQUEST_TEAM_REMOVAL, "GAME_request_team_removal", &Action_Game_RequestTeamRemoval);

  // ########################################################
  // Character's move
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_JUMP, "CHARACTER_jump", &Action_Character_Jump);
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_HIGH_JUMP, "CHARACTER_super_jump", &Action_Character_HighJump);
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_BACK_JUMP, "CHARACTER_back_jump", &Action_Character_BackJump);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_START_MOVING_LEFT, "CHARACTER_start_moving_left", &Action_Character_StartMovingLeft);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_STOP_MOVING_LEFT, "CHARACTER_stop_moving_left", &Action_Character_StopMovingLeft);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_START_MOVING_RIGHT, "CHARACTER_start_moving_right", &Action_Character_StartMovingRight);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_STOP_MOVING_RIGHT, "CHARACTER_stop_moving_right", &Action_Character_StopMovingRight);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_START_MOVING_UP, "CHARACTER_start_moving_up", &Action_Character_StartMovingUp);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_STOP_MOVING_UP, "CHARACTER_stop_moving_up", &Action_Character_StopMovingUp);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_START_MOVING_DOWN, "CHARACTER_start_moving_down", &Action_Character_StartMovingDown);
 ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_STOP_MOVING_DOWN, "CHARACTER_stop_moving_down", &Action_Character_StopMovingDown);

  // ########################################################
  // Quite standard weapon options

  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_START_SHOOTING, "WEAPON_start_shooting", &Action_Weapon_StartShooting);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_SHOOTING, "WEAPON_stop_shooting", &Action_Weapon_StopShooting);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SET_TIMEOUT, "WEAPON_set_timeout", &Action_Weapon_SetTimeout);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SET_TARGET, "WEAPON_set_target", &Action_Weapon_SetTarget);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_START_MOVING_LEFT, "WEAPON_start_moving_left", &Action_Weapon_StartMovingLeft);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_MOVING_LEFT, "WEAPON_stop_moving_left", &Action_Weapon_StopMovingLeft);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_START_MOVING_RIGHT, "WEAPON_start_moving_right", &Action_Weapon_StartMovingRight);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_MOVING_RIGHT, "WEAPON_stop_moving_right", &Action_Weapon_StopMovingRight);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_START_MOVING_UP, "WEAPON_start_moving_up", &Action_Weapon_StartMovingUp);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_MOVING_UP, "WEAPON_stop_moving_up", &Action_Weapon_StopMovingUp);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_START_MOVING_DOWN, "WEAPON_start_moving_down", &Action_Weapon_StartMovingDown);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_MOVING_DOWN, "WEAPON_stop_moving_down", &Action_Weapon_StopMovingDown);

  // Special weapon options
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_CONSTRUCTION, "WEAPON_construction", &Action_Weapon_Construction);

  // Bonus box
  ActionHandler::GetInstance()->Register (Action::ACTION_DROP_BONUS_BOX, "BONUSBOX_drop_box", &Action_DropBonusBox);
  ActionHandler::GetInstance()->Register (Action::ACTION_REQUEST_BONUS_BOX_DROP, "BONUSBOX_request_box_drop", &Action_RequestBonusBoxDrop);
  // ########################################################
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_PING, "NETWORK_ping", &Action_Network_Ping);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_RANDOM_INIT, "NETWORK_random_init", &Action_Network_RandomInit);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_VERIFY_RANDOM_SYNC, "NETWORK_verify_random_sync", &Action_Network_VerifyRandomSync);
  ActionHandler::GetInstance()->Register (Action::ACTION_TIME_VERIFY_SYNC, "TIME_verify_sync", &Action_Time_VerifySync);
  ActionHandler::GetInstance()->Register (Action::ACTION_INFO_CLIENT_DISCONNECT, "INFO_client_disconnect", &Action_Info_ClientDisconnect);
  ActionHandler::GetInstance()->Register (Action::ACTION_INFO_CLIENT_CONNECT, "INFO_client_connect", &Action_Info_ClientConnect);

  // ########################################################
  ActionHandler::GetInstance()->UnLock();
}

// ########################################################
// ########################################################
// ########################################################


ActionHandler::ActionHandler(): WActionHandler()
{
}

ActionHandler::~ActionHandler()
{
}

bool ActionHandler::ExecActionsForOneFrame()
{
  ASSERT(Game::GetInstance()->IsRunning());
  Action * a;
  std::list<Action*>::iterator it;
  bool frame_complete = false;
  Lock();
  it = queue.begin();
  while (it != queue.end() && !frame_complete)
  {
    a = (*it);
    if (a->GetType() == Action::ACTION_GAME_CALCULATE_FRAME)
      frame_complete = true;
    it++;
  }
  if (!frame_complete) {
    UnLock();
    return false;
  }

  frame_complete = false;
  it = queue.begin();
  while (it != queue.end() && !frame_complete)
  {
    a = (*it);
    if (a->GetType() == Action::ACTION_GAME_CALCULATE_FRAME)
      frame_complete = true;

    if (!a->IsFrameLess()) {
      MSG_DEBUG("action_time", "-> Action %s (action time: %u, time: %u)",
                GetActionName(a->GetType()).c_str(), a->GetTimestamp(),
                Time::GetInstance()->Read());
    }

    Exec (a);

    delete *it;
    it = queue.erase(it);
  }
  UnLock();
  return frame_complete;
}

void ActionHandler::ExecFrameLessActions()
{
  Action * a;
  std::list<Action*>::iterator it;
  Lock();

  for (it = queue.begin(); it != queue.end() ;)
  {
    a = (*it);

    // only handle frame less actions.
    if (!a->IsFrameLess()) {
      it++;
      continue;
    }

    // Do not execute actions from Network if we are not connected anymore
    if (!a->GetCreator()|| Network::IsConnected()) {

      Exec (a);

      // To refresh the menu even if it is waiting in SDL_WaitEvent
      // One new event will be needed.
      Menu::WakeUpOnCallback();
    }
    delete *it;
    it = queue.erase(it);
  }
  UnLock();
}

void ActionHandler::NewAction(Action* a, bool repeat_to_network)
{
  WActionHandler::NewAction(a);

  if (repeat_to_network)
    Network::GetInstance()->SendActionToAll(*a);

  // To make action executed even if the menu is waiting in SDL_WaitEvent
  // One new event will be needed (see ExecActions).
  Menu::WakeUpOnCallback();
}

