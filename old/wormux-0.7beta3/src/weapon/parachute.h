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
 * Jet Pack :-)
 *****************************************************************************/

#ifndef PARACHUTE_H
#define PARACHUTE_H
//-----------------------------------------------------------------------------
#include "weapon.h"
//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

class Parachute : public Weapon
{
private:
  bool open ;
  bool closing ;
  double air_resist_factor ;
  double open_speed_limit ;
  Sprite* image;

public:
  Parachute();
  void p_Init();
  void p_Select();
  void p_Deselect();
  void Refresh();
  void Draw() ;
  bool p_Shoot();
  void SignalTurnEnd();
  
  void Activate();
  void Desactivate();
};

 extern Parachute parachute;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
