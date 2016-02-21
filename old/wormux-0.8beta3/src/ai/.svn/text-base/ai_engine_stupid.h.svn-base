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
#ifndef AI_STUPID_ENGINE
#define AI_STUPID_ENGINE

#include "ai/ai_movement_module.h"
#include "ai/ai_shoot_module.h"

class AIStupidEngine
{
 private:
  AIStupidEngine(const AIStupidEngine&);
  const AIStupidEngine& operator=(const AIStupidEngine&);

  static AIStupidEngine * singleton;
  AIStupidEngine();

  AIMovementModule m_movement;
  AIShootModule m_shoot;

  Character* m_last_char;
  Character* m_enemy;

  uint m_begin_turn_time;
  uint m_step;

  uint m_current_time;

  void BeginTurn();

  void ChooseDirection();

 public:
  static AIStupidEngine* GetInstance();

  void Refresh();
  void ForceEndOfTurn();
};

#endif
