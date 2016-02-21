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
    static bool CanUseWeapon(Weapon * weapon);
    static bool CanUseCharacter(Character & character);
    static LRDirection XDeltaToDirection(Double delta);
    static Double GetDirectionRelativeAngle(LRDirection direction, Double angle);
    static Double RateDamageDoneToEnemy(int damage, Character & enemy);
    static Double RateDamageDoneToEnemy(int min_damage, int max_damage, Character & enemy);
    static Double RateExplosion(Character & shooter, Point2i position, ExplosiveWeaponConfig & cfg, Double expected_additional_distance);
  public:
    virtual AIStrategy * CreateStrategy() = 0;
    virtual ~AIIdea() {}
};

class SkipTurnIdea : public AIIdea
{
  public:
    virtual AIStrategy * CreateStrategy();
};

class WasteAmmoUnitsIdea : public AIIdea
{
  public:
    virtual AIStrategy * CreateStrategy();
};

class ShootDirectlyAtEnemyIdea : public AIIdea
{
  private:
    WeaponsWeighting & weapons_weighting;
    Character & shooter;
    Character & enemy;
    Weapon::Weapon_type weapon_type;
    Double max_distance;
  public:
    ShootDirectlyAtEnemyIdea(WeaponsWeighting & weapons_weighting, Character & shooter, Character & enemy, Weapon::Weapon_type weapon_type, Double max_distance);
    virtual AIStrategy * CreateStrategy();
};

class FireMissileWithFixedDurationIdea : public AIIdea
{
  private:
    WeaponsWeighting & weapons_weighting;
    Character & shooter;
    Character & enemy;
    Weapon::Weapon_type weapon_type;
    Double duration;
    int timeout; // if positive the character will set it to the specified value.
  public:
    FireMissileWithFixedDurationIdea(WeaponsWeighting & weapons_weighting, Character & shooter, Character & enemy, Weapon::Weapon_type weapon_type, Double duration, int timeout = -1);
    virtual AIStrategy * CreateStrategy();
};

#endif
