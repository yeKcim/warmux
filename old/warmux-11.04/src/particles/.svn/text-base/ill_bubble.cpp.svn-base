
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

#include "particles/ill_bubble.h"
#include "particles/explosion_smoke.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "network/randomsync.h"

// Vibration period of the bubble
static const uint vib_period = 250;
static const Double MAX_SCALE = ONE;
static const Double MIN_SCALE = 0.25f;

IllBubble::IllBubble() : ExplosionSmoke(20)
{
  // delete the sprite of the ExplosionSmoke
  delete image;
  image = ParticleEngine::GetSprite(ILL_BUBBLE_spr);
  SetAirResistFactor( GetAirResistFactor() * THREE );

  MSG_DEBUG("random.get", "IllBubble::IllBubble()");
  vib_phi = RandomSync().GetUint(0, vib_period);
  image->Scale(MIN_SCALE, MIN_SCALE);
}

void IllBubble::Refresh()
{
  ExplosionSmoke::Refresh();
  uint time = (GameTime::GetInstance()->Read() + vib_phi) % vib_period;

  Double scale_x, scale_y;
  image->GetScaleFactors(scale_x, scale_y);
  Double scale_factor = 1.2;
  Double dangle = time*TWO_PI / vib_period;
  scale_x *= scale_factor * sin(dangle);
  scale_y *= scale_factor * cos(dangle);
  scale_x = std::max(MIN_SCALE, std::min(scale_x, MAX_SCALE));
  scale_y = std::max(MIN_SCALE, std::min(scale_y, MAX_SCALE));
  // scale_x and scale_y should be now >= 0.25f so no flipping to do.
  image->Scale(scale_x, scale_y);
}

void IllBubble::Draw()
{
  if (m_time_left_to_live > m_initial_time_to_live - 3)
    image->SetAlpha( (Double)(m_initial_time_to_live - m_time_left_to_live) / THREE );
  else
    image->SetAlpha(ONE);

  if (m_time_left_to_live > 0)
    image->Draw(GetPosition() - image->GetSize() / 2);
}

