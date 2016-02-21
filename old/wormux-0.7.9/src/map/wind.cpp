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
 *  Refresh du vent
 *****************************************************************************/

#include "wind.h"
#include "camera.h"
#include "../game/config.h"
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../tool/random.h"
#include "../tool/resource_manager.h"
#include "../tool/xml_document.h"
#include "../interface/interface.h"

const uint MAX_WIND_OBJECTS = 200;
const uint BARRE_LARG = 80;
const uint BARRE_HAUT = 10;
const double force = 5; // Max wind strength in m/(sec*sec)
const uint bar_speed = 20;

Wind wind;

WindParticle::WindParticle(std::string &xml_file) :
  PhysicalObj("wind",xml_file)
{
  SetCollisionModel(true, false, false);

  sprite = resource_manager.LoadSprite( ActiveMap().ResProfile(), "wind_particle");
//  if(sprite->GetFrameCount()==1)
//    sprite->cache.EnableLastFrameCache();
  sprite->SetCurrentFrame ( randomObj.GetLong(0, sprite->GetFrameCount()-1));

  double mass, wind_factor ;

  //Mass = mass_mean + or - 25%
  mass = GetMass();
  mass *= (1.0 + randomObj.GetLong(-100, 100)/400.0);
  SetMass (mass);
  SetSize( sprite->GetSize() );
  wind_factor = GetWindFactor() ;
  wind_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);
  SetWindFactor(wind_factor);
  StartMoving();
  SetAirResistFactor(GetAirResistFactor() * (1.0 + randomObj.GetLong(-100, 100)/400.0));

  // Fixe le rectangle de test
  int dx = 0 ;
  int dy = 0 ;
  SetTestRect (dx, dx, dy, dy);

  m_allow_negative_y = true;
}

void WindParticle::Refresh()
{
  sprite->Update();
  UpdatePosition();

  // Flip the sprite if needed and if the direction of wind changed
  if(ActiveMap().wind.need_flip)
  {
    Point2d speed;
    GetSpeedXY(speed);
    float scale_x, scale_y;
    sprite->GetScaleFactors( scale_x, scale_y);
    if((speed.x<0 && scale_x>0)
    || (speed.x>0 && scale_x<0))
    {
      scale_x=-scale_x;
      sprite->Scale( scale_x, scale_y);
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
  sprite->Draw(GetPosition());
}

void WindParticle::Resize(double size)
{
  size=0.5+size/2.0;
  sprite->Scale( size,size);
  sprite->SetAlpha( size);
}

//---------------------------------------------------

Wind::Wind(){
  m_val = m_nv_val = 0;
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
    WindParticle tmp = WindParticle(config_file);
    tmp.Resize( (double)i / nb );
    particles.push_back( tmp );
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
  for (; it != end; ++it) it -> Draw();
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
  for (; it != end; ++it) it -> Refresh();
}

void Wind::RandomizeParticlesPos()
{
  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it)
  {
    if(!camera.IsVisible(*it))
      it -> SetXY( Point2i( randomObj.GetLong(camera.GetPositionX(), camera.GetPositionX()+camera.GetSizeX()),
                            randomObj.GetLong(camera.GetPositionY(), camera.GetPositionY()+camera.GetSizeY())));
  }
}
