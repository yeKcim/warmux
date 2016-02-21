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
 *  wind Refresh (you may get cold ;) )
 *****************************************************************************/

#include "map/wind.h"
#include "map/camera.h"
#include "game/config.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "tool/debug.h"
#include "tool/random.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "interface/interface.h"
#include <SDL.h>
#include "game/config.h"
#include "graphic/sprite.h"
#include "gui/progress_bar.h"

const uint MAX_WIND_OBJECTS = 200;
const uint bar_speed = 20;

Wind wind;

WindParticle::WindParticle(const std::string &xml_file, float scale) :
  PhysicalObj("wind", xml_file)
{
  SetCollisionModel(true, false, false);

  // Physic constants
  double mass, wind_factor ;
  //Mass = mass_mean + or - 25%
  mass = GetMass();
  mass *= (1.0 + RandomLocal().GetLong(-100, 100)/400.0);
  SetMass (mass);
  SetSize( Point2i(20,20) );
  wind_factor = GetWindFactor() ;
  wind_factor *= (1.0 + RandomLocal().GetLong(-100, 100)/400.0);
  SetWindFactor(wind_factor);
  SetAirResistFactor(GetAirResistFactor() * (1.0 + RandomLocal().GetLong(-100, 100)/400.0));

  MSG_DEBUG("wind", "Create wind particle: %s, %f, %f", xml_file.c_str(), mass, wind_factor);

  // Fixe test rectangle
  int dx = 0 ;
  int dy = 0 ;
  SetTestRect (dx, dx, dy, dy);

  m_allow_negative_y = true;

  // Sprite loading
  scale = 0.5 + scale / 2.0;

  sprite = resource_manager.LoadSprite( ActiveMap()->ResProfile(), "wind_particle");
  sprite->Scale(scale, scale);
  sprite->RefreshSurface();
  sprite->SetAlpha(scale);
  sprite->SetCurrentFrame(RandomLocal().GetLong(0, sprite->GetFrameCount() - 1));

  if(ActiveMap()->GetWind().need_flip) {
    flipped = new Sprite(*sprite);
    flipped->Scale(-scale, scale);
    flipped->RefreshSurface();
    flipped->SetAlpha(scale);
    flipped->SetCurrentFrame(RandomLocal().GetLong(0, sprite->GetFrameCount()-1));
  } else {
    flipped = NULL;
  }

  if(ActiveMap()->GetWind().rotation_speed != 0.0) {
    sprite->EnableRotationCache(64);
    sprite->SetRotation_rad(RandomLocal().GetLong(0,628)/100.0); // 0 < angle < 2PI
    if(flipped) {
      flipped->EnableRotationCache(64);
      flipped->SetRotation_rad(RandomLocal().GetLong(0,628)/100.0); // 0 < angle < 2PI
    }
  }
}

WindParticle::~WindParticle()
{
  delete sprite;
  if(flipped) delete flipped;
}

void WindParticle::Refresh()
{
  if(flipped && GetSpeed().x < 0)
    flipped->Update();
  else
    sprite->Update();

  // Rotate the sprite if needed
  if(ActiveMap()->GetWind().rotation_speed != 0.0)
  {
    if(flipped && GetSpeed().x < 0)
    {
      float new_angle = flipped->GetRotation_rad() + ActiveMap()->GetWind().rotation_speed;
      flipped->SetRotation_rad(new_angle);
    }
    else
    {
      float new_angle = sprite->GetRotation_rad() + ActiveMap()->GetWind().rotation_speed;
      sprite->SetRotation_rad(new_angle);
    }
  }
  // Put particles inside of the camera view
  // (there is no point in computing particle out of the camera!)
  int x = GetX();
  int y = GetY();

  if(GetX() > Camera::GetInstance()->GetPositionX() + Camera::GetInstance()->GetSizeX())
    x -= Camera::GetInstance()->GetSizeX() + (int)sprite->GetWidth() - 1;

  if(GetX() + (int)sprite->GetWidth() < Camera::GetInstance()->GetPositionX() )
    x += Camera::GetInstance()->GetSizeX() + (int)sprite->GetWidth() - 1;

  if(GetY() > Camera::GetInstance()->GetPositionY() + Camera::GetInstance()->GetSizeY())
    y -= Camera::GetInstance()->GetSizeY() + (int)sprite->GetHeight() - 1;

  if(GetY() + (int)sprite->GetHeight() < Camera::GetInstance()->GetPositionY() )
    y += Camera::GetInstance()->GetSizeY() + (int)sprite->GetHeight() - 1;

  m_alive = ALIVE;

  if (m_alive != ALIVE || x!=GetX() || y!=GetY())
  {
    m_alive = ALIVE;
    StartMoving();
    SetXY( Point2i(x, y) );
    MSG_DEBUG("wind", "new position %d, %d - mass %f, wind_factor %f", x, y, GetMass(), GetWindFactor());
  }

  UpdatePosition();
}

void WindParticle::Draw()
{
  // Use the flipped sprite if needed and if the direction of wind changed
  if(flipped && GetSpeed().x < 0)
    flipped->Draw(GetPosition());
  else
    sprite->Draw(GetPosition());
}

//---------------------------------------------------

void Wind::RemoveAllParticles()
{
  iterator it=particles.begin(), end=particles.end();
  while (it != end) {
    delete (*it);
    it = particles.erase(it);
  }
}

void Wind::Reset(){
  m_last_move = 0;
  m_last_part_mvt = 0;
  m_val = m_nv_val = 0;
  Interface::GetInstance()->UpdateWindIndicator(m_val);

  RemoveAllParticles();

  if (!Config::GetInstance()->GetDisplayWindParticles())
    return;

  uint nb = ActiveMap()->GetWind().nb_sprite;

  if (!nb) return;

  std::string config_file = ActiveMap()->GetConfigFilepath();

  for (uint i=0; i<nb; ++i){
    WindParticle *tmp = new WindParticle(config_file, (float)i / nb);
    particles.push_back(tmp);
  }
  RandomizeParticlesPos();
}

void Wind::ChooseRandomVal() const{
  int val = RandomLocal().GetLong(-100, 100);
  ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_WIND, val));
}

void Wind::DrawParticles(){
  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it)
    (*it)->Draw();
}

void Wind::Refresh(){
  if(m_last_move + bar_speed < Time::GetInstance()->Read()){
    if(m_val>m_nv_val)
      --m_val;
    else
    if(m_val<m_nv_val)
      ++m_val;
    m_last_move = Time::GetInstance()->Read();
    Interface::GetInstance()->UpdateWindIndicator(m_val);
  }

  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it)
    (*it)->Refresh();
}

void Wind::RandomizeParticlesPos()
{
  iterator it=particles.begin(), end=particles.end();

  MSG_DEBUG("wind", "camera position: %d, %d - %d, %d", Camera::GetInstance()->GetPositionX(),
            Camera::GetInstance()->GetPositionX()+Camera::GetInstance()->GetSizeX(), Camera::GetInstance()->GetPositionY(), Camera::GetInstance()->GetPositionY()+Camera::GetInstance()->GetSizeY());

  for (; it != end; ++it)
  {
    (*it)->SetXY(Point2i( RandomLocal().GetLong(Camera::GetInstance()->GetPositionX(), Camera::GetInstance()->GetPositionX()+Camera::GetInstance()->GetSizeX()),
                          RandomLocal().GetLong(Camera::GetInstance()->GetPositionY(), Camera::GetInstance()->GetPositionY()+Camera::GetInstance()->GetSizeY())));
    MSG_DEBUG("wind", "new particule position: %d, %d", (*it)->GetX(), (*it)->GetY());
  }
}
