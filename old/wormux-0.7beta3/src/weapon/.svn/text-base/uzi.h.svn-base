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
 * Uzi !
 *****************************************************************************/

#ifndef UZI_H
#define UZI_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "weapon.h"
#include <vector>
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------


class BalleUzi : public WeaponProjectile
{
public:
  BalleUzi();
  void Init();
  void SignalCollision();
  
};


//-----------------------------------------------------------------------------

class Uzi : public Weapon
{
 private:
  void p_Init();
  void p_Deselect();
  bool p_Shoot();
  void RepeatShoot(); 

protected:
  SDL_Surface *impact;
  BalleUzi balle;
  uint m_first_shoot;

public:
  Uzi();
  void Refresh();
  WeaponConfig& cfg(); 
  void HandleKeyEvent(int action, int event_type);
};

extern Uzi uzi;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
