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
 * An AI player for a team.
 *****************************************************************************/

#ifndef AI_STUPID_PLAYER_H
#define AI_STUPID_PLAYER_H

#include "ai/ai_player.h"
#include "ai/ai_command.h"
#include "ai/ai_strategy.h"
#include "ai/ai_idea.h"
#include "ai/ai_weapons_weighting.h"
#include "team/team.h"

class AIStupidPlayer : public AIPlayer
{
  private:
    Team * team;
    std::vector<AIIdea*> ideas;
    std::vector<AIIdea*>::iterator idea_iterator;
    AICommand * command;
    AIStrategy * best_strategy;
    bool command_executed;
    uint game_time_at_turn_start;
    int best_strategy_counter;
    WeaponsWeighting weapons_weighting;

    void Reset();
    void CheckNextIdea();
  public:
    AIStupidPlayer(Team * team);
    virtual ~AIStupidPlayer();
    virtual void PrepareTurn();
    virtual void Refresh();
};

#endif
