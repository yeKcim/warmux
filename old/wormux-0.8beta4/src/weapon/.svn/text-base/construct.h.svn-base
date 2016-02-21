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

class Sprite;
class WeaponConfig;

class Construct : public Weapon
{
private:
  bool target_chosen;
  Sprite* construct_spr;
  double angle;
  Point2i dst;

  void Up() const;
  void Down() const;

protected:
  bool p_Shoot();
  void Refresh() { };

public:
  Construct();
  ~Construct();
  void Draw();
  void ChooseTarget(Point2i mouse_pos);

  virtual void HandleKeyPressed_Down(bool) { Down(); };
  virtual void HandleKeyPressed_Up(bool) { Up(); };
  virtual void HandleMouseWheelUp(bool) { Up(); };
  virtual void HandleMouseWheelDown(bool) { Down(); };

  void SetAngle(double _angle) { angle = _angle; }; // to be used by network
  void UpdateTranslationStrings();
  std::string GetWeaponWinString(const char *TeamName, uint items_count) const;
  bool IsInUse() const;
  WeaponConfig& cfg();
};

#endif
