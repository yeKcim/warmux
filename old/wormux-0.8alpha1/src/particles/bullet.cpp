/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "bullet.h"
#include "particle.h"
#include "../game/time.h"

BulletParticle::BulletParticle() :
  Particle("bullet_particle")
{
  SetCollisionModel(false, false, false);
  m_rebound_sound = "weapon/grenade_bounce";
  m_left_time_to_live = 1;

  image = ParticleEngine::GetSprite(BULLET_spr);
  image->Scale(1.0,1.0);
  SetSize( Point2i(1, 1) );
}

void BulletParticle::Refresh()
{
  UpdatePosition();
  image->SetRotation_deg((Time::GetInstance()->Read()/4) % 360);
  image->Update();
  if(IsOutsideWorldXY(GetPosition()))
  {
    m_left_time_to_live = 0;
  }
}

void BulletParticle::SignalRebound()
{
  PhysicalObj::SignalRebound();
  SetCollisionModel(true, false, false);
}
