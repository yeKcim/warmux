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
 *  wind Refresh (you may get cold ;) )
 *****************************************************************************/

#include "map/wind.h"
#include "map/camera.h"
#include "game/config.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "network/randomsync.h"
#include <WARMUX_debug.h>
#include <WARMUX_random.h>
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "tool/string_tools.h"
#include "interface/interface.h"
#include <SDL.h>
#include "game/config.h"
#include "graphic/sprite.h"
#include "gui/progress_bar.h"

const uint MAX_WIND_OBJECTS = 200;
const uint bar_speed = 1;

static bool need_flip = false;

WindParticle::WindParticle(const std::string &xml_file, Double scale)
  : PhysicalObj("wind", xml_file)
{
  SetCollisionModel(false, false, false);
  CanBeGhost(false);

  // Physic constants
  Double mass = GetMass();
  mass *= RandomLocal().GetDouble(0.75, 1.25); //Mass = mass_mean + or - 25%
  SetMass(mass);

  SetSize(Point2i(20,20));
  Double wind_factor = GetWindFactor();
  wind_factor *= RandomLocal().GetDouble(0.75, 1.25);
  SetWindFactor(wind_factor);

  SetAirResistFactor(GetAirResistFactor() * RandomLocal().GetDouble(0.75, 1.25));

  MSG_DEBUG("wind", "Create wind particle: %s, %s, %s", xml_file.c_str(),
            Double2str(mass).c_str(), Double2str(wind_factor).c_str());

  // Fixe test rectangle
  int dx = 0;
  int dy = 0;
  SetTestRect(dx, dx, dy, dy);

  m_allow_negative_y = true;

  // Sprite loading
  scale = ONE_HALF * (1 + scale);

  sprite = GetResourceManager().LoadSprite(ActiveMap()->ResProfile(), "wind_particle");
  sprite->Scale(scale, scale);
  sprite->SetAlpha(scale);
  sprite->SetCurrentFrame(RandomLocal().GetInt(0, sprite->GetFrameCount() - 1));
  SetSize(sprite->GetSize());

  bool not_fixed = GetAlignParticleState()|| ActiveMap()->GetWind().rotation_speed.IsNotZero();
  sprite->FixParameters(false);
  need_flip = ActiveMap()->GetWind().need_flip;
  sprite->EnableCaches(need_flip, not_fixed ? 64 : 0);
  if (not_fixed) {
    sprite->SetRotation_rad(RandomLocal().GetInt(0,628)/100.0); // 0 < angle < 2PI
  }

  // Now that caches have been set, refresh
  sprite->RefreshSurface();
}

WindParticle::~WindParticle()
{
  delete sprite;
}

void WindParticle::Refresh()
{
  if (need_flip)
    sprite->SetFlipped(GetSpeed().x < 0);
  sprite->Update();

  const Double& rotation_speed = ActiveMap()->GetWind().rotation_speed;
  if (GetAlignParticleState()) {
    sprite->SetRotation_rad(GetSpeedAngle() - HALF_PI);
  }
  else if (rotation_speed.IsNotZero()) {
    // Rotate the sprite if needed
    sprite->SetRotation_rad(sprite->GetRotation_rad() + rotation_speed);
  }

  // Put particles inside of the camera view
  // (there is no point in computing particle out of the camera!)
  int x = GetX();
  int y = GetY();

  const Camera *cam = Camera::GetConstInstance();
  if (GetX() > cam->GetPositionX() + cam->GetSizeX())
    x -= cam->GetSizeX() + (int)sprite->GetWidth() - 1;

  if (GetX() + (int)sprite->GetWidth() < cam->GetPositionX())
    x += cam->GetSizeX() + (int)sprite->GetWidth() - 1;

  if (GetY() > cam->GetPositionY() + cam->GetSizeY())
    y -= cam->GetSizeY() + (int)sprite->GetHeight() - 1;

  if (GetY() + (int)sprite->GetHeight() < cam->GetPositionY())
    y += cam->GetSizeY() + (int)sprite->GetHeight() - 1;

  if (m_alive != ALIVE || x != GetX() || y != GetY()) {
    m_alive = ALIVE;
    StartMoving();
    SetXY(Point2i(x, y));
    MSG_DEBUG("wind", "new position %d, %d - mass %s, wind_factor %s", x, y,
              Double2str(GetMass()).c_str(), Double2str(GetWindFactor()).c_str());
  }

  UpdatePosition();
}

void WindParticle::Draw()
{
  if (!IsInWater()) {
    // Use the flipped sprite if needed and if the direction of wind changed
    sprite->Draw(GetPosition());
  }
}

//---------------------------------------------------

Wind::Wind():
  m_val(0),
  m_nv_val(0),
  m_last_move(0),
  m_last_part_mvt(0)
{
}

void Wind::RemoveAllParticles()
{
  iterator it = particles.begin(), end = particles.end();
  while (it != end) {
    delete (*it);
    it = particles.erase(it);
  }
}

void Wind::Reset()
{
  m_last_move = 0;
  m_last_part_mvt = 0;
  m_val = m_nv_val = 0;
  Interface::GetInstance()->UpdateWindIndicator(m_val);

  RemoveAllParticles();
  uint nb = Config::GetConstInstance()->GetWindParticlesPercentage();

  if (!nb) {
    return;
  }

  nb = (ActiveMap()->GetWind().nb_sprite * nb) / 100;
  if (!nb) {
    return;
  }

  std::string config_file = ActiveMap()->GetConfigFilepath();

  for (uint i = 0; i < nb; ++i) {
    particles.push_back(new WindParticle(config_file, (Double)i / nb));
  }

  RandomizeParticlesPos();
}

void Wind::SetVal(int val)
{
  if (val > 100) m_nv_val = 100;
  else if (val < -100) m_nv_val = -100;
  else
    m_nv_val = val;
}

void Wind::ChooseRandomVal()
{
  MSG_DEBUG("random.get", "Wind::ChooseRandomVal()");
  SetVal(RandomSync().GetInt(-100, 100));
}

void Wind::UpdateStrength()
{
  MSG_DEBUG("random.get", "Wind::UpdateStrength()");
  SetVal(m_nv_val + RandomSync().GetInt(-50, 50));
}

void Wind::DrawParticles()
{
  iterator it = particles.begin(), end = particles.end();
  for (; it != end; ++it) {
    (*it)->Draw();
  }
}

void Wind::Refresh()
{
  if (m_last_move + bar_speed < GameTime::GetInstance()->Read()) {
    if (m_val > m_nv_val) {
      --m_val;
    } else if (m_val < m_nv_val) {
      ++m_val;
    }
    m_last_move = GameTime::GetInstance()->Read();
    Interface::GetInstance()->UpdateWindIndicator(m_val);
  }

  iterator it = particles.begin(), end = particles.end();

  for (; it != end; ++it) {
    (*it)->Refresh();
  }
}

void Wind::RandomizeParticlesPos()
{
  iterator it = particles.begin(), end = particles.end();

  const Camera *cam = Camera::GetConstInstance();
  int sx = cam->GetPositionX();
  int sy = cam->GetPositionY();
  int ex = sx + cam->GetSizeX();
  int ey = sy + cam->GetSizeY();
  MSG_DEBUG("wind", "camera position: %d, %d - %d, %d", sx, ex, sy, ey);

  for (; it != end; ++it) {
    (*it)->SetXY(Point2i(RandomLocal().GetInt(sx, ex),
                         RandomLocal().GetInt(sy, ey)));
    MSG_DEBUG("wind", "new particule position: %d, %d", (*it)->GetX(), (*it)->GetY());
  }
}
