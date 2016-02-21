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
 * Holly Grenade
 *****************************************************************************/

#ifndef HOLLY_GRENADE_H
#define HOLLY_GRENADE_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../gui/progress_bar.h"
#include "weapon.h"
#include "grenade.h"
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

// The Holly Grenade
class HollyGrenade : public WeaponProjectile
{
protected:
  double temps_debut_tir;
  bool sing_alleluia;

  ParticleEngine smoke_engine;
public:
  HollyGrenade();
  void Tire (double force);
  void Init();
  void Refresh();
  void Draw();
protected:
  void SignalCollision();
};

//-----------------------------------------------------------------------------

class HollyGrenadeLauncher : public Weapon
{
 private:
  void p_Init();
  bool p_Shoot();

public:
  SDL_Surface* impact;    // Image (alpha) de l'impact

  HollyGrenade grenade;


  HollyGrenadeLauncher();
  void Refresh();
  GrenadeConfig& cfg();

protected:
  void Explosion();
};

extern HollyGrenadeLauncher holly_grenade_launcher;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
