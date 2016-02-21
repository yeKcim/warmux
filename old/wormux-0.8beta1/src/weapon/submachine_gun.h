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

#ifndef SUBMACHINE_GUN_H
#define SUBMACHINE_GUN_H

#include <SDL.h>
#include <vector>
#include "launcher.h"
#include "include/base.h"
#include "tool/point.h"

class SubMachineGunBullet : public WeaponBullet
{
  public:
    SubMachineGunBullet(ExplosiveWeaponConfig& cfg,
                        WeaponLauncher * p_launcher);
  protected:
    void ShootSound();
    void RandomizeShoot(double &angle,double &strength);
};

class SubMachineGun : public WeaponLauncher
{
    ParticleEngine particle;
  private:
    void RepeatShoot();
  protected:
    WeaponProjectile * GetProjectileInstance();
    void IncMissedShots();
    bool p_Shoot();
    void p_Deselect();
  public:
    SubMachineGun();
    virtual void SignalTurnEnd();
    virtual void HandleKeyPressed_Shoot();
    virtual void HandleKeyRefreshed_Shoot();
    virtual void HandleKeyReleased_Shoot();
    DECLARE_GETWEAPONSTRING();
};

#endif /* SUBMACHINE_GUN_H */
