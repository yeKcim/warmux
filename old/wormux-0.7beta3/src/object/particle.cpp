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
 * Particle Engine
 *****************************************************************************/

#include "particle.h"
#include <SDL.h>
#include <map>
#include "../object/objects_list.h"
#include "../game/time.h"
#include "../tool/random.h"
#include "../weapon/weapon_tools.h"
#include "../include/app.h"
#include "../tool/resource_manager.h"
#include "../tool/Point.h"
#include "../graphic/sprite.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ParticleEngine global_particle_engine;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Particle::Particle() : PhysicalObj("Particle", 0.0)
{ 
  m_type = objUNBREAKABLE;
  m_wind_factor = 0.8;
  m_air_resist_factor = 0.2;
  m_rebounding = 0;

  m_initial_time_to_live = 20;
  m_left_time_to_live = 0;
  m_last_refresh = Wormux::global_time.Read();
}

//-----------------------------------------------------------------------------

void Particle::Draw()
{
  if (m_left_time_to_live > 0) 
    image->Draw(GetX(),GetY());
}

//-----------------------------------------------------------------------------

void Particle::Refresh()
{
  uint time = Wormux::global_time.Read() - m_last_refresh; 

  UpdatePosition ();

  image->Update();


  if (time >= m_time_between_scale) {  

    //assert(m_left_time_to_live > 0);
    if (m_left_time_to_live <= 0) return ;

    m_left_time_to_live--;

    float lived_time = m_initial_time_to_live - m_left_time_to_live;

    //during the 1st quarter of the time increase size of particle
    //after the 1st quarter, decrease the alpha value
    if((float)lived_time<m_initial_time_to_live/2.0)
    {
      float coeff = sin((M_PI/2.0)*((float)lived_time/((float)m_initial_time_to_live/2.0)));
      image->Scale(coeff,coeff);  
      SetSize(image->GetWidth(),image->GetHeight());
      image->SetAlpha(1.0);
    }
    else
    {
      float alpha = 1.0 - sin((M_PI/2.0)*((float)lived_time-((float)m_initial_time_to_live/2.0))/((float)m_initial_time_to_live/2.0));
      image->Scale(1.0,1.0);
      image->SetAlpha(alpha);
    }
    m_last_refresh = Wormux::global_time.Read() ;
  }
}
//-----------------------------------------------------------------------------

bool Particle::StillUseful()
{
  return (m_left_time_to_live > 0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Smoke::Smoke() : Particle()
{
  m_name="Smoke";
  SetMass(0.5);
  SetGravityFactor(-1.0);

  m_initial_time_to_live = 10;
  m_left_time_to_live = m_initial_time_to_live; 
  m_time_between_scale = 100;
}

//-----------------------------------------------------------------------------

void Smoke::Init()
{
  Profile *res = resource_manager.LoadXMLProfile( "weapons.xml", false);
  image = resource_manager.LoadSprite(res,"smoke"); 
  resource_manager.UnLoadXMLProfile( res);
   
  m_initial_time_to_live = 10;
  m_left_time_to_live = m_initial_time_to_live; 

  image->Scale(0.0,0.0);
  SetSize(1,1);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

StarParticle::StarParticle() : Particle()
{
  m_name="StarParticle";
  SetMass(0.5);  
  SetGravityFactor(0.0);
  m_wind_factor = 0.2;
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live; 
  m_time_between_scale = 50;
}

//-----------------------------------------------------------------------------

void StarParticle::Init()
{
  Profile *res = resource_manager.LoadXMLProfile( "weapons.xml", false);
  image = resource_manager.LoadSprite(res,"star_particle"); 
  resource_manager.UnLoadXMLProfile( res);

  image->Scale(0.0,0.0);
  SetSize(1,1);  
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ExplosiveWeaponConfig fire_cfg;

FireParticle::FireParticle() : Particle()
{
  m_name="FireParticle";
  SetMass(0.5);
  m_type = objCLASSIC;
  m_rebounding = false;
  m_wind_factor = 0.2;

  m_initial_time_to_live = 15;
  m_left_time_to_live = m_initial_time_to_live; 
  m_time_between_scale = 50;
  fire_cfg.damage = 1;
}

//-----------------------------------------------------------------------------

void FireParticle::Init()
{
  Profile *res = resource_manager.LoadXMLProfile( "weapons.xml", false);
  image = resource_manager.LoadSprite(res,"fire_particle");
  impact = resource_manager.LoadImage(res,"fire_impact");
  resource_manager.UnLoadXMLProfile( res);

  image->Scale(0.0,0.0);
  SetSize(1,1);
}

//-----------------------------------------------------------------------------

void FireParticle::SignalFallEnding()
{
  Point2i pos = GetCenter();
  AppliqueExplosion (pos, pos, impact, fire_cfg, NULL, 
  		     "", false);
  
  m_left_time_to_live = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


ParticleEngine::ParticleEngine()
{
  type_particle = particle_SMOKE ;
  m_time_between_add = 0;
}
//-----------------------------------------------------------------------------


ParticleEngine::ParticleEngine(particle_t type, uint time)
{
  type_particle = type ;
  m_time_between_add = time ;
}

//-----------------------------------------------------------------------------

void ParticleEngine::Refresh()
{
  // remove old particles 
  std::list<Particle *>::iterator it=particles.begin(), end=particles.end(), current=NULL;
  while (it != end) {
    current = it;
    ++it;

    if (! (*current)->StillUseful()) {
      delete (*current);
      particles.erase(current);
      if (it==end) break;
    }   
  }

  // update the particles
  for(it=particles.begin(); it!=particles.end(); ++it) {
    (*it)->Refresh();
  }
}

//-----------------------------------------------------------------------------

void ParticleEngine::AddPeriodic(uint x, uint y, 
				 double angle, double norme)
{
  // time spent since last refresh (in milliseconds)
  uint time = Wormux::global_time.Read() - m_last_refresh; 
  uint tmp = Wormux::global_time.Read();

  uint delta = uint(m_time_between_add * double(RandomLong(3,40))/10);
  if (time >= delta) {
    m_last_refresh = tmp;
    AddNow(x, y, 1, type_particle, angle, norme);
  }
  
  Refresh();
}

//-----------------------------------------------------------------------------

void ParticleEngine::AddNow(uint x, uint y, 
			    uint nb_particles, particle_t type, 
			    double angle, double norme)
{
  Particle * particle = NULL;
  double tmp_angle;
  double tmp_norme;

  for (uint i=0 ; i < nb_particles ; i++) {

    switch (type) {
    case particle_SMOKE : particle = new Smoke();
      break;
    case particle_FIRE : particle = new FireParticle();
      break;
    case particle_STAR : particle = new StarParticle();
      break;
    default : particle = NULL;
      break;
    }
  
    if (particle != NULL) {
      
      if ( norme == -1 ) tmp_norme = double(RandomLong(0, 5000))/1000;
      else tmp_norme = norme;

      if ( angle == -1 ) tmp_angle = - double(RandomLong(0, 3000))/1000; 
      else tmp_angle = angle;
      
      particle->Init();
      particle->SetXY(x,y);
      particle->SetSpeed(tmp_norme, tmp_angle);
      particles.push_back(particle);
    }
  }
}

//-----------------------------------------------------------------------------

void ParticleEngine::Draw()
{
  std::list<Particle *>::iterator it=NULL;
  // draw the particles
  for(it=particles.begin(); it!=particles.end(); ++it) {
    (*it)->Draw();
  }

}

//-----------------------------------------------------------------------------

void ParticleEngine::Stop()
{
  // remove all the particles 
  std::list<Particle *>::iterator it=particles.begin(), end=particles.end(), current=NULL;
  while (it != end) {
    current = it;
    ++it;
    
    delete (*current);
    particles.erase(current);
    if (it==end) break;
    
  }
}

//-----------------------------------------------------------------------------
