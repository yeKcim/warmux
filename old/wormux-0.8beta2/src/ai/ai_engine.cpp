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
 * Artificial intelligence engine
 *****************************************************************************/

#include "ai_engine.h"
#include <string>
#include <iostream>
#include "ai_engine_stupid.h"
#include "character/character.h"
#include "game/game_loop.h"
#include "team/team.h"
#include "team/teams_list.h"


AIengine * AIengine::singleton = NULL;

AIengine::AIengine()
{
  std::cout << "o Artificial Intelligence engine initialization" << std::endl;
}

AIengine* AIengine::GetInstance()
{
  if (singleton == NULL)
    singleton = new AIengine();

  return singleton;
}

void AIengine::Refresh() const
{
  // AI does not have right to play
  if (GameLoop::GetInstance()->ReadState() == GameLoop::END_TURN)
    return;

  if (ActiveCharacter().GetTeam().IsLocalAI()) {
    AIStupidEngine::GetInstance()->Refresh();
  }

}
