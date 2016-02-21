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
 * It's boring if an AI useses always the same weapon.
 * This class makes it possible to rate strategies based on what weapon gets used.
 * The RandomizeFactors gets called every turn so that the AI preferes different weapons each turn.
 *****************************************************************************/

#include <WORMUX_random.h>
#include "ai/ai_weapons_weighting.h"

WeaponsWeighting::WeaponsWeighting()
{
  for (int index = Weapon::FIRST; index < Weapon::LAST; index++) {
    Weapon::Weapon_type type = (Weapon::Weapon_type) index;
    SetMinFactor(type, 1);
    SetMaxFactor(type, 1);
    factor[type] = 1;
  }
  // AI is much to good with bazooka...
  SetMinFactor(Weapon::WEAPON_BAZOOKA, 0.2);
  SetMaxFactor(Weapon::WEAPON_BAZOOKA, 0.8);
}

void WeaponsWeighting::RandomizeFactors()
{
  for (int index = Weapon::FIRST; index < Weapon::LAST; index++) {
    Weapon::Weapon_type type = (Weapon::Weapon_type) index;
    factor[type] = RandomLocal().GetDouble(min_factor[type], max_factor[type]);
  }
}

double WeaponsWeighting::GetFactor(Weapon::Weapon_type type)
{
  return factor[type];
}

void WeaponsWeighting::SetMinFactor(Weapon::Weapon_type type, double value)
{
  min_factor[type] = value;
}

void WeaponsWeighting::SetMaxFactor(Weapon::Weapon_type type, double value)
{
  max_factor[type] = value;
}
