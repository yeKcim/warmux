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


#ifndef AI_WEAPONS_WEIGHTING_H
#define AI_WEAPONS_WEIGHTING_H

#include "weapon/weapon.h"

class WeaponsWeighting
{
  private:
    double factor[Weapon::LAST+1];
    double min_factor[Weapon::LAST+1];
    double max_factor[Weapon::LAST+1];

  public:
    WeaponsWeighting();
    void RandomizeFactors();
    double GetFactor(Weapon::Weapon_type type);
    void SetMinFactor(Weapon::Weapon_type type, double min_factor);
    void SetMaxFactor(Weapon::Weapon_type type, double max_factor);
};

#endif
