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
 * Base classes for weapons configuration.
 *****************************************************************************/

#include <iostream>
#include "weapon/weapon_cfg.h"

//-----------------------------------------------------------------------------

WeaponConfig::WeaponConfig()
{
  push_back(new UintConfigElement("damage", &damage, 10, 0, 2000));
}

//-----------------------------------------------------------------------------

ExplosiveWeaponConfig::ExplosiveWeaponConfig()
{
  push_back(new UintConfigElement("timeout", &timeout, 0));
  push_back(new BoolConfigElement("allow_change_timeout", &allow_change_timeout, false));
  push_back(new DoubleConfigElement("explosion_range", &explosion_range, 0, 0, 200));
  push_back(new DoubleConfigElement("particle_range", &particle_range, 0));
  push_back(new DoubleConfigElement("blast_range", &blast_range, 0));
  push_back(new DoubleConfigElement("blast_force", &blast_force, 0));
  push_back(new DoubleConfigElement("speed_on_hit", &speed_on_hit, 0));
}
