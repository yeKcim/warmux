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
 * Artificial intelligence Shoot module
 *****************************************************************************/

#ifndef AI_SHOOT_MODULE_H
#define AI_SHOOT_MODULE_H

#include "character/character.h"
#include "ai_movement_module.h"

class AIShootModule
{
 private:

  // ===== TODO : move in AI-stupid-engine
  typedef enum {
    NO_STRATEGY,
    NEAR_FROM_ENEMY,
    SHOOT_FROM_POINT
  } strategy_t;

  strategy_t m_current_strategy;
  
  uint m_current_time;

  const Character* m_enemy;
  bool m_has_finished;

  const AIMovementModule& m_AIMovementModule;
  // =======================

  double m_angle;
  uint m_last_shoot_time;

  // for shooting weapons like gun, shotgun, sniper rifle, m16, ...
  static bool IsDirectlyShootable(const Character& shooter, 
				  const Character& enemy,
				  double& shoot_angle);

  static const Character* FindShootableEnemy(Character& shooter,
					     double& shoot_angle);

  bool SelectFiringWeapon(double shoot_angle) const;

  // for proximity weapons like dynamite, mine, ...
  // TODO -> Go in ai_movment_module
  const Character* FindProximityEnemy(const Character& shooter) const;

  bool SelectProximityWeapon(const Character& enemy) const;

  // Watch the choosen enemy
  void ChooseDirection();
  const Character* FindEnemy();

  void Shoot();
 public:
  AIShootModule(const AIMovementModule& to_remove);
  void BeginTurn();

  bool Refresh(uint current_time);
  void SetNoStrategy();
};

#endif
