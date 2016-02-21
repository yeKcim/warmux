/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Weapon bazooka : projette une roquette avec un angle et une force donnée.
 *****************************************************************************/

#ifndef GNU_H
#define GNU_H

#include <SDL.h>
#include "grenade.h"
#include "weapon.h"
#include "../graphic/sprite.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../object/physical_obj.h"

class GnuLauncher;

// The GNU
class Gnu : public WeaponProjectile
{
 private:
  int m_sens;
  int save_x, save_y;
  double angle;
public:
  Gnu(ExplosiveWeaponConfig& cfg);
  void Shoot(double strength);
  void Refresh();
  void SignalCollision();
};

class GnuLauncher : public WeaponLauncher
{
public:
  GnuLauncher();
};

#endif
