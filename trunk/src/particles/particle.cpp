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
 * Particle Engine
 *****************************************************************************/

#include <WARMUX_point.h>
#include <SDL.h>
#include <map>
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "map/map.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"

#include "particles/particle.h"
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
#include "particles/explosion_particle.h"

Particle::Particle(const std::string &name) :
  PhysicalObj(name),
  on_top(true), // if true displayed on top of characters and weapons
  m_initial_time_to_live(20),
  m_time_left_to_live(0),
  m_check_move_on_end_turn(false),
  m_time_between_scale(0),
  m_last_refresh(GameTime::GetInstance()->Read()),
  image(NULL)
{
  SetCollisionModel(false, false, false);
}

Particle::~Particle()
{
  delete image;
}

void Particle::Draw()
{
  if (m_time_left_to_live > 0) {
    image->Draw(GetPosition());
  }
}

void Particle::Refresh()
{
  uint time = GameTime::GetInstance()->Read() - m_last_refresh;

  UpdatePosition();

  image->Update();

  if (time >= m_time_between_scale) {

    //ASSERT(m_time_left_to_live > 0);
    if (m_time_left_to_live <= 0) return ;

    m_time_left_to_live--;

    uint lived_time = m_initial_time_to_live - m_time_left_to_live;

    //during the 1st half of the time, increase size of particle
    //after the 1st half, decrease the alpha value
    if (2*lived_time<m_initial_time_to_live) {
      Double coeff = sin(lived_time*PI/m_initial_time_to_live);
      image->Scale(coeff, coeff);
      SetSize(image->GetSize());
      image->SetAlpha(1.0);
    } else {
      Double alpha = ONE - sin((2*lived_time-m_initial_time_to_live)*HALF_PI/m_initial_time_to_live);
      image->Scale(1.0, 1.0);
      image->SetAlpha(alpha);
    }
    m_last_refresh = GameTime::GetInstance()->Read() ;
  }
}

// ==============================================

ParticleEngine::ParticleEngine(uint time):
  m_last_refresh(GameTime::GetInstance()->Read()),
  m_time_between_add(time)
{}

void ParticleEngine::AddPeriodic(const Point2i &position, particle_t type,
                                 bool upper,
                                 Double angle, Double norme)
{
  // time spent since last refresh (in milliseconds)
  uint time = GameTime::GetInstance()->Read() - m_last_refresh;
  uint tmp = GameTime::GetInstance()->Read();

  MSG_DEBUG("random.get", "ParticleEngine::AddPeriodic(...)");
  uint delta = uint(m_time_between_add * Double(RandomSync().GetUint(3, 40)) / 10);
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
  if (sprites_loaded)
    return;

  // Pre-load the sprite of each particle
  Profile *res = GetResourceManager().LoadXMLProfile("weapons.xml", false);
  particle_sprite[SMOKE_spr] = LOAD_RES_SPRITE("smoke");
  particle_sprite[EXPLOSION_SMOKE_spr] = LOAD_RES_SPRITE("smoke_explosion");
  particle_sprite[EXPLOSION_BIG_SMOKE_spr] = LOAD_RES_SPRITE("smoke_big_explosion");
  particle_sprite[ILL_BUBBLE_spr] = LOAD_RES_SPRITE("ill_bubble");
  particle_sprite[FIRE_spr]  = LOAD_RES_SPRITE("fire_particle");
  particle_sprite[STAR_spr]  = LOAD_RES_SPRITE("star_particle");
  particle_sprite[DARK_SMOKE_spr]  = LOAD_RES_SPRITE("dark_smoke");
  particle_sprite[MAGIC_STAR_R_spr] = LOAD_RES_SPRITE("pink_star_particle");
  particle_sprite[MAGIC_STAR_R_spr]->EnableCaches(false, 32);
  particle_sprite[MAGIC_STAR_Y_spr] = LOAD_RES_SPRITE("yellow_star_particle");
  particle_sprite[MAGIC_STAR_Y_spr]->EnableCaches(false, 32);
  particle_sprite[MAGIC_STAR_B_spr] = LOAD_RES_SPRITE("blue_star_particle");
  particle_sprite[MAGIC_STAR_B_spr]->EnableCaches(false, 32);
  particle_sprite[BULLET_spr] = LOAD_RES_SPRITE("bullet_particle");
  particle_sprite[BULLET_spr]->EnableCaches(false, 6);
  particle_sprite[POLECAT_FART_spr] = LOAD_RES_SPRITE("polecat_fart");
  particle_sprite[CLEARWATER_spr] = LOAD_RES_SPRITE("water_drop");
  particle_sprite[LAVA_spr] = LOAD_RES_SPRITE("lava_drop");
  particle_sprite[RADIOACTIVE_spr] = LOAD_RES_SPRITE("radioactive_drop");
  particle_sprite[DIRTYWATER_spr] = LOAD_RES_SPRITE("dirtywater_drop");
  particle_sprite[CHOCOLATEWATER_spr] = LOAD_RES_SPRITE("chocolate_drop");
  particle_sprite[EXPLOSION_spr] = LOAD_RES_SPRITE("explosion_particle");

  sprites_loaded = true;
}

void ParticleEngine::FreeMem()
{
  // Only called at game exit: loading particles is long and costly
  // and it would be a waste to discard the cache already available
  sprites_loaded = false;

  for (int i=0; i<particle_spr_nbr ; i++)
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
                            bool upper, Double angle, Double norme)
{
  if (!sprites_loaded)
    return;

  Particle *particle = NULL;
  Double tmp_angle, tmp_norme;

  for (uint i=0; i<nb_particles; i++) {
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
    case particle_CLEARWATER : particle = new WaterParticle(CLEARWATER_spr);
      break;
    case particle_LAVA: particle = new WaterParticle(LAVA_spr);
      break;
    case particle_RADIOACTIVE: particle = new WaterParticle(RADIOACTIVE_spr);
      break;
    case particle_DIRTYWATER: particle = new WaterParticle(DIRTYWATER_spr);
      break;
    case particle_CHOCOLATEWATER: particle = new WaterParticle(RADIOACTIVE_spr);
      break;
    case particle_EXPLOSION: particle = new ExplosionParticle();
    case particle_BODY_MEMBER:
    case particle_TELEPORT_MEMBER:
      break;
    }

    if (particle) {

      if (norme == -1) {
        MSG_DEBUG("random.get", "ParticleEngine::AddNow(...) speed vector length");
        tmp_norme = RandomSync().GetDouble(0, 5000/100);
      } else {
        tmp_norme = norme;
      }

      if (angle == -1) {
        MSG_DEBUG("random.get", "ParticleEngine::AddNow(...) speed vector angle");
        tmp_angle = RandomSync().GetDouble(-3000/1000, 0);
      } else {
        tmp_angle = angle;
      }

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
  const uint little_partic_nbr = 1;
  static const float little_cos[] = { 1.000000f,  0.809017f,  0.309017f, -0.309017f, -0.809017f,
                                     -1.000000f, -0.809017f, -0.309017f,  0.309017f,  0.809017f };
  static const float little_sin[] = { 0.000000f,  0.587785f,  0.951057f,  0.951056f,  0.587785f,
                                     -0.000000f, -0.587785f, -0.951056f, -0.951056f, -0.587785f };

  Particle *particle = NULL;

  for (uint i=0; i<little_partic_nbr; i++) {
//      angle = (Double) i * PI * 2.0 / (Double) little_partic_nbr;
    uint size = radius *2;
    float norme = 2.5f * radius / 3.0f;

    particle = new ExplosionSmoke(size);
    particle->SetOnTop(true);

    Point2i pos = position; //Set position to center of explosion
    pos = pos - size / 2;       //Set the center of the smoke to the center..
    //Put inside the circle of the explosion
    pos = pos + Point2i(norme * little_cos[i], norme * little_sin[i]);

    particle->SetXY(pos);
    lst_particles.push_back(particle);
  }
}

void ParticleEngine::AddLittleESmoke(const Point2i &position, const uint &radius)
{
  if (!sprites_loaded)
    return;

  //Add a few big smoke particles
  const uint big_partic_nbr = 1;
  // Sin / cos  precomputed value, to avoid recomputing them and speed up.
  // see the commented value of 'angle' to see how it was generated
  static const float big_cos[] = { 1.000000f, -0.809017f,  0.309017f, 0.309017f, -0.809017f };
  static const float big_sin[] = { 0.000000f,  0.587785f, -0.951056f, 0.951057f, -0.587785f };

  Particle *particle = NULL;
  for (uint i=0; i<big_partic_nbr; i++) {
//      angle = (Double) i * PI * 4.0 / (Double)big_partic_nbr;
    uint size = radius;
    float norme = radius / 3.0f;

    Point2i pos = position; //Set position to center of explosion
    pos = pos - size / 2;       //Set the center of the smoke to the center..
    pos = pos + Point2i(norme * big_cos[i], norme * big_sin[i]); //Put inside the circle of the explosion

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

  if (style == NoESmoke)
    return;

  if (style == BigESmoke) {
    AddBigESmoke(position, radius);
  }else{
    AddLittleESmoke(position, radius);
  }
}

void ParticleEngine::Draw(bool upper)
{
  std::list<Particle *>::iterator Particle_it;
  // draw the particles
  for (Particle_it=lst_particles.begin(); Particle_it!=lst_particles.end(); ++Particle_it)
    if ((*Particle_it)->IsOnTop() == upper)
      (*Particle_it)->Draw();
}

void ParticleEngine::Refresh()
{
  // remove old particles
  std::list<Particle*>::iterator it=lst_particles.begin(), end=lst_particles.end();
  while (it != end) {
    if (! (*it)->StillUseful()) {
      delete *it;
      it = lst_particles.erase(it);
    } else {
      it++;
    }
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
