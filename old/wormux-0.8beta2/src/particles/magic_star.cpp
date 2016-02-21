
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

#include "magic_star.h"
#include "particle.h"
#include "tool/random.h"
#include "game/time.h"
#include "graphic/sprite.h"

MagicStarParticle::MagicStarParticle() :
  Particle("magic_star_particle")
{
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 25;

  uint color=randomObj.GetLong(0,2);
  switch(color)
  {
    case 0 : image = ParticleEngine::GetSprite(MAGIC_STAR_R_spr); break;
    case 1 : image = ParticleEngine::GetSprite(MAGIC_STAR_Y_spr); break;
    case 2 : image = ParticleEngine::GetSprite(MAGIC_STAR_B_spr); break;
    default: ASSERT(false);
  }
  image->Scale(0.0, 0.0);
  SetSize( Point2i(1, 1) );
}

void MagicStarParticle::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;
  if (time >= m_time_between_scale) {
    if (m_left_time_to_live <= 0) return ;
    float lived_time = m_initial_time_to_live - m_left_time_to_live;
    float coeff = sin(M_PI_2*((float)lived_time/((float)m_initial_time_to_live)));
    image->SetRotation_rad(coeff * 2 * M_PI);
  }
  Particle::Refresh();
}
