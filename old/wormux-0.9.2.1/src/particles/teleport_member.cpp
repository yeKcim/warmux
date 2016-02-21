/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include "network/randomsync.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "map/camera.h"

TeleportMemberParticle::TeleportMemberParticle(const Sprite& spr, const Point2i& position, const Point2i& dest, int direction) :
  Particle("teleport_member_particle")
{
  SetCollisionModel(false, false, false);
  image = new Sprite(spr.GetSurface());

  Double scale_x, scale_y;
  image->GetScaleFactors(scale_x, scale_y);
  image->Scale(scale_x * (Double)direction, scale_y);

  ASSERT(image->GetWidth() != 0 && image->GetHeight()!=0);
  SetXY(position);
  m_left_time_to_live = 1;

  SetSize(image->GetSize());
  SetOnTop(true);
  destination = dest;
  start = position;
  time = Time::GetInstance()->Read();

  MSG_DEBUG("random.get", "TeleportMemberParticle::TeleportMemberParticle(...)");
  sin_x_max = RandomSync().GetDouble(QUARTER_PI, THREE * QUARTER_PI);
  MSG_DEBUG("random.get", "TeleportMemberParticle::TeleportMemberParticle(...)");
  sin_y_max = RandomSync().GetDouble(QUARTER_PI, THREE * QUARTER_PI);

  Camera::GetInstance()->FollowObject(this);
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
  dpos.x = (int)((destination.x - start.x) * sin((Double)dt * sin_x_max / (Double)teleportation_anim_duration) / sin(sin_x_max))/* * dt / teleportation_anim_duration*/;
  dpos.y = (int)((destination.y - start.y) * sin((Double)dt * sin_y_max / (Double)teleportation_anim_duration) / sin(sin_y_max))/* * dt / teleportation_anim_duration*/;

  SetXY( start + dpos );
  image->Update();
}
