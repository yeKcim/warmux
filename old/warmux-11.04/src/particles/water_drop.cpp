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

#include "game/game_time.h"
#include "graphic/sprite.h"
#include "particles/water_drop.h"
#include "particles/particle.h"
#include "tool/resource_manager.h"

const uint living_time = 5000;

WaterParticle::WaterParticle()
  : Particle("water_particle")
{
  assert(ActiveMap()->GetWaterType() != "no");
  particle_spr type = CLEARWATER_spr;
  if (ActiveMap()->GetWaterType() == "lava")
    type = LAVA_spr;
  else if (ActiveMap()->GetWaterType() == "radioactive")
    type = RADIOACTIVE_spr;
  else if (ActiveMap()->GetWaterType() == "dirtywater")
    type = DIRTYWATER_spr;
  else if (ActiveMap()->GetWaterType() == "chocolate_drop")
    type = CHOCOLATEWATER_spr;

  SetDefaults(type);
}

WaterParticle::WaterParticle(particle_spr type)
  : Particle("water_particle")
{
  SetDefaults(type);
}

void WaterParticle::SetDefaults(particle_spr type)
{
  SetCollisionModel(true, false, false);
  m_time_left_to_live = 100;
  m_check_move_on_end_turn = false;

  image = new Sprite(*ParticleEngine::GetSprite(type));

  image->SetRotation_HotSpot(bottom_center);
  SetSize(image->GetSize());
}

WaterParticle::~WaterParticle()
{
  // Don't delete image here, it will be freed through ~Particle
}

void WaterParticle::Refresh()
{
  // No need to continue if no longer really existing
  if (!m_time_left_to_live)
    return;

  uint now = GameTime::GetInstance()->Read();
  UpdatePosition();
  image->Update();

  if (image->GetSize().x != 0 && image->GetSize().y != 0) {
    int dx = (GetWidth() - image->GetWidth()) / 2;
    int dy = std::max(0, GetHeight() - 2);
    SetTestRect(dx, dx, dy, 1);
  }

  Double angle = GetSpeedAngle();
  image->SetRotation_rad((angle - HALF_PI));

  m_last_refresh = now;
}

void WaterParticle::Draw()
{
  // No need to continue if no longer really existing
  if (!m_time_left_to_live)
    return;

  Point2i draw_pos = GetPosition();
  draw_pos.y += GetHeight()/2;
  image->Draw( draw_pos );
}
