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
 * A factory for AI strategies. It contains no turn specfic data.
 *****************************************************************************/

#ifndef AI_IDEA_H
#define AI_IDEA_H

#include "ai/ai_strategy.h"
#include "ai/ai_weapons_weighting.h"
#include "weapon/weapon.h"
#include "weapon/weapon_launcher.h"

class AIIdea
{
protected:
  static bool CanUseWeapon(const Weapon * weapon);
  static bool CanUseCharacter(const Character & character);
  static LRDirection XDeltaToDirection(int delta) { return (delta < 0) ? DIRECTION_LEFT : DIRECTION_RIGHT; }
  static LRDirection XDeltaToDirection(float delta) { return (delta < 0) ? DIRECTION_LEFT : DIRECTION_RIGHT; }
  static float GetDirectionRelativeAngle(LRDirection direction, float angle);
  static float RateDamageDoneToEnemy(int damage, const Character & enemy);
  static float RateDamageDoneToEnemy(int min_damage, int max_damage, const Character & enemy);
  static float RateExplosion(const Character & shooter, const Point2i& position,
                             const ExplosiveWeaponConfig & cfg,
                             const float& expected_additional_distance);
public:
  virtual AIStrategy * CreateStrategy(float accuracy) const = 0;
  virtual ~AIIdea() {}
  virtual bool NoLongerPossible() const { return false; }
  virtual float GetMaxRating(bool) const { return 0.0f; }
};

class SkipTurnIdea : public AIIdea
{
public:
  virtual AIStrategy * CreateStrategy(float accuracy) const;
};

class WasteAmmoUnitsIdea : public AIIdea
{
public:
  virtual AIStrategy * CreateStrategy(float accuracy) const;
};

class AIShootIdea : public AIIdea
{
protected:
  const WeaponsWeighting & weapons_weighting;
  const Character & shooter;
  const Character & enemy;
  Weapon::Weapon_type weapon_type;

  AIShootIdea(const WeaponsWeighting & w,
              const Character & s, const Character & e,
              Weapon::Weapon_type t)
    : weapons_weighting(w), shooter(s), enemy(e), weapon_type(t) { }
public:
  virtual bool NoLongerPossible() const;
  virtual float GetMaxRating(bool one_shot) const;
};

class ShootDirectlyAtEnemyIdea : public AIShootIdea
{
  const int max_sq_distance;
public:
  ShootDirectlyAtEnemyIdea(const WeaponsWeighting & weapons_weighting,
                           const Character & shooter, const Character & enemy,
                           Weapon::Weapon_type weapon_type, int max_distance);
  virtual AIStrategy * CreateStrategy(float accuracy) const;
};

class FireMissileWithFixedDurationIdea : public AIShootIdea
{
  const float duration;
  const int   timeout; // if positive the character will set it to the specified value.
  const float confidence;
public:
  FireMissileWithFixedDurationIdea(const WeaponsWeighting & weapons_weighting,
                                   const Character & shooter, const Character & enemy,
                                   Weapon::Weapon_type weapon_type,
                                   float duration, int timeout = -1);
  virtual AIStrategy * CreateStrategy(float accuracy) const;
};

#endif
