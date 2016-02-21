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
 * Téléportation : déplacement d'un ver n'importe où sur le terrain.
 *****************************************************************************/

#ifndef TELEPORTATION_H
#define TELEPORTATION_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../tool/Point.h"
#include "weapon.h"
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

class Teleportation : public Weapon
{
private:
  bool retour; // on est dans le retour ?
  uint temps;
  Point2i src, dst;
  int m_x,m_y;
  float m_zoom;
  int m_direction;

  Sprite* skin;

  bool p_Shoot();

public:
  Teleportation();
  void Refresh();
  void Draw();
  virtual void ChooseTarget();
  WeaponConfig& cfg();
};

extern Teleportation teleportation;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
