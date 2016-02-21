
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

#include "particles/explosion_smoke.h"
#include "particles/particle.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "network/randomsync.h"

ExplosionSmoke::ExplosionSmoke(const uint size_init) :
  Particle("explosion_smoke_particle")
{
  m_initial_size = size_init;


  image = ParticleEngine::GetSprite(EXPLOSION_BIG_SMOKE_spr);
  m_initial_time_to_live = image->GetFrameCount();
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = image->GetCurrentFrameObject().delay;

  image->SetCurrentFrame(0);
  image->Start();

  MSG_DEBUG("random.get", "ExplosionSmoke::ExplosionSmoke(...)");
  SetGravityFactor(RandomSync().GetDouble(-1.0,-2.0));

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
    m_last_refresh = Time::GetInstance()->Read() ;
  }
}

void ExplosionSmoke::Draw()
{
  if (m_left_time_to_live > 0){
    image->Draw(GetPosition()-Point2i(m_initial_size/2,0));
  }

}
