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
 * Define all Warmux actions.
 *****************************************************************************/

#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include <vector>
#include <WARMUX_action_handler.h>
#include <WARMUX_singleton.h>

//-----------------------------------------------------------------------------

// Forward declarations
struct SDL_mutex;
class Player;
class ConfigTeam;
class Team;
class Action;

class ActionHandler : public Singleton<ActionHandler>, public WActionHandler
{
  friend class Singleton<ActionHandler>;
  ActionHandler();
  ~ActionHandler();

public:
  void NewAction(Action* a, bool repeat_to_network=true);
  void NewActionActiveCharacter(const Team* team = NULL, int index=-1); // send infos (on the network) about active character

  void NewRequestTeamAction(const ConfigTeam & team);

  void ExecFrameLessActions();
  bool ExecActionsForOneFrame();
  void AddActionsFrames(uint nb, DistantComputer* cpu);
};

void Action_Handler_Init();

// TODO: Move it in an object !

void SendGameMode();
void SyncCharacters();

void SendInitialGameInfo(DistantComputer* client, int added_player_id);

void WARMUX_ConnectHost(DistantComputer& host);
void WARMUX_DisconnectHost(DistantComputer& host);
void WARMUX_DisconnectPlayer(Player& player);

//-----------------------------------------------------------------------------
#endif
