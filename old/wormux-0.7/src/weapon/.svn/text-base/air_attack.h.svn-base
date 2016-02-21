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
 * Air attack.
 *****************************************************************************/

#ifndef AIR_ATTACK_H
#define AIR_ATTACK_H
#include "../graphic/surface.h"
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "launcher.h"

class AirAttack;

class AirAttackConfig : public ExplosiveWeaponConfig
{ 
public:
  double speed;
  uint nbr_obus; 
public:
  AirAttackConfig();
  virtual void LoadXml(xmlpp::Element *elem);
};

class Obus : public WeaponProjectile
{
public:
  Obus(AirAttackConfig& cfg);

protected:
  void SignalCollision();
};

class Avion : public PhysicalObj
{
public:  
  std::list<Obus*> obus;
  typedef std::list<Obus*>::iterator iterator;
  bool obus_laches;
  bool obus_actifs;

  int obus_dx, obus_dy;
  Sprite *image;
private:
  int cible_x;
  AirAttackConfig &cfg;

public:
  Avion(AirAttackConfig& cfg);
  void Shoot(double speed);
  void Draw();
  void Refresh();
  bool PeutLacherObus() const;
  int LitCibleX() const;
  int GetDirection() const;
  void Avion::SignalGhostState (bool was_dead);
};

class AirAttack : public Weapon
{
private:
  bool p_Shoot();

public:
  Avion avion;

  AirAttack();
  void FinTir();
  void Refresh();
  virtual void ChooseTarget ();

 private:
  AirAttackConfig& cfg();
};

#endif
