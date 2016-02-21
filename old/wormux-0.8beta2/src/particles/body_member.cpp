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
 * Particle Engine
 *****************************************************************************/

#include "body_member.h"
#include "particle.h"
#include "graphic/sprite.h"
#include "tool/random.h"

BodyMemberParticle::BodyMemberParticle(const Sprite* spr, const Point2i& position) :
  Particle("body_member_particle")
{
  SetCollisionModel(false, false, false);
  m_left_time_to_live = 100;
  image = new Sprite(spr->GetSurface());
  image->EnableRotationCache(32);
  ASSERT(image->GetWidth() != 0 && image->GetHeight()!=0);
  SetXY(position);

  SetSize(image->GetSize());
  SetOnTop(true);
  SetSpeed( (double)randomObj.GetLong(10, 15),
        - (double)randomObj.GetLong(0, 3000)/1000.0);
}

void BodyMemberParticle::Refresh()
{
  m_left_time_to_live--;
  UpdatePosition();

  angle_rad += GetSpeedXY().Norm() * 20;
  angle_rad = fmod(angle_rad, 2 *M_PI);
  //FIXME what about negatives values ? what would happen ?
  if(m_left_time_to_live < 50)
    image->SetAlpha(m_left_time_to_live / 50.0);
  image->SetRotation_rad(angle_rad);
  image->Update();
}
