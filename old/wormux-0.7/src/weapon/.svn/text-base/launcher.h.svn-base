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
 * WeaponLauncher: generic weapon to launch a projectile
 *****************************************************************************/

#ifndef WEAPON_LAUNCHER_H
#define WEAPON_LAUNCHER_H
#include "weapon.h"
#include "../graphic/surface.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../object/physical_obj.h"

class WeaponLauncher;

class WeaponProjectile : public PhysicalObj
{
 public:
  bool is_active;

 protected:
  Sprite *image;  

  // Peut toucher les vers et les objets ? (test de collision)
  bool touche_ver_objet;
  bool explode_colliding_character; // before timeout. touche_ver_objet must be true
  double begin_time;

  ExplosiveWeaponConfig& cfg;

 public:
  Character* dernier_ver_touche;
  PhysicalObj* dernier_obj_touche;

 public:
  WeaponProjectile(const std::string &nom, 
		   ExplosiveWeaponConfig& cfg);
  virtual ~WeaponProjectile();

  virtual void Draw();
  virtual void Refresh();
  virtual void Shoot(double strength);
  virtual void Explosion();

  bool CollisionTest (int dx, int dy); // public only for uzi...
 protected:
  virtual void SignalCollision() = 0; 
  bool TestImpact ();
  virtual void ShootSound();
 private:
  void SignalGhostState (bool was_dead);
  void SignalFallEnding();
};

class WeaponBullet : public WeaponProjectile
{
public:
  WeaponBullet(const std::string &name, ExplosiveWeaponConfig& cfg);
  virtual ~WeaponBullet(){};
protected:
  void SignalCollision();
  void Explosion(); 
};


class WeaponLauncher : public Weapon
{
 protected:
  WeaponProjectile * projectile;

 private:
  bool p_Shoot();
  void DirectExplosion();
  void Explosion();
  
 public:
  WeaponLauncher(Weapon_type type, 
		 const std::string &id,
		 EmptyWeaponConfig * params,
		 uint visibility = ALWAYS_VISIBLE);
  virtual ~WeaponLauncher();

  void Refresh();
  ExplosiveWeaponConfig& cfg();
};

#endif
