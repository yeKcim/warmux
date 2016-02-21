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
 * Weapon grenade : lance une grenade avec un angle et une force donn�. Explose 
 * au bout de quelques secondes
 *****************************************************************************/

#ifndef GRENADE_H
#define GRENADE_H

#include <SDL.h>
#include "include/base.h"
#include "graphic/surface.h"
#include "gui/progress_bar.h"
#include "object/physical_obj.h"
#include "launcher.h"

// La Grenade
class Grenade : public WeaponProjectile
{
public:
  Grenade(ExplosiveWeaponConfig& cfg,
          WeaponLauncher * p_launcher);
  void Refresh();
  DECLARE_GETWEAPONSTRING();
protected:
  void SignalOutOfMap();
};

class GrenadeLauncher : public WeaponLauncher
{
 public:
  GrenadeLauncher();
 protected:
  WeaponProjectile * GetProjectileInstance();
};

#endif
