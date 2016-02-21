/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Warmux action handler.
 *****************************************************************************/

#include <algorithm>
#include <iostream>
#include <SDL_mutex.h>
#include <WARMUX_debug.h>
#include <WARMUX_distant_cpu.h>
#include <WARMUX_file_tools.h>
#include <WARMUX_team_config.h>
#include <WARMUX_i18n.h>

#include "action_handler.h"
#include "character/character.h"
#include "character/body.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/game_time.h"
#include "graphic/colors.h"
#include "gui/question.h"
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
#include "replay/replay.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
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
    fprintf(stderr, "%s", ActionHandler::GetActionName(a->GetType()).c_str());
  }

  ASSERT(a->GetCreator());

  if (Network::GetInstance()->IsGameMaster()) {
    fprintf(stderr,
            "Game Master received an action (%s) that is normally sent only by the game master only to a client,"
            " we are going to force disconnection of evil client: %s",
            ActionHandler::GetActionName(a->GetType()).c_str(),
            a->GetCreator()->ToString().c_str());

    a->GetCreator()->ForceDisconnection();
  }
}

// #############################################################################
// #############################################################################

static void Action_Network_ClientChangeState(Action *a)
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
        if (a->GetCreator())
          a->GetCreator()->ForceDisconnection();
        return;
      }
    break;
  }
}

static void Action_Network_MasterChangeState(Action *a)
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
      if (a->GetCreator())
        a->GetCreator()->ForceDisconnection();
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

static void Action_Network_Check_Phase1(Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  int player_id = Network::GetInstance()->GetPlayer().GetId();
  Action b(Action::ACTION_NETWORK_CHECK_PHASE2);
  b.Push(player_id);

  // At that point, data is loaded or this is a bug
  b.Push(ActiveMap()->GetRawName());
  b.Push(int(GetWorld().ground.GetCRC()));
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
  WRONG_TEAM,
  WRONG_SYNC,
};

static std::string NetErrorId_2_String(enum net_error error)
{
  switch (error) {
  case WRONG_MAP_NAME:
    return _("Wrong map name!");
  case WRONG_MAP_CRC:
    return _("Wrong map CRC!");
  case WRONG_TEAM:
    return _("Wrong team!");
  case WRONG_SYNC:
    return _("Game out of sync!");
  default: return "";
  }
}

/*
 * This function is useful to call in a context of Action execution
 * while Network::Disconnect cannot be called because it will
 * result in a deadlock.
 */
static void ForceGlobalDisconnection()
{
  std::list<DistantComputer*>& cpus = Network::GetInstance()->GetRemoteHosts();
  for (std::list<DistantComputer*>::iterator itcpu = cpus.begin();
       itcpu != cpus.end();
       ++itcpu) {
    (*itcpu)->ForceDisconnection();
  }
}

static void Action_Network_Disconnect_On_Error(Action *a)
{
  enum net_error error = (enum net_error)a->PopInt();
  AppWarmux::DisplayError(NetErrorId_2_String(error));
  ForceGlobalDisconnection();
}

static void DisconnectOnError(enum net_error error)
{
  Action a(Action::ACTION_NETWORK_DISCONNECT_ON_ERROR);
  a.Push(int(error));
  Network::GetInstance()->SendActionToAll(a);
  ForceGlobalDisconnection();
}

static void Error_in_Network_Check_Phase2(Action *a, enum net_error error)
{
  std::string str = Format(_("Error initializing network: Client %s does not agree with you!! - %s"),
                           a->GetCreator()->GetAddress().c_str(),
                           NetErrorId_2_String(error).c_str());
  std::cerr << str << std::endl;
  DisconnectOnError(error);
  AppWarmux::DisplayError(str);
}

static void Action_Network_Check_Phase2(Action *a)
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
  int crc = int(GetWorld().ground.GetCRC());
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

static void Action_Player_ChangeWeapon(Action *a)
{
  JukeBox::GetInstance()->Play("default", "change_weapon");
  ActiveTeam().SetWeapon(static_cast<Weapon::Weapon_type>(a->PopInt()));
}

static void Action_Player_ChangeCharacter(Action *a)
{
  JukeBox::GetInstance()->Play("default", "character/change_in_same_team");
  std::string id = a->PopString();
  if (id != "")
    TeamsList::GetInstance()->SetActive(id);
  int char_no = a->PopInt();
  Character * c = ActiveTeam().FindByIndex(char_no);
  ActiveTeam().SelectCharacter(c);
  Camera::GetInstance()->FollowObject(c, true);
}

static void Action_Game_Unpack(Action *a)
{
  int nb = a->PopInt();
  NET_ASSERT(nb>0 && (uint)nb<=Action::MAX_FRAMES)
  {
    if (a->GetCreator())
      a->GetCreator()->ForceDisconnection();
    return;
  }

  // We received actions, act as if 
  GameTime::GetInstance()->SetWaitingForNetwork(false);
  ActionHandler::GetInstance()->AddActionsFrames(nb, a->GetCreator());
}

static void Action_Game_CalculateFrame(Action */*a*/)
{
  // Nothing to do here:
  // It is just an indication for the action handler
  // that the frame is complete.
}

static void Action_DropBonusBox(Action */*a*/)
{
  ObjBox* current_box = Game::GetInstance()->GetCurrentBox();
  if (current_box != NULL) {
    current_box->DropBox();
  }
}

static void Action_RequestBonusBoxDrop(Action */*a*/)
{
  if (Network::GetInstance()->IsTurnMaster())
    Game::GetInstance()->RequestBonusBoxDrop();
}

// ########################################################

static void Action_Rules_SetGameMode(Action *a)
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

static const Color& DefaultCPUColor(const DistantComputer *cpu)
{
  if (!cpu)
    return white_color; // ie local

  const std::list<DistantComputer*>& cpus = Network::GetInstance()->GetRemoteHosts();
  int i = 0;
  for (std::list<DistantComputer*>::const_iterator itcpu = cpus.begin();
       itcpu != cpus.end();
       ++itcpu, ++i) {
    if ((*itcpu)->GetGameId() == cpu->GetGameId()) {
      static const Color cpu_colors[8] = { primary_green_color, white_color, yellow_color, black_color,
                                           primary_blue_color, pink_color, green_color, gray_color };
      assert(i < 8);
      return cpu_colors[i];
    }
  }
  return white_color; // ie local
}

// ########################################################
static void Action_ChatMessage(Action *a)
{
  uint player_id = a->PopInt();
  std::string message = a->PopString();
  DistantComputer *cpu = a->GetCreator();
  const Team *team = NULL;
  std::string nickname = "?";
  const Player* player = (cpu) ? cpu->GetPlayer(player_id)
                               : &Network::GetInstance()->GetPlayer();

  // Search first active team
  if (player) {
    const std::list<ConfigTeam>& teams = player->GetTeams();
    std::list<ConfigTeam>::const_iterator it = teams.begin(), found = it;
    for (; it != teams.end(); ++it) {
      if (it->ai == NO_AI_NAME) {
        found = it;
        break;
      }
    }

    int unused_buffer;
    team = GetTeamsList().FindById(found->id, unused_buffer);
    nickname = team->GetPlayerName();
  }
  const Color& color = (team) ? team->GetColor()
                              : DefaultCPUColor(cpu);

  // cpu is NULL on replay, so only log if !=NULL
  if (Replay::GetConstInstance()->IsPlaying())
    ChatLogger::GetInstance()->LogMessage(nickname+"> "+message);
  AppWarmux::GetInstance()->ReceiveMsgCallback(nickname+"> "+message, color,
                                               a->GetType() == Action::ACTION_CHAT_INGAME_MESSAGE);
}

static void Action_AnnouncePause(Action *a)
{
  DistantComputer * computer = a->GetCreator();
  std::string computer_name = computer->ToString();
  std::string message = Format(_("%s needs a pause."), computer_name.c_str());
  AppWarmux::GetInstance()->ReceiveMsgCallback(message, white_color);
}

static void _Action_SelectMap(Action *a)
{
  std::string map_name = a->PopString();

  if (map_name != "random") {
    MapsList::GetInstance()->SelectMapByName(map_name);
  } else {
    MapsList::GetInstance()->SelectRandomMapByName(a->PopString());
  }

  Network *net = Network::GetInstance();
  if (net->network_menu && !net->IsGameMaster()) {
    net->network_menu->ChangeMapCallback();
  }
}

static void _Action_Game_MapList(Action *a)
{
  int                num        = a->PopInt();
  MapsList          *map_list   = MapsList::GetInstance();
  // We are going to directly update the DistantComputer map list
  std::vector<uint>& index_list = a->GetCreator()->GetAvailableMaps();

  index_list.clear();
  MSG_DEBUG("action_handler.map", "Map list size for %p: %i\n", a->GetCreator(), num);
  while (num--) {
    std::string map_name = a->PopString();
    int index = map_list->FindMapById(map_name);
    if (index != -1) {
      index_list.push_back(index);
      MSG_DEBUG("action_handler.map", "Adding map %s of index %i (%i left)\n", map_name.c_str(), index, num);
    } else
      MSG_DEBUG("action_handler.map", "Rejecting map %s  (%i left)\n", map_name.c_str(), num);
  }
}

static void Action_Game_SetMapList(Action *a)
{
  if (a)
    _Action_Game_MapList(a);
  Network  *net      = Network::GetInstance();
  if (!net->IsGameMaster()) {
    MSG_DEBUG("action_handler.map", "Dropping message from %p\n", a->GetCreator());
    return;
  }

  // This is NetworkServer being invoked here
  net->SendMapsList();
}

static void Action_Game_ForceMapList(Action *a)
{
  _Action_Game_MapList(a);

  // This list should be the common list, already a subset of our own
  // and thus we should apply it
  Network  *net      = Network::GetInstance();
  ASSERT(net->network_menu);
  net->network_menu->SetMapsCallback(a->GetCreator()->GetAvailableMaps());

  //_Action_SelectMap(a);
}

static void _Action_Game_TeamList(Action *a)
{
  int                num        = a->PopInt();
  TeamsList         *team_list  = TeamsList::GetInstance();
  // We are going to directly update the DistantComputer team list
  std::vector<uint>& index_list = a->GetCreator()->GetAvailableTeams();

  index_list.clear();
  MSG_DEBUG("action_handler.team", "Team list size for %p: %i\n", a->GetCreator(), num);
  while (num--) {
    std::string team_name = a->PopString();
    int index;
    if (team_list->FindById(team_name, index) && index!=-1) {
      index_list.push_back((uint)index);
      MSG_DEBUG("action_handler.team", "Adding team %s of index %i (%i left)\n", team_name.c_str(), index, num);
    } else
      MSG_DEBUG("action_handler.team", "Rejecting team %s  (%i left)\n", team_name.c_str(), num);
  }
}

static void Action_Game_SetTeamList(Action *a)
{
  if (a)
    _Action_Game_TeamList(a);
  Network  *net      = Network::GetInstance();
  if (!net->IsGameMaster()) {
    MSG_DEBUG("action_handler.team", "Dropping message from %p\n", a->GetCreator());
    return;
  }

  // This is NetworkServer being invoked here
  net->SendTeamsList();
}

static void Action_Game_ForceTeamList(Action *a)
{
  _Action_Game_TeamList(a);

  // This list should be the common list, already a subset of our own
  // and thus we should apply it
  Network  *net      = Network::GetInstance();
  ASSERT(net->network_menu);
  net->network_menu->SetTeamsCallback(a->GetCreator()->GetAvailableTeams());
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

static void _Action_AddTeam(Action *a, Player* player, bool skip = false)
{
  if (skip) {
    // Just skip the data
    a->PopString();
    a->PopString();
    a->PopInt();
    a->PopString();
    return;
  }

  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());
  the_team.ai = a->PopString();
  the_team.group = uint(a->PopInt());

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

  uint nb_players = a->PopInt();
  for (uint i = 0; i < nb_players; i++) {
    uint player_id = a->PopInt();

    ASSERT (a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL);
    bool skip = player_id == Network::GetInstance()->GetPlayer().GetId();
    if (!skip)
      a->GetCreator()->AddPlayer(player_id);

    uint nb_teams = a->PopInt();

    Player* player = _Action_GetPlayer(a, player_id);
    for (uint j = 0; j < nb_teams; j++) {
      _Action_AddTeam(a, player, skip);
    }
  }
}

static void Action_Game_SetMap(Action *a)
{
  FAIL_IF_GAMEMASTER(a);

  _Action_SelectMap(a);
}

static void Action_Game_AddTeam(Action *a)
{
  uint player_id = a->PopInt();
  ASSERT(Network::GetInstance()->network_menu->HasOpenTeamSlot());

  Player * player = Network::GetInstance()->LockRemoteHostsAndGetPlayer(player_id);
  if (player) {
    _Action_AddTeam(a, player);
  }
  Network::GetInstance()->UnlockRemoteHosts();
}

static void Action_Game_UpdateTeam(Action *a)
{
  uint player_id = a->PopInt();
  std::string old_team_id = a->PopString();

  ConfigTeam team_cfg;

  team_cfg.id = a->PopString();
  team_cfg.player_name = a->PopString();
  team_cfg.nb_characters = uint(a->PopInt());
  team_cfg.ai = a->PopString();
  team_cfg.group = uint(a->PopInt());

  Team* the_team = GetTeamsList().UpdateTeam(old_team_id, team_cfg);
  ASSERT(the_team != NULL);
  the_team->SetCustomCharactersNamesFromAction(a);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->UpdateTeamCallback(old_team_id, team_cfg.id);

  Player* player = _Action_GetPlayer(a, player_id);
  if (player) {
    player->UpdateTeam(old_team_id, team_cfg);
  }
}

// If you change this code, don't forget to update Network::SendTeamsList()
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

static void Action_Game_DelTeam(Action *a)
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
  a.Push(int(team.group));
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
  int group = a->PopInt();

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
      action_to_send.Push(group);

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


void WARMUX_DisconnectPlayer(Player& player)
{
  std::list<ConfigTeam>::iterator it = player.owned_teams.begin();
  while (it != player.owned_teams.end()) {
    std::string team_id = it->id;

    if (GameIsRunning()) {
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

static void Action_Character_Jump(Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ActiveCharacter().Jump();
}

static void Action_Character_HighJump(Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ActiveCharacter().HighJump();
}

static void Action_Character_BackJump(Action */*a*/)
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

static void Action_Weapon_SetTarget(Action *a)
{
  MSG_DEBUG("action_handler", "Set target by clicking");

  ActiveTeam().AccessWeapon().ChooseTarget (a->PopPoint2i());
}

static void Action_Weapon_SetTimeout(Action *a)
{
  ActiveTeam().AccessWeapon().SetProjectileTimeOut(a->PopInt());
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

static void Action_Weapon_Construction(Action *a)
{
  ActiveTeam().AccessWeapon().SetAngle(a->PopDouble());
}

// ########################################################

static void Action_Network_RandomInit(Action *a)
{
  MSG_DEBUG("random", "Initialization from network");
  RandomSync().SetSeed(a->PopInt());
}

static void Action_Network_VerifyRandomSync(Action *a)
{
  uint local_seed = RandomSync().GetSeed();
  uint remote_seed = (uint)a->PopInt();
  MSG_DEBUG("random.verify", "Verify seed: %d (local) == %d (remote)", local_seed, remote_seed);

  if (local_seed != remote_seed) {
    Replay *replay = Replay::GetInstance();
    if (replay->IsRecording())
      replay->SaveReplay(GetHome() + PATH_SEPARATOR "bug.wrf", "I'm bugged");

    Question question(Question::WARNING);
    question.Set(_("Game is not synchronized anymore! This is BAD, the network game will be "
                   "interrupted, sorry. Please, report the bug to the Warmux Team by mail or "
                   "through the forum. Precise your computer configuration and attach the "
                   "bug.wrf file that has just been created in your home folder."),
                   true, 0);
    question.Ask();
    DisconnectOnError(WRONG_SYNC);
  }
}

static void Action_Time_VerifySync(Action *a)
{
  uint local_time = GameTime::GetInstance()->Read();
  uint remote_time = (uint)a->PopInt();
  MSG_DEBUG("time.verify", "Verify time: %d (local) == %d (remote)", local_time, remote_time);
  ASSERT(local_time == remote_time);
}

// Nothing to do here. Just for time synchronisation
static void Action_Network_Ping(Action */*a*/)
{
}

// ########################################################

static void _Info_ConnectHost(const std::string& host_info)
{
  // For translators: extended in "<nickname> (<host>) just connected
  std::string msg = Format(_("%s just connected"), host_info.c_str());
  std::cerr << msg.c_str() << std::endl;

  ChatLogger::LogMessageIfOpen(msg);

  if (GameIsRunning())
    GameMessages::GetInstance()->Add(msg, primary_red_color);
  else if (Network::GetInstance()->network_menu)
    //Network Menu
    AppWarmux::GetInstance()->ReceiveMsgCallback(msg, primary_red_color);

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
        add_player_info_to_action(a, *player);
      }
    }
  }

  Network::GetInstance()->UnlockRemoteHosts();

  Action b(Action::ACTION_GAME_SET_MAP);
  MapsList::GetInstance()->FillActionMenuSetMap(b);
  Network::GetInstance()->SendActionToOne(b, client);
  Network::GetInstance()->SendActionToOne(a, client);
}

// Only used to notify clients that someone connected to the server
static void Action_Info_ClientConnect(Action *a)
{
  int player_id = a->PopInt();
  std::string host_info = a->PopString();

  ASSERT(a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL);
  a->GetCreator()->AddPlayer(player_id);

  _Info_ConnectHost(host_info);

  if (Network::GetInstance()->IsGameMaster()) {
    SendInitialGameInfo(a->GetCreator(), player_id);
  }
}

void WARMUX_ConnectHost(DistantComputer& host)
{
  std::string host_info = host.ToString();

  _Info_ConnectHost(host_info);

  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_INFO_CLIENT_CONNECT);
    ASSERT(host.GetPlayers().size() == 1);
    int player_id = host.GetPlayers().back().GetId();
    a.Push(player_id);
    a.Push(host_info);

    Network::GetInstance()->SendActionToAllExceptOne(a, &host);
  }
}

static void _Info_DisconnectHost(const std::string& host)
{
  // For translators: extended in "<nickname> (<host>) just disconnected
  std::string msg = Format(_("%s just disconnected"), host.c_str());
  fprintf(stderr, "%s\n", msg.c_str());

  ChatLogger::LogMessageIfOpen(msg);

  AppWarmux::GetInstance()->ReceiveMsgCallback(msg, primary_red_color);
}

// Used to notify clients that someone disconnected from the server
static void Action_Info_ClientDisconnect(Action *a)
{
  std::string host = a->PopString();

  _Info_DisconnectHost(host);

  int nb_players = a->PopInt();
  for (int i = 0; i < nb_players; i++) {
    int player_id = a->PopInt();
    a->GetCreator()->DelPlayer(player_id);
  }
}

void WARMUX_DisconnectHost(DistantComputer& host)
{
  std::string host_info = host.ToString();

  _Info_DisconnectHost(host_info);

  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_INFO_CLIENT_DISCONNECT);
    a.Push(host_info);
    a.Push(int(host.GetPlayers().size()));

    std::list<Player>::const_iterator player_it;
    for (player_it = host.GetPlayers().begin(); player_it != host.GetPlayers().end(); player_it++) {
      a.Push(int(player_it->GetId()));
    }
    Network::GetInstance()->SendActionToAll(a); // host is already removed from the list
  }

  // Passing NULL for action makes it not parse the action but do the remaining processing
  Action_Game_SetMapList(NULL);
}
// ########################################################
// ########################################################
// ########################################################

void Action_Handler_Init()
{
  ActionHandler *ah = ActionHandler::GetInstance();
  ah->Lock();

  // ########################################################
  ah->Register(Action::ACTION_NETWORK_CLIENT_CHANGE_STATE, "NETWORK_client_change_state", &Action_Network_ClientChangeState);
  ah->Register(Action::ACTION_NETWORK_MASTER_CHANGE_STATE, "NETWORK_master_change_state", &Action_Network_MasterChangeState);
  ah->Register(Action::ACTION_NETWORK_CHECK_PHASE1, "NETWORK_check1", &Action_Network_Check_Phase1);
  ah->Register(Action::ACTION_NETWORK_CHECK_PHASE2, "NETWORK_check2", &Action_Network_Check_Phase2);
  ah->Register(Action::ACTION_NETWORK_DISCONNECT_ON_ERROR, "NETWORK_disconnect_on_error", &Action_Network_Disconnect_On_Error);
  ah->Register(Action::ACTION_NETWORK_SET_GAME_MASTER, "NETWORK_set_game_master", &Action_Network_Set_GameMaster);
  ah->Register(Action::ACTION_CHAT_MENU_MESSAGE, "menu_chat_message", Action_ChatMessage);

  // ########################################################
  ah->Register(Action::ACTION_GAME_PACK_CALCULATED_FRAMES, "GAME_unpack_frames", &Action_Game_Unpack);
  ah->Register(Action::ACTION_GAME_CALCULATE_FRAME, "GAME_calculate_frame", &Action_Game_CalculateFrame);
  ah->Register(Action::ACTION_PLAYER_CHANGE_WEAPON, "PLAYER_change_weapon", &Action_Player_ChangeWeapon);
  ah->Register(Action::ACTION_PLAYER_CHANGE_CHARACTER, "PLAYER_change_character", &Action_Player_ChangeCharacter);

  // ########################################################
  // To be sure that rules will be the same on each computer
  ah->Register(Action::ACTION_RULES_SET_GAME_MODE, "RULES_set_game_mode", &Action_Rules_SetGameMode);

  // ########################################################
  // Chat message
  ah->Register(Action::ACTION_CHAT_INGAME_MESSAGE, "ingame_chat_message", Action_ChatMessage);
  ah->Register(Action::ACTION_ANNOUNCE_PAUSE, "chat_announce_pause", Action_AnnouncePause);

  // Initial information about the game: map, teams already selected, ...
  ah->Register(Action::ACTION_GAME_INFO, "GAME_info", &Action_Game_Info);

  // Map selection in network menu
  ah->Register(Action::ACTION_GAME_SET_MAP, "GAME_set_map", &Action_Game_SetMap);
  ah->Register(Action::ACTION_GAME_SET_MAP_LIST, "GAME_set_map_list", &Action_Game_SetMapList);
  ah->Register(Action::ACTION_GAME_FORCE_MAP_LIST, "GAME_set_map_list", &Action_Game_ForceMapList);

  // Teams selection in network menu
  ah->Register(Action::ACTION_GAME_SET_TEAM_LIST, "GAME_set_map_list", &Action_Game_SetTeamList);
  ah->Register(Action::ACTION_GAME_FORCE_TEAM_LIST, "GAME_set_map_list", &Action_Game_ForceTeamList);
  ah->Register(Action::ACTION_GAME_ADD_TEAM, "GAME_add_team", &Action_Game_AddTeam);
  ah->Register(Action::ACTION_GAME_DEL_TEAM, "GAME_del_team", &Action_Game_DelTeam);
  ah->Register(Action::ACTION_GAME_UPDATE_TEAM, "GAME_update_team", &Action_Game_UpdateTeam);
  ah->Register(Action::ACTION_GAME_REQUEST_TEAM, "GAME_request_team", &Action_Game_RequestTeam);
  ah->Register(Action::ACTION_GAME_REQUEST_TEAM_REMOVAL, "GAME_request_team_removal", &Action_Game_RequestTeamRemoval);

  // ########################################################
  // Character's move
  ah->Register(Action::ACTION_CHARACTER_JUMP, "CHARACTER_jump", &Action_Character_Jump);
  ah->Register(Action::ACTION_CHARACTER_HIGH_JUMP, "CHARACTER_super_jump", &Action_Character_HighJump);
  ah->Register(Action::ACTION_CHARACTER_BACK_JUMP, "CHARACTER_back_jump", &Action_Character_BackJump);
  ah->Register(Action::ACTION_CHARACTER_START_MOVING_LEFT, "CHARACTER_start_moving_left", &Action_Character_StartMovingLeft);
  ah->Register(Action::ACTION_CHARACTER_STOP_MOVING_LEFT, "CHARACTER_stop_moving_left", &Action_Character_StopMovingLeft);
  ah->Register(Action::ACTION_CHARACTER_START_MOVING_RIGHT, "CHARACTER_start_moving_right", &Action_Character_StartMovingRight);
  ah->Register(Action::ACTION_CHARACTER_STOP_MOVING_RIGHT, "CHARACTER_stop_moving_right", &Action_Character_StopMovingRight);
  ah->Register(Action::ACTION_CHARACTER_START_MOVING_UP, "CHARACTER_start_moving_up", &Action_Character_StartMovingUp);
  ah->Register(Action::ACTION_CHARACTER_STOP_MOVING_UP, "CHARACTER_stop_moving_up", &Action_Character_StopMovingUp);
  ah->Register(Action::ACTION_CHARACTER_START_MOVING_DOWN, "CHARACTER_start_moving_down", &Action_Character_StartMovingDown);
  ah->Register(Action::ACTION_CHARACTER_STOP_MOVING_DOWN, "CHARACTER_stop_moving_down", &Action_Character_StopMovingDown);

  // ########################################################
  // Quite standard weapon options

  ah->Register(Action::ACTION_WEAPON_START_SHOOTING, "WEAPON_start_shooting", &Action_Weapon_StartShooting);
  ah->Register(Action::ACTION_WEAPON_STOP_SHOOTING, "WEAPON_stop_shooting", &Action_Weapon_StopShooting);
  ah->Register(Action::ACTION_WEAPON_SET_TIMEOUT, "WEAPON_set_timeout", &Action_Weapon_SetTimeout);
  ah->Register(Action::ACTION_WEAPON_SET_TARGET, "WEAPON_set_target", &Action_Weapon_SetTarget);
  ah->Register(Action::ACTION_WEAPON_START_MOVING_LEFT, "WEAPON_start_moving_left", &Action_Weapon_StartMovingLeft);
  ah->Register(Action::ACTION_WEAPON_STOP_MOVING_LEFT, "WEAPON_stop_moving_left", &Action_Weapon_StopMovingLeft);
  ah->Register(Action::ACTION_WEAPON_START_MOVING_RIGHT, "WEAPON_start_moving_right", &Action_Weapon_StartMovingRight);
  ah->Register(Action::ACTION_WEAPON_STOP_MOVING_RIGHT, "WEAPON_stop_moving_right", &Action_Weapon_StopMovingRight);
  ah->Register(Action::ACTION_WEAPON_START_MOVING_UP, "WEAPON_start_moving_up", &Action_Weapon_StartMovingUp);
  ah->Register(Action::ACTION_WEAPON_STOP_MOVING_UP, "WEAPON_stop_moving_up", &Action_Weapon_StopMovingUp);
  ah->Register(Action::ACTION_WEAPON_START_MOVING_DOWN, "WEAPON_start_moving_down", &Action_Weapon_StartMovingDown);
  ah->Register(Action::ACTION_WEAPON_STOP_MOVING_DOWN, "WEAPON_stop_moving_down", &Action_Weapon_StopMovingDown);

  // Special weapon options
  ah->Register(Action::ACTION_WEAPON_CONSTRUCTION, "WEAPON_construction", &Action_Weapon_Construction);

  // Bonus box
  ah->Register(Action::ACTION_DROP_BONUS_BOX, "BONUSBOX_drop_box", &Action_DropBonusBox);
  ah->Register(Action::ACTION_REQUEST_BONUS_BOX_DROP, "BONUSBOX_request_box_drop", &Action_RequestBonusBoxDrop);
  // ########################################################
  ah->Register(Action::ACTION_NETWORK_PING, "NETWORK_ping", &Action_Network_Ping);
  ah->Register(Action::ACTION_NETWORK_RANDOM_INIT, "NETWORK_random_init", &Action_Network_RandomInit);
  ah->Register(Action::ACTION_NETWORK_VERIFY_RANDOM_SYNC, "NETWORK_verify_random_sync", &Action_Network_VerifyRandomSync);
  ah->Register(Action::ACTION_TIME_VERIFY_SYNC, "TIME_verify_sync", &Action_Time_VerifySync);
  ah->Register(Action::ACTION_INFO_CLIENT_DISCONNECT, "INFO_client_disconnect", &Action_Info_ClientDisconnect);
  ah->Register(Action::ACTION_INFO_CLIENT_CONNECT, "INFO_client_connect", &Action_Info_ClientConnect);

  // ########################################################
  ah->UnLock();
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

static bool IsFrameAction(const Action* a)
{
  return a->GetType() == Action::ACTION_GAME_CALCULATE_FRAME;
}

bool ActionHandler::ExecActionsForOneFrame()
{
  ASSERT(GameIsRunning());

  Lock();
  if (std::find_if(queue.begin(), queue.end(), IsFrameAction) == queue.end()) {
    UnLock();
    return false;
  }

  // Frame is obviously complete from now on
  std::list<Action*>::iterator it = queue.begin();
  Replay *replay = Replay::GetInstance();
  while (it != queue.end()) {
    Action * a = (*it);
    bool complete = IsFrameAction(a);

    // Do it first, else Exec will strip its content!
    if (replay->IsRecording())
      replay->StoreAction(a);
    Exec(a);

    delete *it;
    it = queue.erase(it);
    if (complete)
      break;
  }
  UnLock();

  return true;
}

void ActionHandler::ExecFrameLessActions()
{
  Action * a;
  std::list<Action*>::iterator it;
  Lock();

  for (it = queue.begin(); it != queue.end() ;) {
    a = (*it);

    // only handle frame less actions.
    if (!a->IsFrameLess()) {
      it++;
      continue;
    }

    // Do not execute actions from Network if we are not connected anymore
    if (!a->GetCreator()|| Network::IsConnected()) {

      Exec(a);

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

void ActionHandler::AddActionsFrames(uint nb, DistantComputer* cpu)
{
  ASSERT(mutex);
  printf("Adding %u frame actions\n", nb);

  // We are not locking queue because the only caller already locked it!
  while (nb) {
    Action *a = new Action(Action::ACTION_GAME_CALCULATE_FRAME);
    a->SetCreator(cpu);
    queue.push_back(a);
    nb--;
  }
}

void ActionHandler::NewActionActiveCharacter(const Team* team, int index)
{
  Action * next_character = new Action(Action::ACTION_PLAYER_CHANGE_CHARACTER);
  next_character->Push((team) ? team->GetId() : "");
  uint next_character_index = (index<0) ? ActiveCharacter().GetCharacterIndex() : index;
  next_character->Push((int)next_character_index);
  NewAction(next_character);
}
