/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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

#include "particles/particle.h"
#include "weapon/weapon_launcher.h"

class SubMachineGun : public WeaponLauncher
{
private:
  ParticleEngine particle;
  bool shoot_started;
protected:
  WeaponProjectile * GetProjectileInstance();
  void IncMissedShots();
  virtual void Refresh();
  virtual bool p_Shoot();
  virtual void p_Deselect();
public:
  SubMachineGun();

  virtual bool IsInUse() const;

  virtual void UpdateTranslationStrings();
  virtual std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

#endif /* SUBMACHINE_GUN_H */
