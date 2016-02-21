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
//-----------------------------------------------------------------------------
#include "../tool/i18n.h"
//-----------------------------------------------------------------------------

#ifdef DEBUG

// Love debug message ?
//#define DBG_ACT

#define COUT_DBG std::cout << "[Action Handler] "
#endif

//-----------------------------------------------------------------------------
#ifdef CL
# include "../network/network.h"
#endif

#include "../include/constant.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../map/map.h"
#include "../map/wind.h"
#include "../map/maps_list.h"
#include "../team/macro.h"
#include "../team/move.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"
//-----------------------------------------------------------------------------
ActionHandler action_handler;
//-----------------------------------------------------------------------------

// Delta appliqué à l'angle du viseur
#define DELTA_CROSSHAIR 2

void Action_Walk (const Action *a)
{
	 MoveCharacter (ActiveCharacter());
}

//-----------------------------------------------------------------------------
void Action_MoveRight (const Action *a)
{
	assert(false);
	MoveCharacterRight (ActiveCharacter());
}

//-----------------------------------------------------------------------------
void Action_MoveLeft (const Action *a)
{
	assert(false);
	MoveCharacterLeft (ActiveCharacter());
}

//-----------------------------------------------------------------------------

void Action_Jump (const Action *a)
{
      game_loop.character_already_chosen = true;
      ActiveCharacter().Saute(); 
}

//-----------------------------------------------------------------------------

void Action_SuperJump (const Action *a)
{
      game_loop.character_already_chosen = true;
      ActiveCharacter().SuperSaut();
}

//-----------------------------------------------------------------------------

void Action_Up (const Action *a)
{
	ActiveTeam().crosshair.ChangeAngle (-DELTA_CROSSHAIR);
}

//-----------------------------------------------------------------------------

void Action_Down (const Action *a)
{
	ActiveTeam().crosshair.ChangeAngle (DELTA_CROSSHAIR);
}

//-----------------------------------------------------------------------------

void Action_ChangeWeapon (const Action *a)
{
	const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
	ActiveTeam().SetWeapon((Weapon_type)ai.GetValue());
}

//-----------------------------------------------------------------------------

void Action_ChangeCharacter (const Action *a)
{
	const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
	ActiveTeam().SelectCharacterIndex (ai.GetValue());
}

//-----------------------------------------------------------------------------

void Action_Shoot (const Action *action)
{
	const ActionDoubleInt& a = dynamic_cast<const ActionDoubleInt&> (*action);
	ActiveTeam().AccessWeapon().Shoot(a.GetValue1(), a.GetValue2());
}

//-----------------------------------------------------------------------------

void Action_Wind (const Action *a)
{
	const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
	Wormux::wind.SetVal (ai.GetValue());
}

//-----------------------------------------------------------------------------

void Action_MoveCharacter (const Action *a)
{
#ifdef CL
	const ActionInt2& ap = dynamic_cast<const ActionInt2&> (*a);
        if (network.is_server())
	{
#ifdef DBG_ACT
	COUT_DBG << ActiveCharacter().m_name << " is " << ActiveCharacter().GetX() << ", " << ActiveCharacter().GetY() << std::endl;
#endif
		return;
	}
	if (!network.is_client()) return;
#ifdef DBG_ACT
	COUT_DBG << ActiveCharacter().m_name << " move to " << ap.GetValue1() << ", " << ap.GetValue2() << std::endl;
#endif
	ActiveCharacter().SetXY (ap.GetValue1(), ap.GetValue2());
#endif // CL defined
}
//-----------------------------------------------------------------------------

void Action_SetCharacterDirection (const Action *a)
{
	const ActionInt& ai = dynamic_cast<const ActionInt&> (*a);
	ActiveCharacter().SetDirection (ai.GetValue());
}

//-----------------------------------------------------------------------------

void Action_SetMap (const Action *a)
{
#ifdef CL
	const ActionString& action = dynamic_cast<const ActionString&> (*a);
#ifdef DBG_ACT
	COUT_DBG << "SetMap : " << action.GetValue() << std::endl;
#endif
        if (!network.is_client()) return;
        lst_terrain.ChangeTerrainNom (action.GetValue());
	network.state = Network::NETWORK_WAIT_TEAMS;
	monde.Reset();
#endif
}

//-----------------------------------------------------------------------------

void Action_ClearTeams (const Action *a)
{
#ifdef DBG_ACT
	COUT_DBG << "ClearTeams" << std::endl;
#endif
#ifdef CL
        if (!network.is_client()) return;
	teams_list.Clear();
#endif
}

//-----------------------------------------------------------------------------

void Action_StartGame (const Action *a)
{
#ifdef DBG_ACT
	COUT_DBG << "StartGame" << std::endl;
#endif
#ifdef CL
        if (!network.is_client()) return;
	network.state = Network::NETWORK_PLAYING;
#endif
}

//-----------------------------------------------------------------------------

void Action_SetGameMode (const Action *a)
{
	const ActionString& action = dynamic_cast<const ActionString&> (*a);
#ifdef DBG_ACT
	COUT_DBG << "SetGameMode : " << action.GetValue() << std::endl;
#endif
	Wormux::game_mode.Load (action.GetValue());
}


//-----------------------------------------------------------------------------

void Action_NewTeam (const Action *a)
{
#ifdef CL
	const ActionString& action = dynamic_cast<const ActionString&> (*a);
#ifdef DBG_ACT
	COUT_DBG << "NewTeam : " << action.GetValue() << std::endl;
#endif
   
        if (!network.is_client()) return;
	teams_list.AddTeam (action.GetValue());
	teams_list.SetActive (action.GetValue());
	ActiveTeam().Reset();
#endif
}

//-----------------------------------------------------------------------------

void Action_ChangeTeam (const Action *a)
{
	const ActionString& action = dynamic_cast<const ActionString&> (*a);
#ifdef DBG_ACT
	COUT_DBG << "ChangeTeam : " << action.GetValue() << std::endl;
#endif
	//if (!network.is_client()) return;
	teams_list.SetActive (action.GetValue());
 	ActiveTeam().PrepareTour();
	assert (!ActiveCharacter().IsDead());
}

//-----------------------------------------------------------------------------

void Action_AskVersion (const Action *a)
{
#ifdef CL
        if (!network.is_client()) return;
	if (network.state != Network::NETWORK_WAIT_SERVER) return;
	action_handler.NewAction(ActionString(ACTION_SEND_VERSION, VERSION));
	network.state = Network::NETWORK_WAIT_MAP;
#endif
}

//-----------------------------------------------------------------------------

void Action_SendVersion (const Action *a)
{
#ifdef CL
        if (!network.is_server()) return;
	const ActionString& action = dynamic_cast<const ActionString&> (*a);
	if (action.GetValue() != VERSION)
	{
		Erreur(Format(_("Wormux versions are differents : client=%s, server=%s."),
			action.GetValue().c_str(), VERSION));
	}
	network.state = Network::NETWORK_SERVER_INIT_GAME;
#endif
}

//-----------------------------------------------------------------------------

void Action_SendTeam (const Action *a)
{
	// @@@ TODO @@@
}

//-----------------------------------------------------------------------------

void Action_AskTeam (const Action *a)
{
#ifdef CL
        if (!network.is_client()) return;
//	action_handler.NewAction(ActionString(ACTION_SEND_TEAM, ???));
#endif
}

//-----------------------------------------------------------------------------

void ActionHandler::ExecActions()
{
    while (queue.size() != 0)
	{
		Action *action = queue.front();
        queue.pop_front();
		Exec (action);
		delete action;
	}
}

//-----------------------------------------------------------------------------

void ActionHandler::NewAction(const Action &a, bool repeat_to_network)
{
#ifdef DBG_ACT
	COUT_DBG << "New action " << a << std::endl;
#endif
	Action *clone = a.clone();
	queue.push_back(clone);
#ifdef CL
        if (repeat_to_network) network.send_action(a);
#endif
}

//-----------------------------------------------------------------------------

void ActionHandler::Register (Action_t action, 
		                      const std::string &name,callback_t fct)
{
	handler[action] = fct;
	action_name[action] = name;
}

//-----------------------------------------------------------------------------

void ActionHandler::Exec(const Action *a)
{
#ifdef DBG_ACT
	COUT_DBG << "Exec action " << *a << std::endl;
#endif

        handler_it it=handler.find(a->GetType());
	assert(it != handler.end());
	(*it->second) (a);
}

//-----------------------------------------------------------------------------

std::string ActionHandler::GetActionName (Action_t action)
{
	name_it it=action_name.find(action);
	assert(it != action_name.end());
	return it->second;
}

//-----------------------------------------------------------------------------

void ActionHandler::Init()
{
	Register (ACTION_WALK, "walk", &Action_Walk);
	Register (ACTION_MOVE_LEFT, "move_left", &Action_MoveLeft);
	Register (ACTION_MOVE_RIGHT, "move_right", &Action_MoveRight);
	Register (ACTION_UP, "up", &Action_Up);
	Register (ACTION_DOWN, "down", &Action_Down);
	Register (ACTION_JUMP, "jump", &Action_Jump);
	Register (ACTION_SUPER_JUMP, "super_jump", &Action_SuperJump);
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
	Register (ACTION_SET_CHARACTER_DIRECTION, "set_character_direction", &Action_SetCharacterDirection);
	Register (ACTION_START_GAME, "start_game", &Action_StartGame);
	Register (ACTION_ASK_VERSION, "ask_version", &Action_AskVersion);
	Register (ACTION_ASK_TEAM, "ask_team", &Action_AskTeam);
	Register (ACTION_SEND_VERSION, "send_version", &Action_SendVersion);
	Register (ACTION_SEND_TEAM, "send_team", &Action_SendTeam);
}

//-----------------------------------------------------------------------------

