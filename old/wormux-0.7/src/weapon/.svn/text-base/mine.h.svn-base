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
 * Mine : Il s'agit de mines capables de detecter la presence d'un ver. Si 
 * c'est le cas, elle s'arme et là... faut pas y toucher ;) Apres un temps si
 * elle ne detecte personne elle se desarme.
 *****************************************************************************/
#ifndef MINE_H
#define MINE_H

#include <SDL.h>
#include "launcher.h"
#include "../graphic/surface.h"
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../team/character.h"

class Mine;
class MineConfig;

class ObjMine : public WeaponProjectile
{
private:
  // channel used for sound
  int channel;

  // Activation des mines ?
  bool animation;//,repos;
  uint attente;
  uint escape_time;

public:
  ObjMine(MineConfig &cfg);
  //  void Reset();
  void Explosion ();
  //void Draw();
  void EnableDetection();
  void DisableDetection();
  void Refresh();
  void Detection();
  void SignalCollision();
};

class MineConfig : public ExplosiveWeaponConfig
{ 
 private:
  static MineConfig * singleton;
 public: 
  uint escape_time;
  double detection_range;

 private:
  MineConfig();
public:
  static MineConfig * GetInstance();
  virtual void LoadXml(xmlpp::Element *elem);
};

class Mine : public WeaponLauncher
{
private:
  bool p_Shoot();
  void Add (int x, int y);
  void Refresh();

public:
  Mine();
  MineConfig& cfg();
};

#endif
