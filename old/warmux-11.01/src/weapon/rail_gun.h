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
 * Rail Gun. In addition to Snipe Rifle, change the collision handling.
 *****************************************************************************/

#ifndef RAIL_GUN_H
#define RAIL_GUN_H

#include "particles/particle.h"
#include "weapon/snipe_rifle.h"

class RailGun : public BaseSnipeRifle
{
  uint hits;
protected:
  virtual bool p_Shoot();
  virtual void IncMissedShots();

  // Must be implemented
  virtual WeaponProjectile * GetProjectileInstance();

public:
  RailGun();

  void IncreaseHits() { hits++; }

  // Must be implemented
  std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
  void UpdateTranslationStrings();
};

#endif /* RAIL_GUN_H */
