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
 * Particle Engine
 *****************************************************************************/

#include "particles/teleport_member.h"
#include "particles/particle.h"
#include "network/randomsync.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "map/camera.h"

TeleportMemberParticle::TeleportMemberParticle(Sprite& spr, const Point2i& position, const Point2i& dest) :
  Particle("teleport_member_particle")
{
  SetCollisionModel(false, false, false);
  spr.RefreshSurface(); // Make sure we have something to build on
  image = new Sprite(spr.GetSurface());

  ASSERT(image->GetWidth() && image->GetHeight());
  SetXY(position);
  m_time_left_to_live = 1;

  SetSize(image->GetSize());
  SetOnTop(true);
  destination = dest;
  start = position;
  time = GameTime::GetInstance()->Read();

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
  uint now = GameTime::GetInstance()->Read();
  if (now > time + TELEPORTATION_ANIM_DURATION)
    m_time_left_to_live = 0;

  uint dt = now - time;
  Point2i dpos = destination - start;
  dpos.x = dpos.x * sin(dt*sin_x_max / TELEPORTATION_ANIM_DURATION) / sin(sin_x_max);
  dpos.y = dpos.y * sin(dt*sin_y_max / TELEPORTATION_ANIM_DURATION) / sin(sin_y_max);

  SetXY(start + dpos);
  image->Update();
}
