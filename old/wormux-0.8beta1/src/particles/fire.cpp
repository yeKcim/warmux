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

#include "fire.h"
#include "particle.h"
#include "game/time.h"
#include "tool/random.h"
#include "weapon/explosion.h"
#include "map/camera.h"

const uint living_time = 5000;
const uint dig_ground_time = 1000;

ExplosiveWeaponConfig fire_cfg;

FireParticle::FireParticle() :
  Particle("fire_particle"),
  direction(randomObj.GetBool() ? -1 : 1),
  creation_time(Time::GetInstance()->Read()),
  on_ground(false),
  oscil_delta(randomObj.GetLong(0, dig_ground_time))
{
  SetCollisionModel(false, false, false);
  m_left_time_to_live = 100;
  m_check_move_on_end_turn = true;

  fire_cfg.damage = 1;
  fire_cfg.explosion_range = 5;
  fire_cfg.blast_range = 0;
  fire_cfg.blast_force = 0;
  fire_cfg.particle_range = 6;

  image = ParticleEngine::GetSprite(FIRE_spr);
  image->SetRotation_HotSpot(Point2i(image->GetWidth()/2,image->GetHeight()));
  SetSize(image->GetSize());
  SetTestRect((image->GetWidth() / 2)-1, (image->GetWidth() / 2) - 1,
      (image->GetHeight()/2)-1,1);
}

FireParticle::~FireParticle()
{
}

void FireParticle::Refresh()
{
  uint now = Time::GetInstance()->Read();
  UpdatePosition();
  image->Update();

  if (creation_time + living_time < now)
    m_left_time_to_live = 0;

  float scale = (now - creation_time)/(float)living_time;
  scale = 1.0 - scale;
  image->Scale((float)direction * scale, scale);

  if(image->GetSize().x != 0 && image->GetSize().y != 0)
  {
    SetSize( image->GetSize() );
    SetTestRect((image->GetWidth() / 2)-1, (image->GetWidth() / 2) - 1,
      (image->GetHeight()/2)-1,1);
  }
  else
    SetSize( Point2i(1,1) );


  if(on_ground || !FootsInVacuum())
  {
    on_ground = true;
    if((now + oscil_delta) / dig_ground_time != (m_last_refresh + oscil_delta) / dig_ground_time)
    {
      ApplyExplosion(Point2i(GetCenter().x,(GetY() + GetHeight() + GetCenter().y)/2), fire_cfg, "", false, ParticleEngine::LittleESmoke);
      fire_cfg.explosion_range = (uint)(0.5 * scale * image->GetWidth()) + 1;
      fire_cfg.particle_range = (uint)(0.6 * scale * image->GetWidth()) + 1;
    }
    double angle = 0.0;
    angle += cos((((now + oscil_delta) % 1000)/500.0) * M_PI) * 0.5; // 0.5 is arbirtary
    image->SetRotation_HotSpot(Point2i(image->GetWidth()/2,image->GetHeight()));
    image->SetRotation_rad( angle);
  }
  else
  {
    double angle = GetSpeedAngle();
    image->SetRotation_rad((angle - M_PI_2) * direction);
  }

  m_last_refresh = now;
}

void FireParticle::Draw()
{
  image->Draw(Point2i(GetX(),GetY()));
}

void FireParticle::SignalDrowning()
{
  m_left_time_to_live = 0;
  // jukebox.Play("share","fire/pschiit");
}

void FireParticle::SignalOutOfMap()
{
  m_left_time_to_live = 0;
}
