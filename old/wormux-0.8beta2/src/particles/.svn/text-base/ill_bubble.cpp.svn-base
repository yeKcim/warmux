
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

#include "particles/ill_bubble.h"
#include "particles/explosion_smoke.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "tool/random.h"

// Vibration period of the bubble
const uint vib_period = 250;

IllBubble::IllBubble() : ExplosionSmoke(20)
{
  // delete the sprite of the ExplosionSmoke
  delete image;
  image = ParticleEngine::GetSprite(ILL_BUBBLE_spr);
  SetAirResistFactor( GetAirResistFactor() * 3.0 );
  vib_phi = randomObj.GetLong(0, vib_period);
}

void IllBubble::Draw()
{
  if (m_left_time_to_live > m_initial_time_to_live - 3)
    image->SetAlpha( (float)(m_initial_time_to_live - m_left_time_to_live) / 3.0 );
  else
    image->SetAlpha(1.0);

  uint time = (Time::GetInstance()->Read() + vib_phi) % vib_period;
  float scale_x, scale_y;
  image->GetScaleFactors(scale_x, scale_y);
  scale_x *= 1.0 + 0.2 * sin(2.0 * M_PI * time / (float)vib_period);
  scale_y *= 1.0 + 0.2 * cos(2.0 * M_PI * time / (float)vib_period);
  image->Scale(scale_x, scale_y);

  if (m_left_time_to_live > 0)
    image->Draw(GetPosition()+Point2i(dx,0) - image->GetSize() / 2);
}
