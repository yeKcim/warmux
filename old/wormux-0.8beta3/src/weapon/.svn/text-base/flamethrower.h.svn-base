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
 * Submachine gun. Don't fire bullet one by one but with burst fire (like
 * a submachine gun :)
 *****************************************************************************/

#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H

#include "particles/particle.h"
#include "weapon_launcher.h"

class FlameThrower : public WeaponLauncher
{
    ParticleEngine particle;
    void RepeatShoot();
  protected:
    WeaponProjectile * GetProjectileInstance();
    void IncMissedShots();
    bool p_Shoot();
  public:
    FlameThrower();
    virtual void HandleKeyPressed_Shoot(bool shift) { HandleKeyRefreshed_Shoot(shift); };
    virtual void HandleKeyRefreshed_Shoot(bool shift);
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

#endif /* SUBMACHINE_GUN_H */
