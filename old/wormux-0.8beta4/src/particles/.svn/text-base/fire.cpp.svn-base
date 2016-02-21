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

#include "particles/fire.h"
#include "particles/particle.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "tool/random.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

const uint living_time = 5000;
const uint dig_ground_time = 1000;

ExplosiveWeaponConfig fire_cfg;

FireParticle::FireParticle() :
  Particle("fire_particle"),
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
  image->SetRotation_HotSpot(bottom_center);
  SetSize(image->GetSize());
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
  image->Scale(scale, scale);

  if(image->GetSize().x != 0 && image->GetSize().y != 0)
  {
    int dx = (GetWidth() - image->GetWidth()) / 2;

    SetTestRect(dx, dx - 1, GetHeight() - 2,1);
  }

  // The position of the object represents its top left corner
  // So, since we are resizing the object, we have to move it
  // to make it appear at the same place

  if(on_ground || !FootsInVacuum())
  {
    on_ground = true;
    if((now + oscil_delta) / dig_ground_time != (m_last_refresh + oscil_delta) / dig_ground_time)
    {
      Point2i expl_pos = GetPosition() + GetSize();
      expl_pos.x -= GetWidth()/2;

      ApplyExplosion(expl_pos, fire_cfg, "", false, ParticleEngine::LittleESmoke);
      fire_cfg.explosion_range = (uint)(scale * image->GetWidth()) + 1;
      fire_cfg.particle_range = (uint)(1.1 * scale * image->GetWidth()) + 1;
    }

    double angle = cos((((now + oscil_delta) % 1000)/500.0) * M_PI) * 0.5; // 0.5 is arbirtary
    image->SetRotation_rad( angle);
  }
  else
  {
    double angle = GetSpeedAngle();
    image->SetRotation_rad((angle - M_PI_2));
  }

  m_last_refresh = now;
}

void FireParticle::Draw()
{
  Point2i draw_pos = GetPosition();
  draw_pos.y += GetHeight()/2;
  image->Draw( draw_pos );
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
