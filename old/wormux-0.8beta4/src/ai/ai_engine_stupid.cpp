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
 * Artificial intelligence stupid engine
 *****************************************************************************/

#include "ai/ai_engine_stupid.h"
#include "character/character.h"
#include "game/game.h"
#include "game/time.h"
#include "team/teams_list.h"

#include <iostream>

AIStupidEngine * AIStupidEngine::singleton = NULL;

AIStupidEngine::AIStupidEngine() :
  m_movement(),
  m_shoot(m_movement),
  m_last_char(NULL),
  m_enemy(NULL),
  m_begin_turn_time(0),
  m_step(0),
  m_current_time(0)
{
  std::cout << "o Artificial Intelligence Stupid engine initialization" << std::endl;
}

AIStupidEngine* AIStupidEngine::GetInstance()
{
  if (singleton == NULL)
    singleton = new AIStupidEngine();

  return singleton;
}

// --------------------------------------------------

void AIStupidEngine::BeginTurn()
{
  m_last_char = &ActiveCharacter();
  m_enemy = NULL;

  m_begin_turn_time = Time::GetInstance()->ReadSec();
  m_step = 0;

  m_movement.BeginTurn();
  m_shoot.BeginTurn();
}

void AIStupidEngine::Refresh()
{
  if (ActiveCharacter().IsDead())
    return;

  // new character to control
  if (&ActiveCharacter() != m_last_char) {
    BeginTurn();
  }

  // Get time
  uint local_time = Time::GetInstance()->ReadSec();
  if (local_time != m_current_time) {
    m_current_time = local_time;
  }

  // wait some seconds as if we are thinking...
  if (m_current_time < m_begin_turn_time + 3)
    return;

  if (m_shoot.Refresh(m_current_time)) {
    m_movement.Move(m_current_time);

    if (!m_movement.IsProgressing())
      m_shoot.SetStrategy(AIShootModule::SKIP_TURN);
  }

//   switch (m_step)
//     {
//     case 0:
//       if (m_nearest_enemy) {
//         // we already knows who to shoot
//         m_step = 1;
//       } else {
//         m_movement.Move(m_current_time);
//       }
//       break;
//     case 1:
//       // shoot !!
//       m_shoot.Shoot(m_current_time);
//       break;
//     case 2:
//       // go go go !!
//       ChooseDirection();
//       m_step++;
//       break;
//     case 3:
//       m_movement.Move(m_current_time);
//       break;
//     case 4:
//       // Wait for end of turn
//       break;
//     default:
//       ASSERT(false);
//     }

}

void AIStupidEngine::ForceEndOfTurn()
{
  m_movement.StopMoving();
  Game::GetInstance()->SetState(Game::HAS_PLAYED);
}
