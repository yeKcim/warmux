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
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"
#include "weapon.h"
#include "grenade.h"

//-----------------------------------------------------------------------------
using namespace Wormux;
// The GNU
class Gnu : public PhysicalObj
{
  uint launched_time;
  Sprite *image;
  int m_sens;
  double save_x,save_y;
public:
  bool is_active;

  Gnu();
  void Init();
  void Draw();
  void Reset();
  void Tire(double force);
  void Refresh();
  void SignalFallEnding();
  void SignalCollision();
  void SignalGhostState();
};

//-----------------------------------------------------------------------------

class GnuLauncher : public Weapon
{
public:
  SDL_Surface *impact;
  Gnu gnu;

public:
  GnuLauncher();
  bool p_Shoot ();
  void Refresh();
  void p_Init();
  GrenadeConfig& cfg();

  void Explosion();

};

extern GnuLauncher gnu_launcher;
//-----------------------------------------------------------------------------
#endif
