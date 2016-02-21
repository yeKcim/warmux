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
 * Weapon Supertux : and now the flying magic pinguin !
 *****************************************************************************/

#ifndef SUPERTUX_H
#define SUPERTUX_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"
#include "weapon.h"
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

class SuperTuxWeaponConfig : public ExplosiveWeaponConfig
{
public:
  uint speed;
  SuperTuxWeaponConfig();
  virtual void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class SuperTux : public WeaponProjectile
{
 private:
  ParticleEngine particle_engine;

public:
  double angle;
  uint time_now;
  uint time_next_action;
  uint last_move;

  SuperTux();
  void Init();
  void Refresh();
  void Draw();

  void turn_left();
  void turn_right();
  void Tire ();

protected:
  void SignalCollision();
};

//-----------------------------------------------------------------------------

class TuxLauncher : public Weapon
{
 private:
  void p_Init();
  bool p_Shoot();

public:
  SDL_Surface* impact;
  SuperTux supertux;

public:
  TuxLauncher();
  void Refresh();
  void HandleKeyEvent(int action, int event_type);
   
  SuperTuxWeaponConfig& cfg();

protected:
  void Explosion();
};

extern TuxLauncher tux;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
