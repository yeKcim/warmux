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
 * Snipe Rifle. Overide the Draw method in order to draw the laser beam.
 *****************************************************************************/

#ifndef SNIPE_RIFLE_H
#define SNIPE_RIFLE_H

#include <vector>
#include "weapon/weapon_launcher.h"
#include "graphic/color.h"
#include "include/base.h"

class SnipeRifle : public WeaponLauncher
{
  private:
    double last_angle;
    Point2i last_rifle_pos;
    Point2i laser_beam_start;
    Point2i targeted_point;
    bool targeting_something;
    Sprite * m_laser_image;
    Color laser_beam_color;
    void ComputeCrossPoint(bool force);

  protected:
    bool p_Shoot();
    void p_Deselect();
    WeaponProjectile * GetProjectileInstance();
  public:
    SnipeRifle();
    void SignalProjectileGhostState();
    void DrawBeam();
    void Draw();  // In order to draw the laser beam / and the contact point.

    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

#endif /* SNIPE_RIFLE_H */
