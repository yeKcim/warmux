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

#include "particles/water_drop.h"
#include "particles/particle.h"
#include "game/time.h"
#include "graphic/sprite.h"

const uint living_time = 5000;

WaterParticle::WaterParticle() :
  Particle("water_particle")
{
  SetCollisionModel(true, false, false);
  m_left_time_to_live = 100;
  m_check_move_on_end_turn = false;

  image = ParticleEngine::GetSprite(WATER_spr);
  image->SetRotation_HotSpot(bottom_center);
  SetSize(image->GetSize());
}

WaterParticle::~WaterParticle()
{
}

void WaterParticle::Refresh()
{
  uint now = Time::GetInstance()->Read();
  UpdatePosition();
  image->Update();

  if (image->GetSize().x != 0 && image->GetSize().y != 0)
  {
    int dx = (GetWidth() - image->GetWidth()) / 2;

    SetTestRect(dx, dx-1, GetHeight() - 2,1);
  }

  double angle = GetSpeedAngle();
  image->SetRotation_rad((angle - M_PI_2));

  m_last_refresh = now;
}

void WaterParticle::Draw()
{
  Point2i draw_pos = GetPosition();
  draw_pos.y += GetHeight()/2;
  image->Draw( draw_pos );
}

void WaterParticle::SignalDrowning()
{
  m_left_time_to_live = 0;
}

void WaterParticle::SignalOutOfMap()
{
  m_left_time_to_live = 0;
}

LavaParticle::LavaParticle()
{
  // delete std water image
  delete image;
  image = ParticleEngine::GetSprite(LAVA_spr);
}
