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
 * Wormux action handler.
 *****************************************************************************/

#include "action_handler.h"
#include "action.h"
#include "character/body.h"
#include "character/move.h"
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "game/game.h"
#include "game/time.h"
#include "include/constant.h"
#include "interface/game_msg.h"
#include "network/network.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "menu/network_menu.h"
#include "network/randomsync.h"
#include "network/network.h"
#include "network/network_server.h"
#include "team/macro.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/vector2.h"
#include "weapon/construct.h"
#include "weapon/launcher.h"
#include "weapon/grapple.h"
#include "weapon/supertux.h"
#include "weapon/weapon.h"
#include "weapon/weapons_list.h"
#include "weapon/explosion.h"

ActionHandler * ActionHandler::singleton = NULL;

ActionHandler * ActionHandler::GetInstance()
{
  if (singleton == NULL)
    singleton = new ActionHandler();
  return singleton;
}

// ########################################################
// ########################################################

void Action_Nickname(Action *a)
{
  if(Network::GetInstance()->IsServer() && a->creator)
  {
      std::string nickname = a->PopString();
      std::cout<<"New nickname: " + nickname<< std::endl;
      a->creator->nickname = nickname;
  }
}

void Action_Network_ChangeState (Action *a)
{
  MSG_DEBUG("action_handler", "ChangeState");

  if (Network::GetInstance()->IsServer())
  {
    Network::network_state_t client_state = (Network::network_state_t)a->PopInt();
    
    switch (Network::GetInstance()->GetState())
    {
    case Network::NO_NETWORK:
      a->creator->SetState(DistantComputer::INITIALIZED);
      net_assert(client_state == Network::NETWORK_MENU_OK); 
      break;

    case Network::NETWORK_LOADING_DATA:
      a->creator->SetState(DistantComputer::READY);
      net_assert(client_state == Network::NETWORK_READY_TO_PLAY);
      break;

    default:
      net_assert(false)
      {
	if(a->creator) a->creator->force_disconnect = true;
	return;
      }
      break;
    }
  }

  if (Network::GetInstance()->IsClient())
  {
    Network::network_state_t server_state = (Network::network_state_t)a->PopInt();
    
    switch (Network::GetInstance()->GetState())
    {
    case Network::NETWORK_MENU_OK:
      Network::GetInstance()->SetState(Network::NETWORK_LOADING_DATA);
      net_assert(server_state == Network::NETWORK_LOADING_DATA);
      break;

    case Network::NETWORK_READY_TO_PLAY:
      Network::GetInstance()->SetState(Network::NETWORK_PLAYING);
      net_assert(server_state == Network::NETWORK_PLAYING);
      break;

    default:
       net_assert(false)
       {
	 if(a->creator) a->creator->force_disconnect = true;
	 return;
       }
    }
  }
}

// ########################################################

void Action_Player_ChangeWeapon (Action *a)
{
  ActiveTeam().SetWeapon(static_cast<Weapon::Weapon_type>(a->PopInt()));
}

void Action_Player_NextCharacter (Action *a)
{
  a->RetrieveCharacter();       // Retrieve current character's informations
  a->RetrieveCharacter();       // Retrieve next character information
  camera.FollowObject(&ActiveCharacter(), true, true);
}

void Action_Player_PreviousCharacter (Action *a)
{
  a->RetrieveCharacter();       // Retrieve current character's informations
  a->RetrieveCharacter();       // Retrieve previous character's information
  camera.FollowObject(&ActiveCharacter(), true, true);
}

void Action_GameLoop_ChangeCharacter (Action *a)
{
  a->RetrieveCharacter();
  camera.FollowObject(&ActiveCharacter(), true, true);
}

void Action_GameLoop_NextTeam (Action *a)
{
  teams_list.SetActive (a->PopString());
  assert (!ActiveCharacter().IsDead());
}

void Action_GameLoop_SetState (Action *a)
{
  GameLoop::GetInstance()->Really_SetState(GameLoop::game_loop_state_t(a->PopInt()));
}

// ########################################################

void Action_Rules_SetGameMode (Action *a)
{
  net_assert(Network::GetInstance()->IsClient())
  {
    if (a->creator)
      a->creator->force_disconnect = true;
    return;
  }

  std::string game_mode_name = a->PopString();
  std::string game_mode = a->PopString();
  std::string game_mode_objects = a->PopString();

  GameMode::GetInstance()->LoadFromString(game_mode_name,
					  game_mode,
					  game_mode_objects);
}

void SendGameMode()
{
  assert(Network::GetInstance()->IsServer());

  Action a(Action::ACTION_RULES_SET_GAME_MODE);

  std::string game_mode_name = "network(";
  game_mode_name += GameMode::GetInstance()->GetName();
  game_mode_name += ")";

  a.Push(game_mode_name);

  std::string game_mode;
  std::string game_mode_objects;

  GameMode::GetInstance()->ExportToString(game_mode,
					  game_mode_objects);
  a.Push(game_mode);
  a.Push(game_mode_objects);

  Network::GetInstance()->SendAction(&a);
}

void Action_Rules_AskVersion (Action *a)
{
  if (!Network::GetInstance()->IsClient()) return;
  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_RULES_SEND_VERSION, Constants::VERSION));
}

void Action_Rules_SendVersion (Action *a)
{
  if (!Network::GetInstance()->IsServer()) return;
  std::string version= a->PopString();
  assert(a->creator != NULL);

  if (version != Constants::VERSION)
  {
    a->creator->force_disconnect = true;
    std::string str = Format(_("%s tries to connect with a different version : client=%s, me=%s."),
                               a->creator->GetAdress().c_str(), version.c_str(), Constants::VERSION.c_str());
    Network::GetInstance()->network_menu->ReceiveMsgCallback(str);
    std::cerr << str << std::endl;
    return;
  }
  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_NETWORK_CONNECT, a->creator->GetAdress()));
  a->creator->version_checked = true;
}

// ########################################################

// TODO: Move this into network/distant_cpu.cpp
void Action_ChatMessage (Action *a)
{
  if(Network::GetInstance()->IsServer() && a->creator)
  {
    a->creator->SendChatMessage(a);
  }
  else
  {
    if(Game::GetInstance()->IsGameLaunched())
      //Add message to chat session in Game
      //    GameLoop::GetInstance()->chatsession.chat->AddText(a->PopString());
      GameLoop::GetInstance()->chatsession.NewMessage(a->PopString());
    else if (Network::GetInstance()->network_menu != NULL) {
      //Network Menu
      Network::GetInstance()->network_menu->ReceiveMsgCallback(a->PopString());
    }
  }
}

void Action_Menu_SetMap (Action *a)
{
  if (!Network::GetInstance()->IsClient()) return;
  MapsList::GetInstance()->SelectMapByName(a->PopString());

  if (Network::GetInstance()->network_menu != NULL) {
    Network::GetInstance()->network_menu->ChangeMapCallback();
  }
}

// TODO: Move this into network/distant_cpu.cpp
void Action_Menu_AddTeam (Action *a)
{
  if(Network::GetInstance()->IsServer() && a->creator)
  {
    a->creator->ManageTeam(a);
    return;
  }

  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

  MSG_DEBUG("action_handler.menu", "+ %s", the_team.id.c_str());

  teams_list.AddTeam (the_team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->AddTeamCallback(the_team.id);
}

void Action_Menu_UpdateTeam (Action *a)
{
  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

  teams_list.UpdateTeam (the_team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->UpdateTeamCallback(the_team.id);
}

// TODO: Move this into network/distant_cpu.cpp
void Action_Menu_DelTeam (Action *a)
{
  if(Network::GetInstance()->IsServer() && a->creator)
  {
    a->creator->ManageTeam(a);
    return;
  }

  std::string team = a->PopString();

  MSG_DEBUG("action_handler.menu", "- %s", team.c_str());

  teams_list.DelTeam (team);

  if (Network::GetInstance()->network_menu != NULL)
    Network::GetInstance()->network_menu->DelTeamCallback(team);
}

// ########################################################

// Send information about energy and the position of every character
void SyncCharacters()
{
  assert(Network::GetInstance()->IsServer());

  Action a_begin_sync(Action::ACTION_NETWORK_SYNC_BEGIN);
  Network::GetInstance()->SendAction(&a_begin_sync);
  TeamsList::iterator
    it=teams_list.playing_list.begin(),
    end=teams_list.playing_list.end();

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
  Network::GetInstance()->SendAction(&a_sync_end);
}

void Action_Character_Jump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().Jump();
}

void Action_Character_HighJump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().HighJump();
}

void Action_Character_BackJump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().BackJump();
}

void Action_Character_SetPhysics (Action *a)
{
  while(!a->IsEmpty())
    a->RetrieveCharacter();
}

// Send character information over the network (it's totally stupid to send it locally ;-)
void SendCharacterInfo(int team_no, int char_no)
{
  Action a(Action::ACTION_CHARACTER_SET_PHYSICS);
  a.StoreCharacter(team_no, char_no);
  Network::GetInstance()->SendAction(&a);
}

uint last_time = 0;

// Send active character information over the network (it's totally stupid to send it locally ;-)
void SendActiveCharacterInfo(bool can_be_dropped)
{
  uint current_time = Time::GetInstance()->Read();

  if (!can_be_dropped || last_time + 50 < Time::GetInstance()->Read()) {
    last_time = current_time; 
    SendCharacterInfo(ActiveCharacter().GetTeamIndex(), ActiveCharacter().GetCharacterIndex());
  }
}

// ########################################################

void Action_Weapon_Shoot (Action *a)
{
  if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING)
    return; // hack related to bug 8656

  double strength = a->PopDouble();
  double angle = a->PopDouble();
  ActiveTeam().AccessWeapon().PrepareShoot(strength, angle);
}

void Action_Weapon_StopUse(Action *a)
{
  ActiveTeam().AccessWeapon().ActionStopUse();
}

void Action_Weapon_SetTarget (Action *a)
{
  MSG_DEBUG("action.handler", "Set target by clicking");

  ActiveTeam().AccessWeapon().ChooseTarget (a->PopPoint2i());
}

void Action_Weapon_SetTimeout (Action *a)
{
  WeaponLauncher* launcher = dynamic_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  net_assert(launcher != NULL)
  {
    return;
  }
  launcher->GetProjectile()->m_timeout_modifier = a->PopInt();
}

void Action_Weapon_Supertux (Action *a)
{
  net_assert(ActiveTeam().GetWeaponType() == Weapon::WEAPON_SUPERTUX)
  {
    return;
  }
  WeaponLauncher* launcher = static_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  SuperTux* tux = static_cast<SuperTux*>(launcher->GetProjectile());

  double x, y;

  tux->SetAngle(a->PopDouble());
  x = a->PopDouble();
  y = a->PopDouble();
  tux->SetPhysXY(x, y);
  tux->SetSpeedXY(Point2d(0,0));
}

void Action_Weapon_Construction (Action *a)
{
  Construct* construct_weapon = dynamic_cast<Construct*>(&(ActiveTeam().AccessWeapon()));
  net_assert(construct_weapon != NULL)
  {
    return;
  }

  construct_weapon->SetAngle(a->PopDouble());
}

void Action_Weapon_Grapple (Action *a)
{
  Grapple* grapple = dynamic_cast<Grapple*>(&(ActiveTeam().AccessWeapon()));
  net_assert(grapple != NULL)
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
    assert(false);
  }
}

// ########################################################

void Action_Wind (Action *a)
{
  wind.SetVal (a->PopInt());
}

void Action_Network_RandomAdd (Action *a)
{
  assert(Network::GetInstance()->IsClient())
  randomSync.AddToTable(a->PopDouble());
}

void Action_Network_RandomClear (Action *a)
{
  assert(Network::GetInstance()->IsClient());
  randomSync.ClearTable();
}

void Action_Network_SyncBegin (Action *a)
{
  assert(!Network::GetInstance()->sync_lock);
  Network::GetInstance()->sync_lock = true;
}

void Action_Network_SyncEnd (Action *a)
{
  assert(Network::GetInstance()->sync_lock);
  Network::GetInstance()->sync_lock = false;
}

// Nothing to do here. Just for time synchronisation
void Action_Network_Ping(Action *a)
{
}

// Only used to notify clients that someone connected to the server
void Action_Network_Connect(Action *a)
{
  std::string msg = Format("%s just connected", a->PopString().c_str());
  if(Game::GetInstance()->IsGameLaunched())
    GameMessages::GetInstance()->Add(msg);
  else if (Network::GetInstance()->network_menu != NULL)
    //Network Menu
    Network::GetInstance()->network_menu->ReceiveMsgCallback(msg);
}

// Only used to notify clients that someone disconnected from the server
void Action_Network_Disconnect(Action *a)
{
  std::string msg = Format("%s just disconnected", a->PopString().c_str());
  if(Game::GetInstance()->IsGameLaunched())
    GameMessages::GetInstance()->Add(msg);
  else if (Network::GetInstance()->network_menu != NULL)
    //Network Menu
    Network::GetInstance()->network_menu->ReceiveMsgCallback(msg);
}

void Action_Explosion (Action *a)
{
  ExplosiveWeaponConfig config;

  Point2i pos = a->PopPoint2i();
  config.explosion_range = a->PopInt();
  config.particle_range = a->PopInt();
  config.damage = a->PopInt();
  config.blast_range = a->PopDouble();
  config.blast_force = a->PopDouble();
  std::string son = a->PopString();
  bool fire_particle = a->PopInt();
  ParticleEngine::ESmokeStyle smoke = (ParticleEngine::ESmokeStyle)a->PopInt();

  ApplyExplosion_common(pos, config, son, fire_particle, smoke);
}

// ########################################################
// ########################################################
// ########################################################

void ActionHandler::ExecActions()
{
  Action * a;
  std::list<Action*> to_remove;
  std::list<Action*>::iterator it;
  assert(mutex!=NULL);
  for(it = queue.begin(); it != queue.end() ; ++it)
  {
    SDL_LockMutex(mutex);
    a = (*it);
    //Time::GetInstance()->RefreshMaxTime((*it)->GetTimestamp());
    // If action is in the future, wait for next refresh
    if((*it)->GetTimestamp() > Time::GetInstance()->Read()) {
      SDL_UnlockMutex(mutex);
      continue;
    }
    SDL_UnlockMutex(mutex);
    Exec ((*it));
    to_remove.push_back((*it));
  }
  while(to_remove.size() != 0)
  {
    a = to_remove.front();
    to_remove.pop_front();
    queue.remove(a);
    delete(a);
  }
}

void ActionHandler::NewAction(Action* a, bool repeat_to_network)
{
  assert(mutex!=NULL);
  SDL_LockMutex(mutex);
  //  MSG_DEBUG("action.handler","New action : %s",a.out());
  //  std::cout << "New action " << a->GetType() << std::endl ;
  queue.push_back(a);
  //  std::cout << "  queue_size " << queue.size() << std::endl;
  SDL_UnlockMutex(mutex);
  if (repeat_to_network) Network::GetInstance()->SendAction(a);
}

void ActionHandler::NewActionActiveCharacter(Action* a)
{
  assert(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());
  Action a_begin_sync(Action::ACTION_NETWORK_SYNC_BEGIN);
  Network::GetInstance()->SendAction(&a_begin_sync);
  SendActiveCharacterInfo();
  NewAction(a);
  Action a_end_sync(Action::ACTION_NETWORK_SYNC_END);
  Network::GetInstance()->SendAction(&a_end_sync);
}

void ActionHandler::Register (Action::Action_t action,
		                      const std::string &name,callback_t fct)
{
  handler[action] = fct;
  action_name[action] = name;
}

void ActionHandler::Exec(Action *a)
{
  MSG_DEBUG("action_handler", "Executing action %s",GetActionName(a->GetType()).c_str());
  handler_it it=handler.find(a->GetType());
  net_assert(it != handler.end())
  {
    if(a->creator) a->creator->force_disconnect = true;
    return;
  }
  (*it->second) (a);
}

std::string ActionHandler::GetActionName (Action::Action_t action)
{
  assert(mutex!=NULL);
  SDL_LockMutex(mutex);
  name_it it=action_name.find(action);
  assert(it != action_name.end());
  SDL_UnlockMutex(mutex);
  return it->second;
}

ActionHandler::ActionHandler():
  handler(),
  action_name(),
  queue()
{
  mutex = SDL_CreateMutex();
  SDL_LockMutex(mutex);

  // ########################################################
  Register (Action::ACTION_NICKNAME, "nickname", Action_Nickname);
  Register (Action::ACTION_NETWORK_CHANGE_STATE, "NETWORK_change_state", &Action_Network_ChangeState);

  // ########################################################
  Register (Action::ACTION_PLAYER_CHANGE_WEAPON, "PLAYER_change_weapon", &Action_Player_ChangeWeapon);
  Register (Action::ACTION_PLAYER_NEXT_CHARACTER, "PLAYER_next_character", &Action_Player_NextCharacter);
  Register (Action::ACTION_PLAYER_PREVIOUS_CHARACTER, "PLAYER_previous_character", &Action_Player_PreviousCharacter);
  Register (Action::ACTION_GAMELOOP_CHANGE_CHARACTER, "GAMELOOP_change_character", &Action_GameLoop_ChangeCharacter);
  Register (Action::ACTION_GAMELOOP_NEXT_TEAM, "GAMELOOP_change_team", &Action_GameLoop_NextTeam);
  Register (Action::ACTION_GAMELOOP_SET_STATE, "GAMELOOP_set_state", &Action_GameLoop_SetState);

  // ########################################################
  // To be sure that rules will be the same on each computer
  Register (Action::ACTION_RULES_ASK_VERSION, "RULES_ask_version", &Action_Rules_AskVersion);
  Register (Action::ACTION_RULES_SEND_VERSION, "RULES_send_version", &Action_Rules_SendVersion);
  Register (Action::ACTION_RULES_SET_GAME_MODE, "RULES_set_game_mode", &Action_Rules_SetGameMode);

  // ########################################################
  // Chat message
  Register (Action::ACTION_CHAT_MESSAGE, "chat_message", Action_ChatMessage);

  // Map selection in network menu
  Register (Action::ACTION_MENU_SET_MAP, "MENU_set_map", &Action_Menu_SetMap);

  // Teams selection in network menu
  Register (Action::ACTION_MENU_ADD_TEAM, "MENU_add_team", &Action_Menu_AddTeam);
  Register (Action::ACTION_MENU_DEL_TEAM, "MENU_del_team", &Action_Menu_DelTeam);
  Register (Action::ACTION_MENU_UPDATE_TEAM, "MENU_update_team", &Action_Menu_UpdateTeam);

  // ########################################################
  // Character's move
  Register (Action::ACTION_CHARACTER_JUMP, "CHARACTER_jump", &Action_Character_Jump);
  Register (Action::ACTION_CHARACTER_HIGH_JUMP, "CHARACTER_super_jump", &Action_Character_HighJump);
  Register (Action::ACTION_CHARACTER_BACK_JUMP, "CHARACTER_back_jump", &Action_Character_BackJump);

  Register (Action::ACTION_CHARACTER_SET_PHYSICS, "CHARACTER_set_physics", &Action_Character_SetPhysics);

  // ########################################################
  // Using Weapon
  Register (Action::ACTION_WEAPON_SHOOT, "WEAPON_shoot", &Action_Weapon_Shoot);
  Register (Action::ACTION_WEAPON_STOP_USE, "WEAPON_stop_use", &Action_Weapon_StopUse);

  // Quite standard weapon options
  Register (Action::ACTION_WEAPON_SET_TIMEOUT, "WEAPON_set_timeout", &Action_Weapon_SetTimeout);
  Register (Action::ACTION_WEAPON_SET_TARGET, "WEAPON_set_target", &Action_Weapon_SetTarget);

  // Special weapon options
  Register (Action::ACTION_WEAPON_SUPERTUX, "WEAPON_supertux", &Action_Weapon_Supertux);
  Register (Action::ACTION_WEAPON_CONSTRUCTION, "WEAPON_construction", &Action_Weapon_Construction);
  Register (Action::ACTION_WEAPON_GRAPPLE, "WEAPON_grapple", &Action_Weapon_Grapple);

  // ########################################################
  Register (Action::ACTION_NETWORK_SYNC_BEGIN, "NETWORK_sync_begin", &Action_Network_SyncBegin);
  Register (Action::ACTION_NETWORK_SYNC_END, "NETWORK_sync_end", &Action_Network_SyncEnd);
  Register (Action::ACTION_NETWORK_PING, "NETWORK_ping", &Action_Network_Ping);

  Register (Action::ACTION_EXPLOSION, "explosion", &Action_Explosion);
  Register (Action::ACTION_WIND, "wind", &Action_Wind);
  Register (Action::ACTION_NETWORK_RANDOM_CLEAR, "NETWORK_clear_random", &Action_Network_RandomClear);
  Register (Action::ACTION_NETWORK_RANDOM_ADD, "NETWORK_add_random", &Action_Network_RandomAdd);
  Register (Action::ACTION_NETWORK_DISCONNECT, "NETWORK_disconnect", &Action_Network_Disconnect);
  Register (Action::ACTION_NETWORK_CONNECT, "NETWORK_connect", &Action_Network_Connect);

  // ########################################################
  SDL_UnlockMutex(mutex);
}

