/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Interface showing various informations about the current weapon.
 *****************************************************************************/

#ifndef WEAPON_HELP_H
#define WEAPON_HELP_H

#include <WARMUX_base.h>
#include "graphic/surface.h"
#include "graphic/text.h"
#include "tool/affine_transform.h"

class Weapon;
class Polygon;
class Sprite;

class WeaponHelp
{
  Sprite  *weapon_icon;
  Text    help;
  Surface background;
  bool show;
  uint motion_start_time;

public:
  WeaponHelp();
  ~WeaponHelp();
  void SetWeapon(Weapon& item);
  void Draw();
  void SwitchDisplay() { if(show) Hide(); else Show(); }
  AffineTransform2D ComputeWeaponTransformation();
  AffineTransform2D ComputeToolTransformation();
  void Show();
  void Hide(bool play_sound=true);
  void Reset();
  bool IsDisplayed() const { return show; }
};

#endif //WEAPON_MENU_H
