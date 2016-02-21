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
 * Weapon gnu : a gnu jump in (more or less) random directions and explodes
 *****************************************************************************/

#ifndef GNU_H
#define GNU_H

#include "weapon/weapon_launcher.h"

class Gnu;

// The GNU
class GnuLauncher : public WeaponLauncher
{
private:
  Gnu *current_gnu;
  uint gnu_death_time;
public:
  GnuLauncher();

  virtual void SignalEndOfProjectile();
  virtual void SignalProjectileCollision() { };
  virtual void SignalProjectileDrowning() { };

  bool IsOnCooldownFromShot() const;
  bool IsReady() const;

  void StopShooting();

  virtual bool IsPreventingLRMovement();
  virtual bool IsPreventingJumps();
  virtual bool IsPreventingWeaponAngleChanges();

  void UpdateTranslationStrings();
  std::string GetWeaponWinString(const char *TeamName, uint items_count) const;

protected:
  virtual bool p_Shoot();
  virtual void Refresh();
  WeaponProjectile * GetProjectileInstance();
};

#endif
