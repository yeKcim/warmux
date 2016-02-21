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
//----------------------------------------------------------------------------
#include "../include/action_handler.h"
#include "../tool/random.h"
#include "../map/map.h"
#include "../map/maps_list.h"
#include "../game/time.h"
#include "../tool/xml_document.h"
#include "camera.h"
#include <SDL.h>
#include "../tool/resource_manager.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#include "camera.h"
//----------------------------------------------------------------------------

const uint MAX_WIND_OBJECTS = 200;

const uint BARRE_LARG = 80;
const uint BARRE_HAUT = 10;
const double force = 5; // Force maximale du vent en m/(sec*sec)
const uint barre_speed = 20;

//----------------------------------------------------------------------------
namespace Wormux {
Wind wind;
//-----------------------------------------------------------------------------

WindParticle::WindParticle() : PhysicalObj("WindParticle", 0.0)
{
  m_type = objUNBREAKABLE;
  m_wind_factor = 1;
  m_air_resist_factor = 0;
  sprite = NULL;
}

//-----------------------------------------------------------------------------

void WindParticle::Init()
{
  double mass, wind_factor ;

  if(sprite != NULL)
    delete sprite;

  sprite = resource_manager.LoadSprite( TerrainActif().res_profile, "wind_particle");
  if(sprite->GetFrameCount()==1)
    sprite->EnableLastFrameCache();
  sprite->SetCurrentFrame ( RandomLong(0, sprite->GetFrameCount()-1));
   
  SetXY(RandomLong(0, world.GetWidth()-1), RandomLong(0, world.GetHeight()-1));

  //Mass = mass_mean + or - 25%
  mass = TerrainActif().wind.particle_mass;
  mass *= (1.0 + RandomLong(-100, 100)/400.0);
  SetMass (mass);
  SetSize( sprite->GetWidth(), sprite->GetHeight());
  wind_factor = TerrainActif().wind.particle_wind_factor ;
  wind_factor *= (1.0 + RandomLong(-100, 100)/400.0);  
  SetWindFactor(wind_factor);
  StartMoving();

  // Fixe le rectangle de test
  int dx = 0 ;
  int dy = 0 ;
  SetTestRect (dx, dx, dy, dy);

  m_allow_negative_y = true;
}

//-----------------------------------------------------------------------------

void WindParticle::Reset()
{ 
}

//-----------------------------------------------------------------------------

void WindParticle::Refresh()
{
  sprite->Update();
  UpdatePosition();
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
      x = RandomLong(0, world.GetWidth()-1);
      y = -GetHeight()+1;
    }
    Ready();
    SetXY(x, y);
  }
}

//-----------------------------------------------------------------------------

void WindParticle::Draw()
{
  if(TerrainActif().wind.need_flip)
  {
    DoubleVector speed;
    GetSpeedXY(speed);
    float scale_x,scale_y;
    sprite->GetScaleFactors( scale_x, scale_y);
    if((speed.x<0 && scale_x>0)
    || (speed.x>0 && scale_x<0))
    {
      scale_x=-scale_x;
      sprite->Scale( scale_x, scale_y);
    }
  }
  sprite->Draw(GetX(), GetY());
}

//-----------------------------------------------------------------------------

void WindParticle::Resize(double size)
{
  size=0.5+size/2.0;
  sprite->Scale( size,size);
  sprite->SetAlpha( size);
}

//-----------------------------------------------------------------------------


   SDL_Color c_white  = {0xFF, 0xFF, 0xFF, 0x70};
   SDL_Color c_black  = {0x00, 0x00, 0x00, 0x70};
   SDL_Color c_red    = {0xFF, 0x00, 0x00, 0x70};
   SDL_Color c_yellow = {0x00, 0xFF, 0xFF, 0x70};
   SDL_Color c_grey   = {0xF0, 0xF0, 0xF0, 0x70};
   
Wind::Wind()
{
  m_val = m_nv_val = 0;
  barre.InitPos (10, 10, BARRE_LARG, BARRE_HAUT);
  barre.InitVal (0, -100, 100);
  barre.border_color = c_white;
  barre.background_color = c_black;
  barre.value_color = c_red;
  barre.AjouteMarqueur (100, c_white);
  barre.SetReferenceValue (true, 0);

  wind_particle_array = new WindParticle[MAX_WIND_OBJECTS];
}

//-----------------------------------------------------------------------------

Wind::~Wind()
{
  delete []wind_particle_array;
}

//-----------------------------------------------------------------------------

void Wind::Init()
{
}

//-----------------------------------------------------------------------------

void Wind::Reset()
{
  m_last_move = 0;
  m_last_part_mvt = 0;
  m_val = m_nv_val = 0;
  barre.Actu (m_val);

  for(uint i=0;i<TerrainActif().wind.nb_sprite; i++)
  {
    wind_particle_array[i].Init();
    wind_particle_array[i].Resize((double)i/(double)TerrainActif().wind.nb_sprite);
  }
}

//-----------------------------------------------------------------------------

double Wind::GetStrength() const
{
  return m_nv_val*force/100.0;
}

//-----------------------------------------------------------------------------

void Wind::ChooseRandomVal()
{
  int val = RandomLong(-100, 100);
  action_handler.NewAction (ActionInt(ACTION_WIND, val));
}

//-----------------------------------------------------------------------------

void Wind::SetVal(long val)
{
  m_nv_val = val;
}

//-----------------------------------------------------------------------------

void Wind::DrawParticles()
{
  //  TerrainActif().wind.nbr_sprite = 1 ;

  for(uint i=0;i<TerrainActif().wind.nb_sprite; i++)
    wind_particle_array[i].Draw();
}

//-----------------------------------------------------------------------------

void Wind::Refresh()
{
  if(m_last_move + barre_speed < global_time.Read())
  {
    if(m_val>m_nv_val)
      --m_val;
    else
    if(m_val<m_nv_val)
      ++m_val;
    m_last_move = global_time.Read();
    barre.Actu(m_val); 
  }

  for(uint i=0;i<TerrainActif().wind.nb_sprite; i++)
    {
      wind_particle_array[i].Refresh();
    }
}

//-----------------------------------------------------------------------------

void Wind::Draw()
{
  barre.Draw();
}

//-----------------------------------------------------------------------------
} //namespace Wormux
//-----------------------------------------------------------------------------
