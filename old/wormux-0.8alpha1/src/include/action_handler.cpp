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
#include "../include/constant.h"
#include "../network/network.h"
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
#define DELTA_CROSSHAIR 2

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

  Action a_begin_sync(ACTION_SYNC_BEGIN);
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
      Action* a = new Action(ACTION_SET_CHARACTER_ENERGY);
		a->Push(team_no);
		a->Push(char_no);
      a->Push((int)(*tit).GetEnergy());
      action_handler->NewAction(a);
      // Sync the character's position
      a = BuildActionSendCharacterPhysics(team_no, char_no);
      action_handler->NewAction(a);
    }
  }
  Action a_sync_end(ACTION_SYNC_END);
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
  ActiveTeam().crosshair.ChangeAngle (-DELTA_CROSSHAIR);
}

void Action_Down (Action *a)
{
  ActiveTeam().crosshair.ChangeAngle (DELTA_CROSSHAIR);
}

void Action_ChangeWeapon (Action *a)
{
  ActiveTeam().SetWeapon((Weapon_type)a->PopInt());
}

void Action_NextCharacter (Action *a)
{
  ActiveTeam().NextCharacter();
}

void Action_ChangeCharacter (Action *a)
{
  int char_index = a->PopInt();
  while( (int)ActiveCharacter().GetCharacterIndex() != char_index )
    ActiveTeam().NextCharacter();

  printf("Action_NextCharacter:\n");
  printf("char_index = %i\n",char_index);
  printf("Playing character : %i %s\n", ActiveCharacter().GetCharacterIndex(), ActiveCharacter().GetName().c_str());
  printf("Playing team : %i %s\n", ActiveCharacter().GetTeamIndex(), ActiveTeam().GetName().c_str());
  printf("Alive characters: %i / %i\n\n",ActiveTeam().NbAliveCharacter(),ActiveTeam().GetNbCharacters());

  // Ok, this code is dirty, if you write a cleaner way
  // Use it in mouse.cpp as well ;)
}

void Action_Shoot (Action *a)
{
  double strength = a->PopDouble();
  int angle = a->PopInt();
  ActiveTeam().AccessWeapon().PrepareShoot(strength, angle);
}

void Action_Wind (Action *a)
{
  wind.SetVal (a->PopInt());
}

Action* BuildActionSendCharacterPhysics(int team_no, int char_no)
{
  Action* a = new Action(ACTION_SET_CHARACTER_PHYSICS);
  Character* c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  a->Push(team_no);
  a->Push(char_no);
  a->Push(c->GetX());
  a->Push(c->GetY());
  Point2d speed;
  c->GetSpeedXY(speed);
  a->Push(speed.x);
  a->Push(speed.y);
//  printf("Sending physics of %s (%i, %i / %f, %f)\n",c->GetName().c_str(), c->GetX(), c->GetY(), speed.x, speed.y);
  return a;
}

void Action_SetCharacterPhysics (Action *a)
{
  int team_no, char_no;
  double s_x, s_y;
  int x, y;

  team_no = a->PopInt();
  char_no = a->PopInt();
  Character* c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  assert(c != NULL);

  x = a->PopInt();
  y = a->PopInt();
  s_x = a->PopDouble();
  s_y = a->PopDouble();
  c->SetXY(Point2i(x, y));
  c->SetSpeedXY(Point2d(s_x, s_y));

  Point2d speed;
  c->GetSpeedXY(speed);
  printf("Setting physics of %s (%i, %i / %f, %f)\n",c->GetName().c_str(), c->GetX(), c->GetY(), speed.x, speed.y);
}

void Action_SetCharacterEnergy(Action *a)
{
  int team_no, char_no;
  team_no = a->PopInt();
  char_no = a->PopInt();
  Character* c = teams_list.FindPlayingByIndex(team_no)->FindByIndex(char_no);
  c->SetEnergy( a->PopInt() );
}

void Action_SetFrame (Action *a)
{
  //Set the frame of the walking skin, to get the position of the hand synced
  if (!ActiveTeam().is_local || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().body->SetFrame((uint)a->PopInt());
  }
}

void Action_SetClothe (Action *a)
{
  if (!ActiveTeam().is_local || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().SetClothe(a->PopString());
  }
}

void Action_SetMovement (Action *a)
{
  if (!ActiveTeam().is_local || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().SetMovement(a->PopString());
  }
}

void Action_SetCharacterDirection (Action *a)
{
  ActiveCharacter().SetDirection (a->PopInt());
}

void Action_SetMap (Action *a)
{
  if (!network.IsClient()) return;
  MapsList::GetInstance()->SelectMapByName(a->PopString());
  network.network_menu->ChangeMapCallback();
}

void Action_ClearTeams (Action *a)
{
  MSG_DEBUG("action.handler", "ClearTeams");
  if (!network.IsClient()) return;
  teams_list.Clear();
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
  GameMode::GetInstance()->character.jump_angle = a->PopInt();
  GameMode::GetInstance()->character.super_jump_strength = a->PopInt();
  GameMode::GetInstance()->character.super_jump_angle = a->PopInt();
  GameMode::GetInstance()->character.back_jump_strength = a->PopInt();
  GameMode::GetInstance()->character.back_jump_angle = a->PopInt();
}

void SendGameMode()
{
  assert(network.IsServer());
  Action a(ACTION_SET_GAME_MODE);
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

void Action_NewTeam (Action *a)
{
  std::string team = a->PopString();
  teams_list.AddTeam (team);
  network.network_menu->AddTeamCallback(team);
}

void Action_DelTeam (Action *a)
{
  std::string team = a->PopString();
  teams_list.DelTeam (team);
  network.network_menu->DelTeamCallback(team);
}

void Action_ChatMessage (Action *a)
{
  if(Game::GetInstance()->IsGameLaunched())
    //Add message to chat session in Game
    //    GameLoop::GetInstance()->chatsession.chat->AddText(a->PopString());
    GameLoop::GetInstance()->chatsession.NewMessage(a->PopString());
  else
    //Network Menu
    network.network_menu->msg_box->NewMessage(a->PopString());
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
  ActionHandler::GetInstance()->NewAction(new Action(ACTION_SEND_VERSION, Constants::VERSION));
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
  assert(ActiveTeam().GetWeaponType() == WEAPON_SUPERTUX);
  WeaponLauncher* launcher = static_cast<WeaponLauncher*>(&(ActiveTeam().AccessWeapon()));
  SuperTux* tux = static_cast<SuperTux*>(launcher->GetProjectile());

  double x, y;

  tux->angle = a->PopDouble();
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
  assert(mutex!=NULL);
  while (queue.size() != 0)
  {
    SDL_LockMutex(mutex);
    Action *action = queue.front();
    queue.pop_front();
    SDL_UnlockMutex(mutex);
    Exec (action);
    delete action;
  }
}

void ActionHandler::NewAction(Action* a, bool repeat_to_network)
{
  assert(mutex!=NULL);
  SDL_LockMutex(mutex);
  //  MSG_DEBUG("action.handler","New action : %s",a.out());
  //  std::cout << "New action " << a ;
  queue.push_back(a);
  //  std::cout << "  queue_size " << queue.size() << std::endl;
  SDL_UnlockMutex(mutex);
  if (repeat_to_network) network.SendAction(a);
}

void ActionHandler::Register (Action_t action,
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

std::string ActionHandler::GetActionName (Action_t action)
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
  Register (ACTION_MOVE_LEFT, "move_left", &Action_MoveLeft);
  Register (ACTION_MOVE_RIGHT, "move_right", &Action_MoveRight);
  Register (ACTION_UP, "up", &Action_Up);
  Register (ACTION_DOWN, "down", &Action_Down);
  Register (ACTION_JUMP, "jump", &Action_Jump);
  Register (ACTION_HIGH_JUMP, "super_jump", &Action_HighJump);
  Register (ACTION_BACK_JUMP, "back_jump", &Action_BackJump);
  Register (ACTION_SHOOT, "shoot", &Action_Shoot);
  Register (ACTION_CHANGE_WEAPON, "change_weapon", &Action_ChangeWeapon);
  Register (ACTION_WIND, "wind", &Action_Wind);
  Register (ACTION_NEXT_CHARACTER, "next_character", &Action_NextCharacter);
  Register (ACTION_CHANGE_CHARACTER, "change_character", &Action_ChangeCharacter);
  Register (ACTION_SET_GAME_MODE, "set_game_mode", &Action_SetGameMode);
  Register (ACTION_SET_MAP, "set_map", &Action_SetMap);
  Register (ACTION_CLEAR_TEAMS, "clear_teams", &Action_ClearTeams);
  Register (ACTION_NEW_TEAM, "new_team", &Action_NewTeam);
  Register (ACTION_DEL_TEAM, "del_team", &Action_DelTeam);
  Register (ACTION_CHANGE_TEAM, "change_team", &Action_ChangeTeam);
  Register (ACTION_SET_CHARACTER_PHYSICS, "set_character_physics", &Action_SetCharacterPhysics);
  Register (ACTION_SET_MOVEMENT, "set_movement", &Action_SetMovement);
  Register (ACTION_SET_CLOTHE, "set_clothe", &Action_SetClothe);
  Register (ACTION_SET_FRAME, "set_frame", &Action_SetFrame);
  Register (ACTION_SET_CHARACTER_DIRECTION, "set_character_direction", &Action_SetCharacterDirection);
  Register (ACTION_CHANGE_STATE, "change_state", &Action_ChangeState);
  Register (ACTION_ASK_VERSION, "ask_version", &Action_AskVersion);
  Register (ACTION_SEND_VERSION, "send_version", &Action_SendVersion);
  Register (ACTION_SEND_RANDOM, "send_random", &Action_SendRandom);
  Register (ACTION_SYNC_BEGIN, "sync_begin", &Action_SyncBegin);
  Register (ACTION_SYNC_END, "sync_end", &Action_SyncEnd);
  Register (ACTION_EXPLOSION, "explosion", &Action_Explosion);
  Register (ACTION_SET_TARGET, "set_target", &Action_SetTarget);
  Register (ACTION_SUPERTUX_STATE, "supertux_state", &Action_SupertuxState);
  Register (ACTION_SET_TIMEOUT, "set_timeout", &Action_SetTimeout);
  Register (ACTION_CONSTRUCTION_UP, "construction_up", &Action_ConstructionUp);
  Register (ACTION_CONSTRUCTION_DOWN, "construction_down", &Action_ConstructionDown);
  Register (ACTION_WEAPON_STOP_USE, "weapon_stop_use", &Action_WeaponStopUse);
  Register (ACTION_SET_CHARACTER_ENERGY, "set_character_energy", &Action_SetCharacterEnergy);
  Register (ACTION_CHAT_MESSAGE, "chat_message", Action_ChatMessage);
  Register (ACTION_NICKNAME, "nickname", Action_Nickname);
  Register (ACTION_PAUSE, "pause", Action_Pause);
  SDL_UnlockMutex(mutex);
}
