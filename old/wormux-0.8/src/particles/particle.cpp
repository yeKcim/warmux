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
 * Particle Engine
 *****************************************************************************/

#include "particles/particle.h"
#include <SDL.h>
#include <map>
#include "game/time.h"
#include "graphic/sprite.h"
#include "object/objects_list.h"
#include "tool/resource_manager.h"
#include "tool/random.h"
#include "tool/point.h"
#include "weapon/explosion.h"
#include "map/map.h"

#include "particles/body_member.h"
#include "particles/teleport_member.h"
#include "particles/bullet.h"
#include "particles/dark_smoke.h"
#include "particles/explosion_smoke.h"
#include "particles/fire.h"
#include "particles/fading_text.h"
#include "particles/ground_particles.h"
#include "particles/ill_bubble.h"
#include "particles/magic_star.h"
#include "particles/polecat_fart.h"
#include "particles/smoke.h"
#include "particles/star.h"
#include "particles/water_drop.h"

Particle::Particle(const std::string &name) :
  PhysicalObj(name),
  on_top(true), // if true displayed on top of characters and weapons
  m_initial_time_to_live(20),
  m_left_time_to_live(0),
  m_check_move_on_end_turn(false),
  m_time_between_scale(0),
  m_last_refresh(Time::GetInstance()->Read()),
  image(NULL)
{
  SetCollisionModel(true, false, false);
}

Particle::~Particle()
{
  delete image;
}

void Particle::Draw()
{
  if (m_left_time_to_live > 0) {
    image->Draw(GetPosition());
  }
}

void Particle::Refresh()
{
  uint time = Time::GetInstance()->Read() - m_last_refresh;

  UpdatePosition();

  image->Update();

  if (time >= m_time_between_scale) {

    //ASSERT(m_left_time_to_live > 0);
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

bool Particle::StillUseful() const
{
  return (m_left_time_to_live > 0);
}

// ==============================================

ParticleEngine::ParticleEngine(uint time):
  m_last_refresh(Time::GetInstance()->Read()),
  m_time_between_add(time)
{}


void ParticleEngine::AddPeriodic(const Point2i &position, particle_t type,
                                 bool upper,
                                 double angle, double norme)
{
  // time spent since last refresh (in milliseconds)
  uint time = Time::GetInstance()->Read() - m_last_refresh;
  uint tmp = Time::GetInstance()->Read();

  uint delta = uint(m_time_between_add * double(randomObj.GetLong(3, 40)) / 10);
  if (time >= delta) {
    m_last_refresh = tmp;
    ParticleEngine::AddNow(position, 1, type, upper, angle, norme);
  }
}

//-----------------------------------------------------------------------------
// Static methods
bool ParticleEngine::sprites_loaded = false;
std::list<Particle*> ParticleEngine::lst_particles;
Sprite* ParticleEngine::particle_sprite[particle_spr_nbr];

void ParticleEngine::Load()
{
  // Pre-load the sprite of each particle
  Profile *res = resource_manager.LoadXMLProfile( "weapons.xml", false);
  particle_sprite[SMOKE_spr] = resource_manager.LoadSprite(res,"smoke");
  particle_sprite[EXPLOSION_SMOKE_spr] = resource_manager.LoadSprite(res,"smoke_explosion");
  particle_sprite[ILL_BUBBLE_spr] = resource_manager.LoadSprite(res,"ill_bubble");
  particle_sprite[FIRE_spr]  = resource_manager.LoadSprite(res,"fire_particle");
  particle_sprite[STAR_spr]  = resource_manager.LoadSprite(res,"star_particle");
  particle_sprite[DARK_SMOKE_spr]  = resource_manager.LoadSprite(res,"dark_smoke");
  particle_sprite[MAGIC_STAR_R_spr] = resource_manager.LoadSprite(res,"pink_star_particle");
  particle_sprite[MAGIC_STAR_R_spr]->EnableRotationCache(32);
  particle_sprite[MAGIC_STAR_Y_spr] = resource_manager.LoadSprite(res,"yellow_star_particle");
  particle_sprite[MAGIC_STAR_Y_spr]->EnableRotationCache(32);
  particle_sprite[MAGIC_STAR_B_spr] = resource_manager.LoadSprite(res,"blue_star_particle");
  particle_sprite[MAGIC_STAR_B_spr]->EnableRotationCache(32);
  particle_sprite[BULLET_spr] = resource_manager.LoadSprite(res,"bullet_particle");
  particle_sprite[BULLET_spr]->EnableRotationCache(6);
  particle_sprite[POLECAT_FART_spr] = resource_manager.LoadSprite(res,"polecat_fart");
  particle_sprite[WATER_spr] = resource_manager.LoadSprite(res,"water_drop");
  particle_sprite[LAVA_spr] = resource_manager.LoadSprite(res,"lava_drop");
  resource_manager.UnLoadXMLProfile(res);

  sprites_loaded = true;
}

void ParticleEngine::FreeMem()
{
  sprites_loaded = false;

  for(int i=0; i<particle_spr_nbr ; i++)
    delete particle_sprite[i];
}

Sprite* ParticleEngine::GetSprite(particle_spr type)
{
  ASSERT(type < particle_spr_nbr);
  if (!sprites_loaded)
    return NULL;

  return new Sprite(*(particle_sprite[type]));
}

void ParticleEngine::AddNow(const Point2i &position,
                            uint nb_particles, particle_t type,
                            bool upper, double angle, double norme)
{
  if (!sprites_loaded)
    return;

  Particle *particle = NULL;
  double tmp_angle, tmp_norme;

  for (uint i=0 ; i < nb_particles ; i++) {
    switch (type) {
    case particle_SMOKE : particle = new Smoke();
      break;
    case particle_ILL_BUBBLE : particle = new IllBubble();
      break;
    case particle_DARK_SMOKE : particle = new DarkSmoke();
      break;
    case particle_FIRE : particle = new FireParticle();
      break;
    case particle_STAR : particle = new StarParticle();
      break;
    case particle_BULLET : particle = new BulletParticle();
      break;
    case particle_POLECAT_FART : particle = new PolecatFart();
      break;
    case particle_GROUND : particle = new GroundParticle(Point2i(10,10), position);
      break;
    case particle_AIR_HAMMER : particle = new GroundParticle(Point2i(21,18), position);
      // Half the size of the airhammer impact
      // Dirty, but we have no way to read the
      // impact size from here ...
      break;
    case particle_MAGIC_STAR : particle = new MagicStarParticle();
      break;
    case particle_WATER : particle = new WaterParticle();
      break;
    case particle_LAVA: particle = new LavaParticle();
      break;
    default : particle = NULL;
      ASSERT(0);
      break;
    }

    if (particle != NULL) {

      if( norme == -1 )
        tmp_norme = double(randomObj.GetLong(0, 5000))/100;
      else
        tmp_norme = norme;

      if( angle == -1 )
        tmp_angle = - double(randomObj.GetLong(0, 3000))/1000;
      else
        tmp_angle = angle;

      particle->SetXY(position);
      particle->SetOnTop(upper);
      particle->SetSpeed(tmp_norme, tmp_angle);
      lst_particles.push_back(particle);
    }
  }
}

void ParticleEngine::AddNow(Particle* particle)
{
  if (!sprites_loaded || !particle)
    return;

  lst_particles.push_back(particle);
}

void ParticleEngine::AddBigESmoke(const Point2i &position, const uint &radius)
{
  if (!sprites_loaded)
    return;

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
  if (!sprites_loaded)
    return;

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

      Point2i pos = position; //Set position to center of explosion
      pos = pos - size / 2;       //Set the center of the smoke to the center..
      pos = pos + Point2i(int(norme * big_cos[i]),int(norme * big_sin[i])); //Put inside the circle of the explosion

      particle = new ExplosionSmoke(size);
      particle->SetXY(pos);
      particle->SetOnTop(true);

      lst_particles.push_back(particle);
  }
}

void ParticleEngine::AddExplosionSmoke(const Point2i &position, const uint &radius, const ESmokeStyle &style)
{
  if (!sprites_loaded)
    return;

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

PhysicalObj * ParticleEngine::IsSomethingMoving()
{
  std::list<Particle *>::iterator Particle_it;
  // check if particle need to be check in end of turn
  for (Particle_it=lst_particles.begin(); Particle_it!=lst_particles.end(); ++Particle_it)
    if ((*Particle_it)->CheckOnEndTurn() && (*Particle_it)->StillUseful())
      return *Particle_it;
  return NULL;
}
