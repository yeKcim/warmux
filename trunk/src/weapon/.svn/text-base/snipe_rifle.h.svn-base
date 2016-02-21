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
 * Snipe Rifle. Overide the Draw method in order to draw the laser beam.
 *****************************************************************************/

#ifndef SNIPE_RIFLE_H
#define SNIPE_RIFLE_H

#include <vector>
#include "weapon/weapon_launcher.h"
#include "graphic/color.h"
#include <WARMUX_base.h>

class BaseSnipeRifle : public WeaponLauncher
{
private:
  float last_angle;
  bool targeting_something;
  Point2i last_rifle_pos;
  Point2i laser_beam_start;
  Point2i targeted_point;

  Sprite * m_laser_image;
  Color laser_beam_color, laser_beam_end_color;

  void ComputeCrossPoint(bool force);

protected:
  virtual bool p_Shoot();
  void p_Deselect();

public:
  BaseSnipeRifle(Weapon_type type,
                 const std::string &id);
  ~BaseSnipeRifle();
  void SignalProjectileGhostState();
  void DrawBeam();
  void Draw();  // In order to draw the laser beam / and the contact point.
};

class SnipeRifle : public BaseSnipeRifle
{
protected:
  virtual WeaponProjectile * GetProjectileInstance();

public:
  SnipeRifle();

  std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
  void UpdateTranslationStrings();
};

#endif /* SNIPE_RIFLE_H */
