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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include <WORMUX_action_handler.h>
#include "include/action.h"
#include "include/base.h"

//-----------------------------------------------------------------------------

// Forward declarations
struct SDL_mutex;
class Player;
class TeamConfig;

class ActionHandler : public WActionHandler, public Singleton<ActionHandler>
{
private:
  friend class Singleton<ActionHandler>;
  ActionHandler();
  ~ActionHandler();

public:
  void NewAction(Action* a, bool repeat_to_network=true);
  void NewActionActiveCharacter(Action* a); // send infos (on the network) about active character in the same time

  void NewRequestTeamAction(const ConfigTeam & team);

  void ExecFrameLessActions();
  bool ExecActionsForOneFrame();
};

void Action_Handler_Init();

// TODO: Move it in an object !

void SendGameMode();
void SyncCharacters();

void SendInitialGameInfo(DistantComputer* client, int added_player_id);

void WORMUX_ConnectHost(DistantComputer& host);
void WORMUX_DisconnectHost(DistantComputer& host);
void WORMUX_DisconnectPlayer(Player& player);

//-----------------------------------------------------------------------------
#endif
