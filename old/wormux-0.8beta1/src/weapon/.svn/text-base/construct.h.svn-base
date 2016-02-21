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
 * Add a structure to the ground
 *****************************************************************************/

#ifndef CONSTRUCT_H
#define CONSTRUCT_H
#include "weapon.h"
#include "include/base.h"
#include "tool/point.h"
#include "graphic/sprite.h"

class Construct : public Weapon
{
private:
  bool target_chosen;
  Sprite* construct_spr;
  double angle;
  Point2i dst;

  void Up();
  void Down();

protected:
  bool p_Shoot();
  void Refresh();

public:
  Construct();
  ~Construct();
  void Draw();
  void ChooseTarget(Point2i mouse_pos);

  virtual void HandleKeyPressed_Down();
  virtual void HandleKeyPressed_Up();
  virtual void HandleMouseWheelUp();
  virtual void HandleMouseWheelDown();

  void SetAngle(double angle); // to be used by network
  DECLARE_GETWEAPONSTRING();

  WeaponConfig& cfg();
};

#endif
