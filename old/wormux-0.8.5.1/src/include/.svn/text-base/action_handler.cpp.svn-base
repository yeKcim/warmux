/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
#include "character/move.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "game/time.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/game_msg.h"
#include "network/chat.h"
#include "network/network.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
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
#include "weapon/polecat.h"
#include "weapon/supertux.h"
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

  WNet::net_game_state_t client_state = (WNet::net_game_state_t)a->PopInt();

  switch (Network::GetInstance()->GetState()) {
  case WNet::NO_NETWORK:
  case WNet::NETWORK_MENU_INIT:
    a->GetCreator()->SetState(DistantComputer::STATE_INITIALIZED);
    ASSERT(client_state == WNet::NETWORK_MENU_OK);
    break;

  case WNet::NETWORK_LOADING_DATA:
    a->GetCreator()->SetState(DistantComputer::STATE_READY);
    ASSERT(client_state == WNet::NETWORK_READY_TO_PLAY);
    break;

  case WNet::NETWORK_PLAYING:
  case WNet::NETWORK_NEXT_GAME:
    if (client_state == WNet::NETWORK_MENU_OK) {
      a->GetCreator()->SetState(DistantComputer::STATE_INITIALIZED);
    } else if (client_state == WNet::NETWORK_NEXT_GAME) {
      a->GetCreator()->SetState(DistantComputer::STATE_NEXT_GAME);
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

  Action b(Action::ACTION_NETWORK_CHECK_PHASE2);
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
  a->GetCreator()->SetState(DistantComputer::STATE_CHECKED);
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
  Character::RetrieveCharacterFromAction(a);       // Retrieve current character's information
  Character::RetrieveCharacterFromAction(a);       // Retrieve next character information
  Camera::GetInstance()->FollowObject(&ActiveCharacter(),true);
}

static void Action_Game_NextTeam (Action *a)
{
  std::string team = a->PopString();
  GetTeamsList().SetActive(team);

  Character::RetrieveCharacterFromAction(a);       // Retrieve current character's information

  ASSERT (!ActiveCharacter().IsDead());

  Camera::GetInstance()->FollowObject(&ActiveCharacter());

  // Are we turn master for next turn ?
  if (ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI())
    Network::GetInstance()->SetTurnMaster(true);
  else
    Network::GetInstance()->SetTurnMaster(false);
}

static void Action_NewBonusBox (Action *a)
{
  ObjBox * box;
  switch(a->PopInt()) {
    case 2 :
      box = new BonusBox();
      break;
    default: /* case 1 */
      box = new Medkit();
      break;
  }
  box->GetValueFromAction(a);
  Game::GetInstance()->AddNewBox(box);
}

static void Action_DropBonusBox (Action *a)
{
  ObjBox* current_box = Game::GetInstance()->GetCurrentBox();
  if (current_box != NULL) {
    current_box->GetValueFromAction(a);
    current_box->DropBox();
  }
}

static void Action_Game_SetState (Action *a)
{
  // to re-synchronize random number generator
  uint seed = (uint)(a->PopInt());
#ifdef DEBUG
  if (IsLOGGING("random")) {
    uint nb = RandomSync().GetSeed();
    MSG_DEBUG("random.get", "Action_Game_SetState(...): %d", nb);
    ASSERT(nb == seed);
  }
#endif
  RandomSync().SetSeed(seed);

  Game::game_loop_state_t state = Game::game_loop_state_t(a->PopInt());
  Game::GetInstance()->Really_SetState(state);
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

static void _Action_AddTeam(Action *a, uint player_id)
{
  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

  MSG_DEBUG("action_handler.menu", "+ %s", the_team.id.c_str());

  bool local_team = (!Network::IsConnected() || !a->GetCreator());

  GetTeamsList().AddTeam(the_team, local_team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->AddTeamCallback(the_team.id);

  Player* player = _Action_GetPlayer(a, player_id);
  if (player) {
    player->AddTeam(the_team);
  }
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

    if (a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL) {
      a->GetCreator()->AddPlayer(player_id);
    }

    uint nb_teams = a->PopInt();

    for (uint j = 0; j < nb_teams; j++) {
      _Action_AddTeam(a, player_id);
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

  if (a->GetCreator() && a->GetCreator()->GetPlayer(player_id) == NULL) {
    a->GetCreator()->AddPlayer(player_id);
  }

  _Action_AddTeam(a, player_id);
}

static void Action_Game_UpdateTeam (Action *a)
{
  uint player_id = a->PopInt();
  std::string old_team_id = a->PopString();

  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

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
  if (Game::IsRunning() && Network::GetInstance()->IsGameMaster()) {
    int i;
    Team* the_team = GetTeamsList().FindById(team_id, i);
    if (the_team == &ActiveTeam()) // we have loose the turn master!!
      Network::GetInstance()->SetTurnMaster(true);
  }

  GetTeamsList().DelTeam(team_id);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->DelTeamCallback(team_id);
}

static void Action_Game_DelTeam (Action *a)
{
  Player *player = NULL;
  uint player_id = a->PopInt();
  std::string team_id = a->PopString();

  player = _Action_GetPlayer(a, player_id);
  _Action_DelTeam(player, team_id);
}

void WORMUX_DisconnectPlayer(Player& player)
{
  std::map<const std::string, ConfigTeam>::iterator it = player.owned_teams.begin();
  while (it != player.owned_teams.end()) {
    std::string team_id = it->first;
    _Action_DelTeam(&player, team_id);
    it = player.owned_teams.begin();
  }
}

// ########################################################

// Send information about energy and the position of every character
void SyncCharacters()
{
  ASSERT(Network::GetInstance()->IsTurnMaster());

  Action a_begin_sync(Action::ACTION_NETWORK_SYNC_BEGIN);
  Network::GetInstance()->SendActionToAll(a_begin_sync);
  TeamsList::iterator
    it=GetTeamsList().playing_list.begin(),
    end=GetTeamsList().playing_list.end();

  for (int team_no = 0; it != end; ++it, ++team_no)
  {
    Team& team = **it;
    Team::iterator
        tit = team.begin(),
        tend = team.end();

    for (int char_no = 0; tit != tend; ++tit, ++char_no)
    {
      // Sync the character's position, energy, ...
      SendCharacterInfo(team_no, char_no);
    }
  }
  Action a_sync_end(Action::ACTION_NETWORK_SYNC_END);
  Network::GetInstance()->SendActionToAll(a_sync_end);
}

static void Action_Character_Jump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ASSERT(!ActiveTeam().IsLocal());
  ActiveCharacter().Jump();
}

static void Action_Character_HighJump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ASSERT(!ActiveTeam().IsLocal());
  ActiveCharacter().HighJump();
}

static void Action_Character_BackJump (Action */*a*/)
{
  Game::GetInstance()->SetCharacterChosen(true);
  ASSERT(!ActiveTeam().IsLocal());
  ActiveCharacter().BackJump();
}

static void Action_Character_SetPhysics (Action *a)
{
  while(!a->IsEmpty())
    Character::RetrieveCharacterFromAction(a);
}

void SendActiveCharacterAction(const Action& a)
{
  ASSERT(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());
  Action a_begin_sync(Action::ACTION_NETWORK_SYNC_BEGIN);
  Network::GetInstance()->SendActionToAll(a_begin_sync);
  SendActiveCharacterInfo();
  Network::GetInstance()->SendActionToAll(a);
  Action a_end_sync(Action::ACTION_NETWORK_SYNC_END);
  Network::GetInstance()->SendActionToAll(a_end_sync);
}

// Send character information over the network (it's totally stupid to send it locally ;-)
void SendCharacterInfo(int team_no, int char_no)
{
  Action a(Action::ACTION_CHARACTER_SET_PHYSICS);
  Character::StoreCharacter(&a, team_no, char_no);
  Network::GetInstance()->SendActionToAll(a);
}

uint last_time = 0;

// Send active character information over the network (it's totally stupid to send it locally ;-)
void SendActiveCharacterInfo(bool can_be_dropped)
{
  uint current_time = Time::GetInstance()->Read();

  if (!can_be_dropped || last_time + 100 < Time::GetInstance()->Read()) {
    last_time = current_time;
    SendCharacterInfo(ActiveCharacter().GetTeamIndex(), ActiveCharacter().GetCharacterIndex());
  }
}

// ########################################################

static void Action_Weapon_Shoot (Action *a)
{
  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return; // hack related to bug 8656

  double strength = a->PopDouble();
  double angle = a->PopDouble();
  ActiveTeam().AccessWeapon().PrepareShoot(strength, angle);
}

static void Action_Weapon_StopUse(Action */*a*/)
{
  ActiveTeam().AccessWeapon().ActionStopUse();
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
  launcher->GetProjectile()->m_timeout_modifier = a->PopInt();
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

static void Action_Weapon_Gnu (Action *a)
{
  GnuLauncher* launcher = dynamic_cast<GnuLauncher*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(launcher != NULL)
  {
    return;
  }

  Point2d pos(a->PopPoint2d());
  launcher->ExplosionFromNetwork(pos);
}

static void Action_Weapon_Grapple (Action *a)
{
  Grapple* grapple = dynamic_cast<Grapple*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(grapple != NULL)
  {
    return;
  }

  int subaction = a->PopInt();
  switch (subaction) {
  case Grapple::ATTACH_ROPE:
    {// attach rope
      Point2i contact_point = a->PopPoint2i();
      grapple->AttachRope(contact_point);
    }
    break;

  case Grapple::ATTACH_NODE: // attach node
    {
      Point2i contact_point = a->PopPoint2i();
      double angle = a->PopDouble();
      int sense = a->PopInt();
      grapple->AttachNode(contact_point, angle, sense);
    }
    break;

  case Grapple::DETACH_NODE: // detach last node
    grapple->DetachNode();
    break;

  case Grapple::SET_ROPE_SIZE: // update rope size
    grapple->SetRopeSize(a->PopDouble());
    break;

  default:
    ASSERT(false);
  }
}

static void Action_Weapon_Polecat (Action *a)
{
  PolecatLauncher* launcher = dynamic_cast<PolecatLauncher*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(launcher != NULL)
  {
    return;
  }

  Point2d pos(a->PopPoint2d());
  launcher->ExplosionFromNetwork(pos);
}

static void Action_Weapon_Supertux (Action *a)
{
  TuxLauncher* launcher = dynamic_cast<TuxLauncher*>(&(ActiveTeam().AccessWeapon()));
  NET_ASSERT(launcher != NULL)
  {
    return;
  }

  int subaction = a->PopInt();

  if (subaction == 0) {
    double angle = a->PopDouble();
    Point2d pos(a->PopPoint2d());
    launcher->RefreshFromNetwork(angle, pos);
  } else if (subaction == 1) {
    Point2d pos(a->PopPoint2d());
    launcher->ExplosionFromNetwork(pos);
  } else {
    ASSERT(false);
  }
}

// ########################################################

static void Action_Wind (Action *a)
{
  Wind::GetRef().SetVal (a->PopInt());
}

static void Action_Network_RandomInit (Action *a)
{
  MSG_DEBUG("random", "Initialization from network");
  RandomSync().SetSeed(a->PopInt());
}

static void Action_Network_SyncBegin (Action */*a*/)
{
  ASSERT(!Network::GetInstance()->sync_lock);
  Network::GetInstance()->sync_lock = true;
}

static void Action_Network_SyncEnd (Action */*a*/)
{
  ASSERT(Network::GetInstance()->sync_lock);
  Network::GetInstance()->sync_lock = false;
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

static inline void add_team_config_to_action(Action& a, const ConfigTeam& team)
{
  a.Push(team.id);
  a.Push(team.player_name);
  a.Push(int(team.nb_characters));
}

static inline void add_player_info_to_action(Action& a, const Player& player)
{
  a.Push(int(player.GetId()));
  a.Push(int(player.GetNbTeams()));

  std::map<const std::string, ConfigTeam>::const_iterator team;
  for (team = player.GetTeams().begin(); team != player.GetTeams().end(); team++) {
    add_team_config_to_action(a, team->second);
  }
}

void SendInitialGameInfo(DistantComputer* client)
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
    nb_players += (*it)->GetPlayers().size();
  }

  a.Push(nb_players);

  // Teams infos of each player
  add_player_info_to_action(a, Network::GetInstance()->GetPlayer());

  for (it = hosts.begin(); it != hosts.end(); it++) {

    const std::list<Player>& players = (*it)->GetPlayers();

    for (player = players.begin(); player != players.end(); player++) {
      add_player_info_to_action(a, (*player));
    }
  }

  Network::GetInstance()->UnlockRemoteHosts();

  Network::GetInstance()->SendActionToOne(a, client);
}

// Only used to notify clients that someone connected to the server
static void Action_Info_ClientConnect(Action *a)
{
  std::string hostname = a->PopString();
  std::string nicknames = a->PopString();

  _Info_ConnectHost(hostname, nicknames);

  if (Network::GetInstance()->IsGameMaster()) {
    SendInitialGameInfo(a->GetCreator());
  }
}

void WORMUX_ConnectHost(DistantComputer& host)
{
  std::string hostname = host.GetAddress();
  std::string nicknames = host.GetNicknames();

  _Info_ConnectHost(hostname, nicknames);

  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_INFO_CLIENT_CONNECT);
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

static void Action_Explosion (Action *a)
{
  ExplosiveWeaponConfig config;
  MSG_DEBUG("action_handler","-> Begin");

  Point2i pos = a->PopPoint2i();
  config.explosion_range = a->PopInt();
  config.particle_range = a->PopInt();
  config.damage = a->PopInt();
  config.blast_range = a->PopInt();
  config.blast_force = a->PopInt();
  std::string son = a->PopString();
  bool fire_particle = !!a->PopInt();
  ParticleEngine::ESmokeStyle smoke = (ParticleEngine::ESmokeStyle)a->PopInt();
  std::string unique_id = a->PopString();

  ApplyExplosion_common(pos, config, son, fire_particle, smoke, unique_id);

  MSG_DEBUG("action_handler","<- End");
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
  ActionHandler::GetInstance()->Register (Action::ACTION_GAMELOOP_NEXT_TEAM, "GAMELOOP_change_team", &Action_Game_NextTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAMELOOP_SET_STATE, "GAMELOOP_set_state", &Action_Game_SetState);

  // ########################################################
  // To be sure that rules will be the same on each computer
  ActionHandler::GetInstance()->Register (Action::ACTION_RULES_SET_GAME_MODE, "RULES_set_game_mode", &Action_Rules_SetGameMode);

  // ########################################################
  // Chat message
  ActionHandler::GetInstance()->Register (Action::ACTION_CHAT_MESSAGE, "chat_message", Action_ChatMessage);

  // Initial information about the game: map, teams already selected, ...
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_INFO, "GAME_info", &Action_Game_Info);

  // Map selection in network menu
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_SET_MAP, "GAME_set_map", &Action_Game_SetMap);

  // Teams selection in network menu
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_ADD_TEAM, "GAME_add_team", &Action_Game_AddTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_DEL_TEAM, "GAME_del_team", &Action_Game_DelTeam);
  ActionHandler::GetInstance()->Register (Action::ACTION_GAME_UPDATE_TEAM, "GAME_update_team", &Action_Game_UpdateTeam);

  // ########################################################
  // Character's move
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_JUMP, "CHARACTER_jump", &Action_Character_Jump);
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_HIGH_JUMP, "CHARACTER_super_jump", &Action_Character_HighJump);
  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_BACK_JUMP, "CHARACTER_back_jump", &Action_Character_BackJump);

  ActionHandler::GetInstance()->Register (Action::ACTION_CHARACTER_SET_PHYSICS, "CHARACTER_set_physics", &Action_Character_SetPhysics);

  // ########################################################
  // Using Weapon
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SHOOT, "WEAPON_shoot", &Action_Weapon_Shoot);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_STOP_USE, "WEAPON_stop_use", &Action_Weapon_StopUse);

  // Quite standard weapon options
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SET_TIMEOUT, "WEAPON_set_timeout", &Action_Weapon_SetTimeout);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SET_TARGET, "WEAPON_set_target", &Action_Weapon_SetTarget);

  // Special weapon options
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_CONSTRUCTION, "WEAPON_construction", &Action_Weapon_Construction);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_GNU, "WEAPON_gnu", &Action_Weapon_Gnu);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_GRAPPLE, "WEAPON_grapple", &Action_Weapon_Grapple);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_POLECAT, "WEAPON_polecat", &Action_Weapon_Polecat);
  ActionHandler::GetInstance()->Register (Action::ACTION_WEAPON_SUPERTUX, "WEAPON_supertux", &Action_Weapon_Supertux);

  // Bonus box
  ActionHandler::GetInstance()->Register (Action::ACTION_NEW_BONUS_BOX, "BONUSBOX_new_box", &Action_NewBonusBox);
  ActionHandler::GetInstance()->Register (Action::ACTION_DROP_BONUS_BOX, "BONUSBOX_drop_box", &Action_DropBonusBox);
  // ########################################################
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_SYNC_BEGIN, "NETWORK_sync_begin", &Action_Network_SyncBegin);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_SYNC_END, "NETWORK_sync_end", &Action_Network_SyncEnd);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_PING, "NETWORK_ping", &Action_Network_Ping);

  ActionHandler::GetInstance()->Register (Action::ACTION_EXPLOSION, "explosion", &Action_Explosion);
  ActionHandler::GetInstance()->Register (Action::ACTION_WIND, "wind", &Action_Wind);
  ActionHandler::GetInstance()->Register (Action::ACTION_NETWORK_RANDOM_INIT, "NETWORK_random_init", &Action_Network_RandomInit);
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

void ActionHandler::ExecActions()
{
  Action * a;
  std::list<Action*>::iterator it;
  Lock();
  for (it = queue.begin(); it != queue.end() ;)
  {
    a = (*it);
    //Time::GetInstance()->RefreshMaxTime((*it)->GetTimestamp());
    // If action is in the future, wait for next refresh
    if (a->GetTimestamp() > Time::GetInstance()->Read()) {
      it++;
      continue;
    }

    // Do not execute actions from Network if we are not connected anymore
    if (!a->GetCreator()
	|| Network::IsConnected()) {

      MSG_DEBUG("action_time", "-> Action %s (action time: %u, time: %u)",
		GetActionName(a->GetType()).c_str(), a->GetTimestamp(),
		Time::GetInstance()->Read());

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

void ActionHandler::NewActionActiveCharacter(Action* a)
{
  ASSERT(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());
  Action a_begin_sync(Action::ACTION_NETWORK_SYNC_BEGIN);
  Network::GetInstance()->SendActionToAll(a_begin_sync);
  SendActiveCharacterInfo();
  NewAction(a);
  Action a_end_sync(Action::ACTION_NETWORK_SYNC_END);
  Network::GetInstance()->SendActionToAll(a_end_sync);
}

