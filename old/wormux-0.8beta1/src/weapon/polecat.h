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
 * Polecat : send a polecat to the enemy. Close character get sick with the mefitic odor.
 *****************************************************************************/

#ifndef POLECAT_H
#define POLECAT_H

#include <SDL.h>
#include "grenade.h"
#include "weapon.h"
#include "graphic/sprite.h"
#include "gui/progress_bar.h"
#include "include/base.h"
#include "object/physical_obj.h"

class PolecatFart : public WeaponProjectile
{
  public:
    PolecatFart(ExplosiveWeaponConfig& cfg,
                WeaponLauncher * p_launcher);
};

class Polecat : public WeaponProjectile
{
 private:
  int m_sens;
  int save_x, save_y;
  uint last_fart_time;
  double angle;
 protected:
  void SignalOutOfMap();
 public:
  Polecat(ExplosiveWeaponConfig& cfg,
      WeaponLauncher * p_launcher);
  void Shoot(double strength);
  void Refresh();
  DECLARE_GETWEAPONSTRING();
};

class PolecatLauncher : public WeaponLauncher
{
public:
  PolecatLauncher();
protected:
  WeaponProjectile * GetProjectileInstance();
};

#endif /* POLECAT_H */
