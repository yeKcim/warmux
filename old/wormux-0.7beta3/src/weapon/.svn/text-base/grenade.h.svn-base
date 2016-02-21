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
 * Weapon grenade : lance une grenade avec un angle et une force donnée. Explose 
 * au bout de quelques secondes
 *****************************************************************************/

#ifndef GRENADE_H
#define GRENADE_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../include/base.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"
#include "weapon.h"

//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

// La Grenade
class Grenade : public WeaponProjectile
{
protected:
  double temps_debut_tir;
public:
  Grenade();
  void Tire (double force);
  uint ChoixFrame(double angle);
  void Init();
  void Refresh();
  void Draw();
protected:
  void SignalCollision();
};

//-----------------------------------------------------------------------------

class GrenadeConfig : public ExplosiveWeaponConfig
{ 
public: 
  uint timeout;
  double rebound_factor;
public:
  GrenadeConfig();
  virtual void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class GrenadeLauncher : public Weapon
{
private:
  void p_Init();
  bool p_Shoot();

public:
  SDL_Surface *impact;
  Grenade grenade;
  int time;

  GrenadeLauncher();
  void Refresh();
  GrenadeConfig& cfg();

protected:
  void Explosion();
};

extern GrenadeLauncher lance_grenade;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
