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
 * Damage statistics for a character
 *****************************************************************************/

#ifndef _DAMAGE_STATS_H
#define _DAMAGE_STATS_H

#include "include/base.h"

class Character;

class DamageStatistics
{
  const Character& owner;

  uint damage_other_teams;
  uint damage_friendly_fire; // damage same team but not itself
  uint damage_itself;
  uint max_damage;
  uint current_total_damage;
  uint total_shots;

  uint death_time; // if 0, not dead

public:
  DamageStatistics(const Character& _owner);
  DamageStatistics(const DamageStatistics& adamage_stats,
                   const Character& _owner);

  void OneMoreShot() { total_shots++; }
  void MadeDamage(const int Dmg, const Character &other);
  void HandleMostDamage();
  void ResetDamage();

  uint GetMostDamage() const { return max_damage; }
  uint GetFriendlyFireDamage() const { return damage_friendly_fire; }
  uint GetItselfDamage() const { return damage_itself; }
  uint GetOthersDamage() const { return damage_other_teams; }
  double GetAccuracy() const { return (total_shots) ? damage_other_teams/(double)total_shots : 0.0; }

  void SetDeathTime(uint _death_time) { death_time = _death_time; };
  uint GetDeathTime() const { return death_time; };
};

#endif
