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
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#include "../object/objects_list.h"
#include "../tool/resource_manager.h"
#include "../tool/random.h"
#include "../tool/point.h"
#include "../weapon/weapon_tools.h"

ParticleEngine global_particle_engine;

Particle::Particle(const std::string &name) :
  PhysicalObj(name)
{
  m_type = objUNBREAKABLE;
  m_initial_time_to_live = 20;
  m_left_time_to_live = 0;
  m_last_refresh = Time::GetInstance()->Read();
}

Particle::~Particle()
{
  delete image;
}

void Particle::Draw()
{
  if (m_left_time_to_live > 0)
    image->Draw(GetPosition());
}

void Particle::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;

  UpdatePosition();

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
      SetSize(image->GetSize());
      image->SetAlpha(1.0);
    }
    else
    {
      float alpha = 1.0 - sin((M_PI/2.0)*((float)lived_time-((float)m_initial_time_to_live/2.0))/((float)m_initial_time_to_live/2.0));
      image->Scale(1.0,1.0);
      image->SetAlpha(alpha);
    }
    m_last_refresh = Time::GetInstance()->Read() ;
  }
}

bool Particle::StillUseful()
{
  return (m_left_time_to_live > 0);
}

Smoke::Smoke() :
  Particle("smoke_particle")
{
  m_initial_time_to_live = 10;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 100;
}

void Smoke::Init()
{
  m_initial_time_to_live = 10;
  m_left_time_to_live = m_initial_time_to_live;
  image = ParticleEngine::GetSprite(SMOKE_spr);
  image->Scale(0.0,0.0);
  SetSize( Point2i(1, 1) );
}

ExplosionSmoke::ExplosionSmoke(const uint size_init) :
  Particle("explosion_smoke_particle")
{
  m_initial_size = size_init;
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 25;
  dx = 0;
}

void ExplosionSmoke::Init()
{
  image = ParticleEngine::GetSprite(EXPLOSION_SMOKE_spr);
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  mvt_freq = randomObj.GetDouble(-2.0, 2.0);
  SetGravityFactor(randomObj.GetDouble(-1.0,-2.0));

  image->ScaleSize(m_initial_size, m_initial_size);
  SetSize( Point2i(m_initial_size, m_initial_size) );
  StartMoving();
}

void ExplosionSmoke::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;

  UpdatePosition();

  image->Update();

  if (time >= m_time_between_scale) {
    //assert(m_left_time_to_live > 0);
    if (m_left_time_to_live <= 0) return ;

    m_left_time_to_live--;

    float lived_time = m_initial_time_to_live - m_left_time_to_live;

    float coeff = cos((M_PI/2.0)*((float)lived_time/((float)m_initial_time_to_live)));
    image->ScaleSize(int(coeff * m_initial_size),int(coeff * m_initial_size));

    dx = int((int)m_initial_size * sin(5.0 * ((float)lived_time/((float)m_initial_time_to_live)) * M_PI * mvt_freq / 2.0) / 2);
    m_last_refresh = Time::GetInstance()->Read() ;
  }
}

void ExplosionSmoke::Draw()
{
  if (m_left_time_to_live > 0)
    image->Draw(GetPosition()+Point2i(dx,0));
}

StarParticle::StarParticle() :
  Particle("star_particle")
{
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 50;
}

void StarParticle::Init()
{
  image = ParticleEngine::GetSprite(STAR_spr);
  image->Scale(0.0, 0.0);
  SetSize( Point2i(1, 1) );
}

MagicStarParticle::MagicStarParticle() :
  Particle("magic_star_particle")
{
  m_initial_time_to_live = 30;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 25;
}

void MagicStarParticle::Init()
{
  uint color=randomObj.GetLong(0,2);
  switch(color)
  {
    case 0 : image = ParticleEngine::GetSprite(MAGIC_STAR_R_spr); break;
    case 1 : image = ParticleEngine::GetSprite(MAGIC_STAR_Y_spr); break;
    case 2 : image = ParticleEngine::GetSprite(MAGIC_STAR_B_spr); break;
    default: assert(false);
  }
  image->Scale(0.0, 0.0);
  SetSize( Point2i(1, 1) );
}

void MagicStarParticle::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;
  if (time >= m_time_between_scale) {
    if (m_left_time_to_live <= 0) return ;
    float lived_time = m_initial_time_to_live - m_left_time_to_live;
    float coeff = sin((M_PI/2.0)*((float)lived_time/((float)m_initial_time_to_live)));
    image->SetRotation_deg(coeff * 360.0);
  }
  Particle::Refresh();
}

ExplosiveWeaponConfig fire_cfg;

FireParticle::FireParticle() :
  Particle("fire_particle")
{
  m_type = objCLASSIC;
  m_initial_time_to_live = 15;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 50;
  fire_cfg.damage = 1;
  fire_cfg.explosion_range = 5;
}

void FireParticle::Init()
{
  image = ParticleEngine::GetSprite(FIRE_spr);
  image->Scale(0.0,0.0);
  SetSize( Point2i(1, 1) );
}

void FireParticle::SignalFallEnding()
{
  Point2i pos = GetCenter();
  ApplyExplosion (pos, fire_cfg, NULL, "", false, ParticleEngine::NoESmoke);

  m_left_time_to_live = 0;
}

//-----------------------------------------------------------------------------

ParticleEngine::ParticleEngine(uint time)
{
  m_time_between_add = time ;
}


void ParticleEngine::AddPeriodic(const Point2i &position, particle_t type,
				 bool upper,
				 double angle, double norme)
{
  // time spent since last refresh (in milliseconds)
  uint time = Time::GetInstance()->Read() - m_last_refresh;
  uint tmp = Time::GetInstance()->Read();

  uint delta = uint(m_time_between_add * double(randomObj.GetLong(3,40))/10);
  if (time >= delta) {
    m_last_refresh = tmp;
    ParticleEngine::AddNow(position, 1, type, upper, angle, norme);
  }
}

//-----------------------------------------------------------------------------
// Static methods

std::list<Particle*> ParticleEngine::lst_particles;
Sprite* ParticleEngine::particle_sprite[particle_spr_nbr];

void ParticleEngine::Init()
{
  // Pre-load the sprite of each particle
  Profile *res = resource_manager.LoadXMLProfile( "weapons.xml", false);
  particle_sprite[SMOKE_spr] = resource_manager.LoadSprite(res,"smoke");
  particle_sprite[EXPLOSION_SMOKE_spr] = resource_manager.LoadSprite(res,"smoke_explosion");
  particle_sprite[FIRE_spr]  = resource_manager.LoadSprite(res,"fire_particle");
  particle_sprite[STAR_spr]  = resource_manager.LoadSprite(res,"star_particle");
  particle_sprite[MAGIC_STAR_R_spr] = resource_manager.LoadSprite(res,"pink_star_particle");
  particle_sprite[MAGIC_STAR_R_spr]->EnableRotationCache(32);
  particle_sprite[MAGIC_STAR_Y_spr] = resource_manager.LoadSprite(res,"yellow_star_particle");
  particle_sprite[MAGIC_STAR_Y_spr]->EnableRotationCache(32);
  particle_sprite[MAGIC_STAR_B_spr] = resource_manager.LoadSprite(res,"blue_star_particle");
  particle_sprite[MAGIC_STAR_B_spr]->EnableRotationCache(32);
  resource_manager.UnLoadXMLProfile( res);
}

void ParticleEngine::FreeMem()
{
  for(int i=0; i<particle_spr_nbr ; i++)
    delete global_particle_engine.particle_sprite[i];
}

Sprite* ParticleEngine::GetSprite(particle_spr type)
{
  assert(type < particle_spr_nbr);
  return new Sprite(*(global_particle_engine.particle_sprite[type]));
}

void ParticleEngine::AddNow(const Point2i &position,
			    uint nb_particles, particle_t type,
			    bool upper,
			    double angle, double norme)
{
  Particle *particle = NULL;
  double tmp_angle, tmp_norme;

  for (uint i=0 ; i < nb_particles ; i++) {
    switch (type) {
      case particle_SMOKE : particle = new Smoke();
                            break;
      case particle_FIRE : particle = new FireParticle();
                           break;
      case particle_STAR : particle = new StarParticle();
                           break;
      case particle_MAGIC_STAR : particle = new MagicStarParticle();
                                 break;
      default : particle = NULL;
                assert(0);
                break;
    }

    if (particle != NULL) {
      if( norme == -1 )
		  tmp_norme = double(randomObj.GetLong(0, 5000))/1000;
      else
		  tmp_norme = norme;

      if( angle == -1 )
		  tmp_angle = - double(randomObj.GetLong(0, 3000))/1000;
      else
		  tmp_angle = angle;

      particle->Init();
      particle->SetOnTop(upper);
      particle->SetXY(position);
      particle->SetSpeed(tmp_norme, tmp_angle);
      lst_particles.push_back(particle);
    }
  }
}

void ParticleEngine::AddBigESmoke(const Point2i &position, const uint &radius)
{
  //Add many little smoke particles
  // Sin / cos  precomputed value, to avoid recomputing them and speed up.
  // see the commented value of 'angle' to see how it was generated
  const uint little_partic_nbr = 10;
  const float little_cos[] = { 1.000000, 0.809017, 0.309017, -0.309017, -0.809017, -1.000000, -0.809017, -0.309017, 0.309017, 0.809017 };
  const float little_sin[] = { 0.000000, 0.587785, 0.951057, 0.951056, 0.587785, -0.000000, -0.587785, -0.951056, -0.951056, -0.587785 };

  Particle *particle = NULL;
  float norme;
  uint size;

  for(uint i=0; i < little_partic_nbr ; i++)
  {
//      angle = (float) i * M_PI * 2.0 / (float) little_partic_nbr;
      size = uint(radius / 1.5);
      norme = 2.5 * radius / 3.0;

      particle = new ExplosionSmoke(size);
      particle->Init();
      particle->SetOnTop(true);

      Point2i pos = position; //Set position to center of explosion
      pos = pos - size / 2;       //Set the center of the smoke to the center..
      pos = pos + Point2i(int(norme * little_cos[i]),int(norme * little_sin[i])); //Put inside the circle of the explosion

      particle->SetXY(pos);
      lst_particles.push_back(particle);
  }
}

void ParticleEngine::AddLittleESmoke(const Point2i &position, const uint &radius)
{
  //Add a few big smoke particles
  const uint big_partic_nbr = 5;
  // Sin / cos  precomputed value, to avoid recomputing them and speed up.
  // see the commented value of 'angle' to see how it was generated
  const float big_cos[] = { 1.000000, -0.809017, 0.309017, 0.309017, -0.809017 };
  const float big_sin[] = { 0.000000, 0.587785, -0.951056, 0.951057, -0.587785 };

  Particle *particle = NULL;
  float norme;
  uint size;
  for(uint i=0; i < big_partic_nbr ; i++)
  {
//      angle = (float) i * M_PI * 4.0 / (float)big_partic_nbr;
      size = radius;
      norme = radius / 3.0;
      particle = new ExplosionSmoke(size);
      particle->Init();
      particle->SetOnTop(true);

      Point2i pos = position; //Set position to center of explosion
      pos = pos - size / 2;       //Set the center of the smoke to the center..
      pos = pos + Point2i(int(norme * big_cos[i]),int(norme * big_sin[i])); //Put inside the circle of the explosion

      particle->SetXY(pos);
      lst_particles.push_back(particle);
  }
}

void ParticleEngine::AddExplosionSmoke(const Point2i &position, const uint &radius, ESmokeStyle &style)
{
  if(style == NoESmoke) return;
  AddLittleESmoke (position, radius);
  if(style == BigESmoke) AddBigESmoke (position, radius);
}

void ParticleEngine::Draw(bool upper)
{
  std::list<Particle *>::iterator Particle_it;
  // draw the particles
  for (Particle_it=lst_particles.begin(); Particle_it!=lst_particles.end(); ++Particle_it){
    if ( (*Particle_it)->IsOnTop() == upper) {
      (*Particle_it)->Draw();
    }
  }

}

void ParticleEngine::Refresh()
{
  // remove old particles
  std::list<Particle*>::iterator it=lst_particles.begin(), end=lst_particles.end();
  while (it != end) {
    if (! (*it)->StillUseful()) {
      delete *it;
      it = lst_particles.erase(it);
    }
    else
      it++;
  }

  // update the particles
  for(it=lst_particles.begin(); it!=lst_particles.end(); ++it) {
    (*it)->Refresh();
  }
}

void ParticleEngine::Stop()
{
  // remove all the particles
  std::list<Particle*>::iterator it=lst_particles.begin(), end=lst_particles.end();
  while (it != end) {
    delete *it;
    it = lst_particles.erase(it);
  }
}

