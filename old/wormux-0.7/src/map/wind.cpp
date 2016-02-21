/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/action_handler.h"
#include "../include/app.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../tool/random.h"
#include "../tool/resource_manager.h"
#include "../tool/xml_document.h"

const uint MAX_WIND_OBJECTS = 200;
const uint BARRE_LARG = 80;
const uint BARRE_HAUT = 10;
const double force = 5; // Force maximale du vent en m/(sec*sec)
const uint barre_speed = 20;

Wind wind;

WindParticle::WindParticle() :
  PhysicalObj("WindParticle", 0.0)
{
  m_type = objUNBREAKABLE;

  sprite = resource_manager.LoadSprite( TerrainActif().res_profile, "wind_particle");
  if(sprite->GetFrameCount()==1)
    sprite->cache.EnableLastFrameCache();
  sprite->SetCurrentFrame ( randomObj.GetLong(0, sprite->GetFrameCount()-1));
   
  SetXY( randomObj.GetPoint(world.GetSize()) );

  double mass, wind_factor ; 

  //Mass = mass_mean + or - 25%
  mass = TerrainActif().wind.particle_mass;
  mass *= (1.0 + randomObj.GetLong(-100, 100)/400.0);
  SetMass (mass);
  SetSize( sprite->GetSize() );
  wind_factor = TerrainActif().wind.particle_wind_factor ;
  wind_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);  
  SetWindFactor(wind_factor);
  StartMoving();
  m_air_resist_factor = TerrainActif().wind.particle_air_resist_factor ;
  m_air_resist_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);

  // Fixe le rectangle de test
  int dx = 0 ;
  int dy = 0 ;
  SetTestRect (dx, dx, dy, dy);

  m_allow_negative_y = true;
}

void WindParticle::Refresh()
{
  sprite->Update(); 
  
  double initial_air_resist_factor = m_air_resist_factor;
  m_air_resist_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);

  double initial_wind_factor = m_wind_factor;
  m_wind_factor *= (1.0 + randomObj.GetLong(-100, 100)/400.0);

  UpdatePosition();
  
  m_air_resist_factor = initial_air_resist_factor;
  m_wind_factor = initial_wind_factor;

  if (IsGhost())
  {
    int x, y;
    if (GetY() < (int)world.GetHeight()) {
      x = GetX();
      y = GetY();
      if (x < 0)
        x = world.GetWidth()-1;
      else
        x = -GetWidth()+1;
    } else {
      x = randomObj.GetLong(0, world.GetWidth()-1);
      y = -GetHeight()+1;
    }
    Ready();
    SetXY( Point2i(x, y) );
  }
}

void WindParticle::Draw()
{
  if(TerrainActif().wind.need_flip){
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
  barre.InitPos (10, 10, BARRE_LARG, BARRE_HAUT);
  barre.InitVal (0, -100, 100);
  barre.border_color = c_white;
  barre.background_color = c_black;
  barre.value_color = c_red;
  barre.AjouteMarqueur (100, c_white);
  barre.SetReferenceValue (true, 0);
}

void Wind::Reset(){
  m_last_move = 0;
  m_last_part_mvt = 0;
  m_val = m_nv_val = 0;
  barre.Actu (m_val);

  particles.clear();
  
  const uint nb = TerrainActif().wind.nb_sprite;

  for (uint i=0; i<nb; ++i){
    WindParticle particle;
    particle.Resize( (double)i / nb );
    particles.push_back( particle );
  }
}

double Wind::GetStrength() const{
  return m_nv_val * force / 100.0;
}

void Wind::ChooseRandomVal(){
  int val = randomObj.GetLong(-100, 100);
  ActionHandler::GetInstance()->NewAction (ActionInt(ACTION_WIND, val));
}

void Wind::SetVal(long val){
  m_nv_val = val;
}

void Wind::DrawParticles(){
  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it) it -> Draw();
}

void Wind::Refresh(){
  if(m_last_move + barre_speed < Time::GetInstance()->Read()){
    if(m_val>m_nv_val)
      --m_val;
    else
    if(m_val<m_nv_val)
      ++m_val;
    m_last_move = Time::GetInstance()->Read();
    barre.Actu(m_val); 
  }

  iterator it=particles.begin(), end=particles.end();
  for (; it != end; ++it) it -> Refresh();
}

void Wind::Draw(){
  barre.Draw();
}

