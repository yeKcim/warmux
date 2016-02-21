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

#include "particles/teleport_member.h"
#include "particles/particle.h"
#include "tool/random.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "map/camera.h"

TeleportMemberParticle::TeleportMemberParticle(const Sprite* spr, const Point2i& position, const Point2i& dest, int direction) :
  Particle("teleport_member_particle")
{
  SetCollisionModel(true, false, false);
  image = new Sprite(spr->GetSurface());

  float scale_x, scale_y;
  image->GetScaleFactors(scale_x, scale_y);
  image->Scale(scale_x * (float)direction, scale_y);

  ASSERT(image->GetWidth() != 0 && image->GetHeight()!=0);
  SetXY(position);
  m_left_time_to_live = 1;

  SetSize(image->GetSize());
  SetOnTop(true);
  destination = dest;
  start = position;
  time = Time::GetInstance()->Read();

  sin_x_max = RandomLocal().GetDouble(M_PI_4, 3.0 * M_PI_4);
  sin_y_max = RandomLocal().GetDouble(M_PI_4, 3.0 * M_PI_4);
  Camera::GetInstance()->FollowObject(this, true);
}

TeleportMemberParticle::~TeleportMemberParticle()
{
  Camera::GetInstance()->StopFollowingObj(this);
}

void TeleportMemberParticle::Refresh()
{
  uint now = Time::GetInstance()->Read();
  if(now > time + teleportation_anim_duration)
    m_left_time_to_live = 0;

  uint dt = now - time;
  Point2i dpos;
  dpos.x = (int)((destination.x - start.x) * sin((float)dt * sin_x_max / (float)teleportation_anim_duration) / sin(sin_x_max))/* * dt / teleportation_anim_duration*/;
  dpos.y = (int)((destination.y - start.y) * sin((float)dt * sin_y_max / (float)teleportation_anim_duration) / sin(sin_y_max))/* * dt / teleportation_anim_duration*/;

  SetXY( start + dpos );
  image->Update();
}
