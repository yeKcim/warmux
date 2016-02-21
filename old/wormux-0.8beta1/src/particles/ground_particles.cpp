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

#include "ground_particles.h"
#include "particle.h"
#include "game/time.h"
#include "map/map.h"

GroundParticle::GroundParticle(const Point2i& size, const Point2i& position) :
  Particle("ground_particle")
{
  SetCollisionModel(true, false, false);
  SetSize(Point2i(1,1));
  m_initial_time_to_live = 1; // used as a boolean because we redefine Refresh!
  m_left_time_to_live = 1; // used as a boolean because we redefine Refresh!
  image = NULL;

  Rectanglei rec;
  rec.SetPosition( position - size / 2);
  rec.SetSize( size );
  image = new Sprite(world.ground.GetPart(rec));
}

void GroundParticle::Refresh()
{
  UpdatePosition();
  image->SetRotation_rad(Time::GetInstance()->Read() / 180.0 * M_PI);
  image->Update();
  if(IsOutsideWorld()) {
    m_left_time_to_live = 0;
  }
}
