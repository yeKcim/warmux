
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

#include "particles/explosion_smoke.h"
#include "particles/particle.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "tool/random.h"

ExplosionSmoke::ExplosionSmoke(const uint size_init) :
  Particle("explosion_smoke_particle")
{
  m_initial_size = size_init;
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 25;
  dx = 0;

  image = ParticleEngine::GetSprite(EXPLOSION_SMOKE_spr);
  mvt_freq = randomObj.GetDouble(-2.0, 2.0);
  SetGravityFactor(randomObj.GetDouble(-1.0,-2.0));

  image->ScaleSize(m_initial_size, m_initial_size);
  SetSize( Point2i(1, 1) );
  StartMoving();
}

void ExplosionSmoke::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;

  UpdatePosition();

  image->Update();

  if (time >= m_time_between_scale) {
    //ASSERT(m_left_time_to_live > 0);
    if (m_left_time_to_live <= 0) return ;

    m_left_time_to_live--;

    float lived_time = m_initial_time_to_live - m_left_time_to_live;

    float coeff = cos((M_PI/2.0)*((float)lived_time/((float)m_initial_time_to_live)));
    image->ScaleSize(int(coeff * m_initial_size),int(coeff * m_initial_size));

    dx = int((int)m_initial_size * sin(5.0 * ((float)lived_time/((float)m_initial_time_to_live)) * M_PI * mvt_freq / 2.0) / 2);
    m_last_refresh = Time::GetInstance()->Read() ;
  }
}

void ExplosionSmoke::Draw()
{
  if (m_left_time_to_live > 0)
    image->Draw(GetPosition()+Point2i(dx,0));
}
