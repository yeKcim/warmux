/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Particle Engine
 *****************************************************************************/

#ifndef TELEPORT_MEMBER_H
#define TELEPORT_MEMBER_H
#include "particles/particle.h"

const int teleportation_anim_duration = 1000;

class TeleportMemberParticle : public Particle
{
  uint time;
  Point2i destination;
  Point2i start;
  float sin_x_max;
  float sin_y_max;
 public:
  TeleportMemberParticle(const Sprite& spr, const Point2i& position, const Point2i& dest, int direction);
  ~TeleportMemberParticle();
  void Refresh();
};

#endif /* TELEPORT_MEMBER_H */
