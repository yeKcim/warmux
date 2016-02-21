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
 *  wind Refresh (you may get cold ;) )
 *****************************************************************************/

#include "wind.h"
#include "camera.h"
#include "game/config.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "tool/random.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "interface/interface.h"

const uint MAX_WIND_OBJECTS = 200;
const uint BARRE_LARG = 80;
const uint BARRE_HAUT = 10;
const double force = 5; // Max wind strength in m/(sec*sec)
const uint bar_speed = 20;

Wind wind;

WindParticle::WindParticle(std::string &xml_file, float scale) :
  PhysicalObj("wind",xml_file)
{
  SetCollisionModel(true, false, false);

  // Physic constants
  double mass, wind_factor ;
  //Mass = mass_mean + or - 25%
  mass = GetMass();
  mass *= (1.0 + randomObj.GetLong(-100, 100)/400.0);
  SetMass (mass);
  SetSize( Point2i(20,20) );
  wind_factor = GetWindFactor() ;
  wind_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);
  SetWindFactor(wind_factor);
  StartMoving();
  SetAirResistFactor(GetAirResistFactor() * (1.0 + randomObj.GetLong(-100, 100)/400.0));

  // Fixe test rectangle
  int dx = 0 ;
  int dy = 0 ;
  SetTestRect (dx, dx, dy, dy);

  m_allow_negative_y = true;

  // Sprite loading
  scale = 0.5 + scale / 2.0;

  Sprite* tmp = resource_manager.LoadSprite( ActiveMap().ResProfile(), "wind_particle");
  tmp->Scale(scale, scale);
  tmp->RefreshSurface();
  sprite = new Sprite(tmp->GetSurface());
  sprite->SetAlpha(scale);
  sprite->SetCurrentFrame ( randomObj.GetLong(0, sprite->GetFrameCount()-1));

  if(ActiveMap().wind.need_flip)
  {
    tmp->Scale(-scale, scale);
    tmp->RefreshSurface();
    flipped = new Sprite(tmp->GetSurface());
    flipped->SetAlpha(scale);
    flipped->SetCurrentFrame ( randomObj.GetLong(0, sprite->GetFrameCount()-1));
  }
  else
    flipped = NULL;

  delete tmp;


  if(ActiveMap().wind.rotation_speed != 0.0)
  {
    sprite->EnableRotationCache(64);
    sprite->SetRotation_rad(randomObj.GetLong(0,628)/100.0); // 0 < angle < 2PI
    if(flipped)
    {
      flipped->EnableRotationCache(64);
      flipped->SetRotation_rad(randomObj.GetLong(0,628)/100.0); // 0 < angle < 2PI
    }
  }
}

void WindParticle::Refresh()
{
  if(flipped && GetSpeed().x < 0)
    flipped->Update();
  else
    sprite->Update();

  UpdatePosition();

  // Rotate the sprite if needed
  if(ActiveMap().wind.rotation_speed != 0.0)
  {
    if(flipped && GetSpeed().x < 0)
    {
      float new_angle = flipped->GetRotation_rad() + ActiveMap().wind.rotation_speed;
      flipped->SetRotation_rad(new_angle);
    }
    else
    {
      float new_angle = sprite->GetRotation_rad() + ActiveMap().wind.rotation_speed;
      sprite->SetRotation_rad(new_angle);
    }
  }
  // Put particles inside of the camera view
  // (there is no point in computing particle out of the camera!)
  int x = GetX();
  int y = GetY();

  if(GetX() > camera.GetPositionX() + camera.GetSizeX())
    x = camera.GetPositionX() - GetWidth() + 1;

  if(GetX() + GetWidth() < camera.GetPositionX() )
    x = camera.GetPositionX() + camera.GetSizeX() - 1;

  if(GetY() > camera.GetPositionY() + camera.GetSizeY())
    y = camera.GetPositionY() - GetHeight() + 1;

  if(GetY() + GetHeight() < camera.GetPositionY() )
    y = camera.GetPositionY() + camera.GetSizeY() - 1;

  m_alive = ALIVE;

  if(x!=GetX() || y!=GetY())
  {
    StartMoving();
    SetXY( Point2i(x, y) );
  }
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

Wind::Wind(){
  m_val = m_nv_val = 0;
}

Wind::~Wind()
{
  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it)
    delete (*it);
}

void Wind::Reset(){
  m_last_move = 0;
  m_last_part_mvt = 0;
  m_val = m_nv_val = 0;
  Interface::GetInstance()->UpdateWindIndicator(m_val);

  particles.clear();

  if (!Config::GetInstance()->GetDisplayWindParticles())
    return ;

  uint nb = ActiveMap().wind.nb_sprite;

  if(!nb) return;

  std::string config_file = ActiveMap().m_directory + PATH_SEPARATOR + "config.xml";

  for (uint i=0; i<nb; ++i){
    WindParticle *tmp = new WindParticle(config_file, (float)i / nb);
    particles.push_back(tmp);
  }
  RandomizeParticlesPos();
}

double Wind::GetStrength() const{
  return m_nv_val * force / 100.0;
}

void Wind::ChooseRandomVal(){
  int val = randomObj.GetLong(-100, 100);
  ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_WIND, val));
}

void Wind::SetVal(long val){
  m_nv_val = val;
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
  for (; it != end; ++it)
  {
    if(!camera.IsVisible(**it))
      (*it)->SetXY(Point2i( randomObj.GetLong(camera.GetPositionX(), camera.GetPositionX()+camera.GetSizeX()),
                            randomObj.GetLong(camera.GetPositionY(), camera.GetPositionY()+camera.GetSizeY())));
  }
}
