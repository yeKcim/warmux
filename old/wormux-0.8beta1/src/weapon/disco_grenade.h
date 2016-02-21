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
 * Disco Grenade
 *****************************************************************************/

#ifndef DISCO_GRENADE_H
#define DISCO_GRENADE_H

#include "launcher.h"
#include "graphic/surface.h"
#include "gui/progress_bar.h"
#include "include/base.h"

class DiscoGrenadeLauncher;

// The Disco Grenade
class DiscoGrenade : public WeaponProjectile
{
  protected:
    bool have_played_music;

    ParticleEngine smoke_engine;
  public:
    DiscoGrenade(ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher);
    void Refresh();
    DECLARE_GETWEAPONSTRING();
  protected:
    void Explosion();
    void SignalOutOfMap();
};

class DiscoGrenadeLauncher : public WeaponLauncher
{
  public:
    DiscoGrenadeLauncher();
  protected:
    WeaponProjectile * GetProjectileInstance();
};

#endif /* DISCO_GRENADE_H */
