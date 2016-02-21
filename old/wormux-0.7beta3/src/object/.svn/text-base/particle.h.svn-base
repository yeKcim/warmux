
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

#ifndef PARTICLE_H
#define PARTICLE_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "physical_obj.h"
//#include "../team/character.h"
#include "../include/base.h"
#include "../weapon/weapon_cfg.h"
#include "../graphic/sprite.h"
//-----------------------------------------------------------------------------

typedef enum { 
  particle_SMOKE,
  particle_FIRE,
  particle_STAR
} particle_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Particle : public PhysicalObj
{
 protected:
  uint m_initial_time_to_live;
  uint m_left_time_to_live;

  uint m_time_between_scale;
  uint m_last_refresh;

  Sprite *image;
   
 public:
  Particle();
  virtual void Init()=0;
  void Draw();
  void Refresh();
  void Reset(){};
  bool StillUseful();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Smoke : public Particle
{
 public:
  Smoke();
  void Init();
};
//-----------------------------------------------------------------------------

class StarParticle : public Particle
{
 public:
  StarParticle();
  void Init();
};
//-----------------------------------------------------------------------------

class FireParticle : public Particle
{
 protected:
//  Character* dernier_ver_touche;
//  PhysicalObj* dernier_obj_touche;

 public: 
  SDL_Surface *impact;
  FireParticle();
  void Init();
  void SignalFallEnding();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ParticleEngine
{
 private:
  uint m_last_refresh;
  uint m_time_between_add;
  particle_t type_particle;
  std::list<Particle *> particles;

 public:
  ParticleEngine();
  ParticleEngine(particle_t type, uint time=100);
  void AddPeriodic(uint x, uint y, 
		   double angle=-1, double norme=-1);
  void AddNow(uint x, uint y, 
	      uint nb_particles, particle_t type, 
	      double angle=-1, double norme=-1);
  void Refresh();
  void Draw();
  void Stop();
};

extern ParticleEngine global_particle_engine;

#endif
