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
 * Wormux action handler.
 *****************************************************************************/

#include "action_handler.h"
#include "action.h"
#include "../character/body.h"
#include "../character/move.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../game/game.h"
#include "../game/time.h"
#include "../include/constant.h"
#include "../network/network.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/wind.h"
#include "../menu/network_menu.h"
#include "../network/randomsync.h"
#include "../network/network.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/vector2.h"
#include "../weapon/construct.h"
#include "../weapon/launcher.h"
#include "../weapon/supertux.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"
#include "../weapon/explosion.h"

// Delta angle used to move the crosshair
#define DELTA_CROSSHAIR 0.035 /* ~1 degree */

ActionHandler * ActionHandler::singleton = NULL;

ActionHandler * ActionHandler::GetInstance()
{
  if (singleton == NULL)
    singleton = new ActionHandler();
  return singleton;
}

// Send information about energy and the position of every character
void SyncCharacters()
{
  assert(network.IsServer());
  ActionHandler* action_handler = ActionHandler::GetInstance();

  Action a_begin_sync(Action::ACTION_SYNC_BEGIN);
  network.SendAction(&a_begin_sync);
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
      // Sync the character's energy
      Action* a = new Action(Action::ACTION_SET_CHARACTER_ENERGY);
      a->Push(team_no);
      a->Push(char_no);
      a->Push((int)(*tit).GetEnergy());
      action_handler->NewAction(a);
      // Sync the character's position
      a = BuildActionSendCharacterPhysics(team_no, char_no);
      action_handler->NewAction(a);
    }
  }
  Action a_sync_end(Action::ACTION_SYNC_END);
  network.SendAction(&a_sync_end);
}

void Action_MoveRight (Action *a)
{
  assert(false);
  MoveCharacterRight (ActiveCharacter());
}

void Action_MoveLeft (Action *a)
{
  assert(false);
  MoveCharacterLeft (ActiveCharacter());
}

void Action_Jump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().Jump();
}

void Action_HighJump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().HighJump();
}

void Action_BackJump (Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().BackJump();
}

void Action_Up (Action *a)
{
  ActiveCharacter().AddFiringAngle(-DELTA_CROSSHAIR);
}

void Action_Down (Action *a)
{
  ActiveCharacter().AddFiringAngle(DELTA_CROSSHAIR);
}

void Action_ChangeWeapon (Action *a)
{
  ActiveTeam().SetWeapon(static_cast<Weapon::Weapon_type>(a->PopInt()));
}

void Action_NextCharacter (Action *a)
{
  a->RetrieveCharacter();       // Retrieve current character's informations
  a->RetrieveCharacter();       // Retrieve next character information
  camera.FollowObject(&ActiveCharacter(), true, true);
}

void Action_ChangeCharacter (Action *a)
{
  a->RetrieveCharacter();
  camera.FollowObject(&ActiveCharacter(), true, true);
}

void Action_Shoot (Action *a)
{
  double strength = a->PopDouble();
  double angle = a->PopDouble();
  a->RetrieveCharacter();
  ActiveTeam().AccessWeapon().PrepareShoot(strength, angle);
}

void Action_Wind (Action *a)
{
  wind.SetVal (a->PopInt());
}

Action* BuildActionSendCharacterPhysics(int team_no, int char_no)
{
  Action* a = new Action(Action::ACTION_SET_CHARACTER_PHYSICS);
  a->StoreCharacter(team_no, char_no);
  return a;
}

void Action_SetCharacterPhysics (Action *a)
{
  while(!a->IsEmpty())
    a->RetrieveCharacter();
}

void Action_SetCharacterEnergy(Action *a)
{
  int team_no, char_no;
  team_no = a->PopInt();
  char_no = a->PopInt();
  Character* c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  c->SetEnergy( a->PopInt() );
}

void Action_SetSkin (Action *a)
{
  //Set the frame of the walking skin, to get the position of the hand synced
  if (!ActiveTeam().IsLocal() || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().SetClothe(a->PopString());
    ActiveTeam().ActiveCharacter().SetMovement(a->PopString());
    ActiveTeam().ActiveCharacter().body->SetFrame((uint)a->PopInt());
  }
}

void Action_SetCharacterDirection (Action *a)
{
  ActiveCharacter().SetDirection (Body::Direction_t(a->PopInt()));
}

void Action_SetMap (Action *a)
{
  if (!network.IsClient()) return;
  MapsList::GetInstance()->SelectMapByName(a->PopString());
  network.network_menu->ChangeMapCallback();
}

void Action_ChangeState (Action *a)
{
  MSG_DEBUG("action.handler", "ChangeState");

  if(network.IsServer())
  {
    switch(network.state)
    {
    case Network::NETWORK_OPTION_SCREEN:
      // State is changed when server clicks on the launch game button
      network.client_inited++;
      break;
    case Network::NETWORK_INIT_GAME:
      // One more client is ready to play
      network.client_inited++;
      if(network.client_inited == network.connected_player)
        network.state = Network::NETWORK_READY_TO_PLAY;
      break;
    default:
      assert(false);
      break;
    }
  }

  if(network.IsClient())
  {
    switch(network.state)
    {
    case Network::NETWORK_OPTION_SCREEN:
      network.state = Network::NETWORK_INIT_GAME;
      break;
    case Network::NETWORK_INIT_GAME:
      network.state = Network::NETWORK_READY_TO_PLAY;
      break;
    case Network::NETWORK_READY_TO_PLAY:
      network.state = Network::NETWORK_PLAYING;
      break;
    default:
       assert(false);
    }
  }
}

void Action_SetGameMode (Action *a)
{
  assert(network.IsClient());
  GameMode::GetInstance()->max_characters = a->PopInt();
  GameMode::GetInstance()->max_teams = a->PopInt();
  GameMode::GetInstance()->duration_turn = a->PopInt();
  GameMode::GetInstance()->duration_exchange_player = a->PopInt();
  GameMode::GetInstance()->duration_before_death_mode = a->PopInt();
  GameMode::GetInstance()->gravity = a->PopDouble();
  GameMode::GetInstance()->safe_fall = a->PopDouble();
  GameMode::GetInstance()->damage_per_fall_unit = a->PopDouble();
  GameMode::GetInstance()->duration_move_player = a->PopInt();
  GameMode::GetInstance()->allow_character_selection = a->PopInt();
  GameMode::GetInstance()->character.init_energy = a->PopInt();
  GameMode::GetInstance()->character.max_energy = a->PopInt();
  GameMode::GetInstance()->character.mass = a->PopInt();
  GameMode::GetInstance()->character.air_resist_factor = a->PopDouble();
  GameMode::GetInstance()->character.jump_strength = a->PopInt();
  GameMode::GetInstance()->character.jump_angle = a->PopDouble();
  GameMode::GetInstance()->character.super_jump_strength = a->PopInt();
  GameMode::GetInstance()->character.super_jump_angle = a->PopDouble();
  GameMode::GetInstance()->character.back_jump_strength = a->PopInt();
  GameMode::GetInstance()->character.back_jump_angle = a->PopDouble();
}

void SendGameMode()
{
  assert(network.IsServer());
  Action a(Action::ACTION_SET_GAME_MODE);
  a.Push((int)GameMode::GetInstance()->max_characters);
  a.Push((int)GameMode::GetInstance()->max_teams);
  a.Push((int)GameMode::GetInstance()->duration_turn);
  a.Push((int)GameMode::GetInstance()->duration_exchange_player);
  a.Push((int)GameMode::GetInstance()->duration_before_death_mode);
  a.Push(GameMode::GetInstance()->gravity);
  a.Push(GameMode::GetInstance()->safe_fall);
  a.Push(GameMode::GetInstance()->damage_per_fall_unit);
  a.Push((int)GameMode::GetInstance()->duration_move_player);
  a.Push(GameMode::GetInstance()->allow_character_selection);
  a.Push((int)GameMode::GetInstance()->character.init_energy);
  a.Push((int)GameMode::GetInstance()->character.max_energy);
  a.Push((int)GameMode::GetInstance()->character.mass);
  a.Push(GameMode::GetInstance()->character.air_resist_factor);
  a.Push((int)GameMode::GetInstance()->character.jump_strength);
  a.Push(GameMode::GetInstance()->character.jump_angle);
  a.Push((int)GameMode::GetInstance()->character.super_jump_strength);
  a.Push(GameMode::GetInstance()->character.super_jump_angle);
  a.Push((int)GameMode::GetInstance()->character.back_jump_strength);
  a.Push(GameMode::GetInstance()->character.back_jump_angle);
  network.SendAction(&a);
}

// TODO: Move this into network/distant_cpu.cpp
void Action_NewTeam (Action *a)
{
  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

  teams_list.AddTeam (the_team);

  network.network_menu->AddTeamCallback(the_team.id);
}

void Action_UpdateTeam (Action *a)
{
  ConfigTeam the_team;

  the_team.id = a->PopString();
  the_team.player_name = a->PopString();
  the_team.nb_characters = uint(a->PopInt());

  teams_list.UpdateTeam (the_team);

  network.network_menu->UpdateTeamCallback(the_team.id);
}

// TODO: Move this into network/distant_cpu.cpp
void Action_DelTeam (Action *a)
{
  std::string team = a->PopString();
  teams_list.DelTeam (team);
  network.network_menu->DelTeamCallback(team);
}

// TODO: Move this into network/distant_cpu.cpp
void Action_ChatMessage (Action *a)
{
  if(Game::GetInstance()->IsGameLaunched())
    //Add message to chat session in Game
    //    GameLoop::GetInstance()->chatsession.chat->AddText(a->PopString());
    GameLoop::GetInstance()->chatsession.NewMessage(a->PopString());
  else
    //Network Menu
    network.network_menu->ReceiveMsgCallback(a->PopString());
}

void Action_ChangeTeam (Action *a)
{
  teams_list.SetActive (a->PopString());
  ActiveTeam().PrepareTurn();
  assert (!ActiveCharacter().IsDead());
}

void Action_AskVersion (Action *a)
{
  if (!network.IsClient()) return;
  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_SEND_VERSION, Constants::VERSION));
}

void Action_SendVersion (Action *a)
{
  if (!network.IsServer()) return;
  std::string version= a->PopString();
  if (version != Constants::VERSION)
  {
    Error(Format(_("Wormux versions are differents : client=%s, server=%s."),
    version.c_str(), Constants::VERSION.c_str()));
  }
}

void Action_SendRandom (Action *a)
{
  if (!network.IsClient()) return;
  randomSync.AddToTable(a->PopDouble());
}

void Action_SupertuxState (Action *a)
{
  assert(ActiveTeam().GetWeaponType() == Weapon::WEAPON_SUPERTUX);
  WeaponLauncher* launcher = static_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  SuperTux* tux = static_cast<SuperTux*>(launcher->GetProjectile());

  double x, y;

  tux->SetAngle(a->PopDouble());
  x = a->PopDouble();
  y = a->PopDouble();
  tux->SetPhysXY(x, y);
  tux->SetSpeedXY(Point2d(0,0));
}

void Action_SyncBegin (Action *a)
{
  assert(!network.sync_lock);
  network.sync_lock = true;
}

void Action_SyncEnd (Action *a)
{
  assert(network.sync_lock);
  network.sync_lock = false;
}

// Nothing to do here. Just for time synchronisation
void Action_Ping(Action *a)
{
}

void Action_Explosion (Action *a)
{
  Point2i pos;
  ExplosiveWeaponConfig config;

  pos.x = a->PopInt();
  pos.y = a->PopInt();
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

void Action_SetTarget (Action *a)
{
  MSG_DEBUG("action.handler", "Set target by clicking");

  Point2i target;
  target.x = a->PopInt();
  target.y = a->PopInt();

  ActiveTeam().AccessWeapon().ChooseTarget (target);
}

void Action_SetTimeout (Action *a)
{
  WeaponLauncher* launcher = dynamic_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  assert(launcher != NULL);
  launcher->GetProjectile()->m_timeout_modifier = a->PopInt();
}

void Action_ConstructionUp (Action *a)
{
  Construct* launcher = dynamic_cast<Construct*>(&(ActiveTeam().AccessWeapon()));
  assert(launcher != NULL);
  launcher->Up();
}

void Action_ConstructionDown (Action *a)
{
  Construct* launcher = dynamic_cast<Construct*>(&(ActiveTeam().AccessWeapon()));
  assert(launcher != NULL);
  launcher->Down();
}

void Action_WeaponStopUse(Action *a)
{
  ActiveTeam().AccessWeapon().ActionStopUse();
}

void Action_Nickname(Action *a)
{

}

void Action_Pause(Action *a)
{
  // Toggle pause
  Game::GetInstance()->Pause();
}

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
    Time::GetInstance()->RefreshMaxTime((*it)->GetTimestamp());
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
  if (repeat_to_network) network.SendAction(a);
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
  assert(it != handler.end());
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

ActionHandler::ActionHandler()
{
  mutex = SDL_CreateMutex();
  SDL_LockMutex(mutex);
  Register (Action::ACTION_MOVE_LEFT, "move_left", &Action_MoveLeft);
  Register (Action::ACTION_MOVE_RIGHT, "move_right", &Action_MoveRight);
  Register (Action::ACTION_UP, "up", &Action_Up);
  Register (Action::ACTION_DOWN, "down", &Action_Down);
  Register (Action::ACTION_JUMP, "jump", &Action_Jump);
  Register (Action::ACTION_HIGH_JUMP, "super_jump", &Action_HighJump);
  Register (Action::ACTION_BACK_JUMP, "back_jump", &Action_BackJump);
  Register (Action::ACTION_SHOOT, "shoot", &Action_Shoot);
  Register (Action::ACTION_CHANGE_WEAPON, "change_weapon", &Action_ChangeWeapon);
  Register (Action::ACTION_WIND, "wind", &Action_Wind);
  Register (Action::ACTION_NEXT_CHARACTER, "next_character", &Action_NextCharacter);
  Register (Action::ACTION_CHANGE_CHARACTER, "change_character", &Action_ChangeCharacter);
  Register (Action::ACTION_SET_GAME_MODE, "set_game_mode", &Action_SetGameMode);
  Register (Action::ACTION_SET_MAP, "set_map", &Action_SetMap);
  Register (Action::ACTION_UPDATE_TEAM, "update_team", &Action_UpdateTeam);
  Register (Action::ACTION_NEW_TEAM, "new_team", &Action_NewTeam);
  Register (Action::ACTION_DEL_TEAM, "del_team", &Action_DelTeam);
  Register (Action::ACTION_CHANGE_TEAM, "change_team", &Action_ChangeTeam);
  Register (Action::ACTION_SET_CHARACTER_PHYSICS, "set_character_physics", &Action_SetCharacterPhysics);
  Register (Action::ACTION_SET_SKIN, "set_skin", &Action_SetSkin);
  Register (Action::ACTION_SET_CHARACTER_DIRECTION, "set_character_direction", &Action_SetCharacterDirection);
  Register (Action::ACTION_CHANGE_STATE, "change_state", &Action_ChangeState);
  Register (Action::ACTION_ASK_VERSION, "ask_version", &Action_AskVersion);
  Register (Action::ACTION_SEND_VERSION, "send_version", &Action_SendVersion);
  Register (Action::ACTION_SEND_RANDOM, "send_random", &Action_SendRandom);
  Register (Action::ACTION_PING, "ping", &Action_Ping);
  Register (Action::ACTION_SYNC_BEGIN, "sync_begin", &Action_SyncBegin);
  Register (Action::ACTION_SYNC_END, "sync_end", &Action_SyncEnd);
  Register (Action::ACTION_EXPLOSION, "explosion", &Action_Explosion);
  Register (Action::ACTION_SET_TARGET, "set_target", &Action_SetTarget);
  Register (Action::ACTION_SUPERTUX_STATE, "supertux_state", &Action_SupertuxState);
  Register (Action::ACTION_SET_TIMEOUT, "set_timeout", &Action_SetTimeout);
  Register (Action::ACTION_CONSTRUCTION_UP, "construction_up", &Action_ConstructionUp);
  Register (Action::ACTION_CONSTRUCTION_DOWN, "construction_down", &Action_ConstructionDown);
  Register (Action::ACTION_WEAPON_STOP_USE, "weapon_stop_use", &Action_WeaponStopUse);
  Register (Action::ACTION_SET_CHARACTER_ENERGY, "set_character_energy", &Action_SetCharacterEnergy);
  Register (Action::ACTION_CHAT_MESSAGE, "chat_message", Action_ChatMessage);
  Register (Action::ACTION_NICKNAME, "nickname", Action_Nickname);
  Register (Action::ACTION_PAUSE, "pause", Action_Pause);
  SDL_UnlockMutex(mutex);
}

