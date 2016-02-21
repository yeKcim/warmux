/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../include/constant.h"
#include "../network/network.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../map/wind.h"
#include "../network/randomsync.h"
#include "../team/macro.h"
#include "../team/move.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"

// Delta appliqué à l'angle du viseur
#define DELTA_CROSSHAIR 2

ActionHandler * ActionHandler::singleton = NULL;

ActionHandler * ActionHandler::GetInstance()
{
  if (singleton == NULL)
    singleton = new ActionHandler();
  return singleton;
}

ActionHandler::ActionHandler()
{
  mutex = NULL;
}

void Action_Walk (const Action *a)
{
  assert(false);
  MoveCharacter (ActiveCharacter());
}

void Action_MoveRight (const Action *a)
{
  assert(false);
  MoveCharacterRight (ActiveCharacter());
}

void Action_MoveLeft (const Action *a)
{
  assert(false);
  MoveCharacterLeft (ActiveCharacter());
}

void Action_Jump (const Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().Jump(); 
}

void Action_HighJump (const Action *a)
{
  GameLoop::GetInstance()->character_already_chosen = true;
  ActiveCharacter().HighJump();
}

void Action_Up (const Action *a)
{
  ActiveTeam().crosshair.ChangeAngle (-DELTA_CROSSHAIR);
}

void Action_Down (const Action *a)
{
  ActiveTeam().crosshair.ChangeAngle (DELTA_CROSSHAIR);
}

void Action_ChangeWeapon (const Action *a)
{
  const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
  ActiveTeam().SetWeapon((Weapon_type)ai.GetValue());
}

void Action_ChangeCharacter (const Action *a)
{
  const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
  ActiveTeam().SelectCharacterIndex (ai.GetValue());
}

void Action_Shoot (const Action *action)
{
  const ActionDoubleInt& a = dynamic_cast<const ActionDoubleInt&> (*action);
  ActiveTeam().AccessWeapon().Shoot(a.GetValue1(), a.GetValue2());
}

void Action_Wind (const Action *a)
{
  const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
  wind.SetVal (ai.GetValue());
}

void Action_MoveCharacter (const Action *a)
{
  const ActionInt2& ap = dynamic_cast<const ActionInt2&> (*a);
//  if (network.is_server())
//  {
//    MSG_DEBUG("action.handler", "%s is %d, %d", ActiveCharacter().m_name, ActiveCharacter().GetX(), ActiveCharacter().GetY());
//    return;
//  }
//  if (!network.is_client()) return;

//  MSG_DEBUG("action.handler", "%s move to  %d, %d", ActiveCharacter().m_name, ap.GetValue1(), ap.GetValue2());

  ActiveCharacter().SetXY (Point2i(ap.GetValue1(), ap.GetValue2()));
}

void Action_SetFrame (const Action *a)
{
  //Set the frame of the walking skin, to get the position of the hand synced
  const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
  if (!ActiveTeam().is_local || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().image->SetCurrentFrame((uint)ai.GetValue());
  }
}

void Action_SetSkin (const Action *a)
{
  const ActionString& action = dynamic_cast<const ActionString&> (*a);
  if (!ActiveTeam().is_local || network.state != Network::NETWORK_PLAYING)
  {
    ActiveTeam().ActiveCharacter().SetSkin(action.GetValue());
  }
}

void Action_SetCharacterDirection (const Action *a)
{
  const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
  ActiveCharacter().SetDirection (ai.GetValue());
}

void Action_SetMap (const Action *a)
{
  const ActionString& action = dynamic_cast<const ActionString&> (*a);
  MSG_DEBUG("action.handler", "SetMap : %s", action.GetValue());
  if (!network.is_client()) return;
  lst_terrain.ChangeTerrainNom (action.GetValue());
  network.state = Network::NETWORK_WAIT_TEAMS;
  world.Reset();
}

void Action_ClearTeams (const Action *a)
{
  MSG_DEBUG("action.handler", "ClearTeams");
  if (!network.is_client()) return;
  teams_list.Clear();
}

void Action_StartGame (const Action *a)
{
  MSG_DEBUG("action.handler", "StartGame");
  network.state = Network::NETWORK_PLAYING;
}

void Action_SetGameMode (const Action *a)
{
  const ActionString& action = dynamic_cast<const ActionString&> (*a);	
  MSG_DEBUG("action.handler", "SetGameMode : %s", action.GetValue());
  GameMode::GetInstance()->Load (action.GetValue());
}


void Action_NewTeam (const Action *a)
{
  const ActionString& action = dynamic_cast<const ActionString&> (*a);
  MSG_DEBUG("action.handler", "NewTeam : %s", action.GetValue());

  if (!network.is_client()) return;
  teams_list.AddTeam (action.GetValue());
  teams_list.SetActive (action.GetValue());
  ActiveTeam().Reset();
}

void Action_ChangeTeam (const Action *a)
{
  const ActionString& action = dynamic_cast<const ActionString&> (*a);
  MSG_DEBUG("action.handler", "ChangeTeam : %s", action.GetValue());
//  if (!network.is_client()) return;
  teams_list.SetActive (std::string(action.GetValue()));
  ActiveTeam().PrepareTurn();
  assert (!ActiveCharacter().IsDead());
}

void Action_AskVersion (const Action *a)
{
  if (!network.is_client()) return;
  if (network.state != Network::NETWORK_WAIT_SERVER) return;
  ActionHandler::GetInstance()->NewAction(ActionString(ACTION_SEND_VERSION, Constants::VERSION));
  network.state = Network::NETWORK_WAIT_MAP;
}

void Action_SendVersion (const Action *a)
{
  if (!network.is_server()) return;
  const ActionString& action = dynamic_cast<const ActionString&> (*a);
  if (action.GetValue() != Constants::VERSION)
  {
    Error(Format(_("Wormux versions are differents : client=%s, server=%s."),
    action.GetValue(), Constants::VERSION.c_str()));
  }
  network.state = Network::NETWORK_SERVER_INIT_GAME;
}

void Action_SendTeam (const Action *a)
{
	// @@@ TODO @@@
}

void Action_AskTeam (const Action *a)
{
  if (!network.is_client()) return;
//	action_handler.NewAction(ActionString(ACTION_SEND_TEAM, ???));
}

void Action_SendRandom (const Action *a)
{
  if (!network.is_client()) return;
  const ActionDouble& action = dynamic_cast<const ActionDouble&> (*a);
  randomSync.AddToTable(action.GetValue());
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

void ActionHandler::NewAction(const Action &a, bool repeat_to_network)
{
  assert(mutex!=NULL);
  SDL_LockMutex(mutex);
  //  MSG_DEBUG("action.handler","New action : %s",a.out());
  //  std::cout << "New action " << a ;
  Action *clone = a.clone();
  queue.push_back(clone);
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

void ActionHandler::Exec(const Action *a)
{
// #ifdef DBG_ACT
  // std::cout << "Exec action " << *a << std::endl;
// #endif

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

void ActionHandler::Init()
{
  mutex = SDL_CreateMutex();
  SDL_LockMutex(mutex);
  Register (ACTION_WALK, "walk", &Action_Walk);
  Register (ACTION_MOVE_LEFT, "move_left", &Action_MoveLeft);
  Register (ACTION_MOVE_RIGHT, "move_right", &Action_MoveRight);
  Register (ACTION_UP, "up", &Action_Up);
  Register (ACTION_DOWN, "down", &Action_Down);
  Register (ACTION_JUMP, "jump", &Action_Jump);
  Register (ACTION_HIGH_JUMP, "super_jump", &Action_HighJump);
  Register (ACTION_SHOOT, "shoot", &Action_Shoot);
  Register (ACTION_CHANGE_WEAPON, "change_weapon", &Action_ChangeWeapon);
  Register (ACTION_WIND, "wind", &Action_Wind);
  Register (ACTION_CHANGE_CHARACTER, "change_character", &Action_ChangeCharacter);
  Register (ACTION_SET_GAME_MODE, "set_game_mode", &Action_SetGameMode);
  Register (ACTION_SET_MAP, "set_map", &Action_SetMap);
  Register (ACTION_CLEAR_TEAMS, "clear_teams", &Action_ClearTeams);
  Register (ACTION_NEW_TEAM, "new_team", &Action_NewTeam);
  Register (ACTION_CHANGE_TEAM, "change_team", &Action_ChangeTeam);
  Register (ACTION_MOVE_CHARACTER, "move_character", &Action_MoveCharacter);
  Register (ACTION_SET_SKIN, "set_skin", &Action_SetSkin);
  Register (ACTION_SET_FRAME, "set_frame", &Action_SetFrame);
  Register (ACTION_SET_CHARACTER_DIRECTION, "set_character_direction", &Action_SetCharacterDirection);
  Register (ACTION_START_GAME, "start_game", &Action_StartGame);
  Register (ACTION_ASK_VERSION, "ask_version", &Action_AskVersion);
  Register (ACTION_ASK_TEAM, "ask_team", &Action_AskTeam);
  Register (ACTION_SEND_VERSION, "send_version", &Action_SendVersion);
  Register (ACTION_SEND_TEAM, "send_team", &Action_SendTeam);
  Register (ACTION_SEND_RANDOM, "send_random", &Action_SendRandom);
  SDL_UnlockMutex(mutex);
}

