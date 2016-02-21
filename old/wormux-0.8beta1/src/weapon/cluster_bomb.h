/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Cluster Bomb : launch a grenade will exploding, it produce new little cluster
 * exploding bomb
 *****************************************************************************/

#ifndef CLUSTER_BOMB_H
#define CLUSTER_BOMB_H

#include <list>
#include "launcher.h"
#include "graphic/surface.h"
#include "gui/progress_bar.h"
#include "include/base.h"
#include "object/physical_obj.h"

class ClusterBombConfig;

class Cluster : public WeaponProjectile
{
public:
  Cluster(ClusterBombConfig& cfg,
          WeaponLauncher * p_launcher);
  void Refresh();
  void Shoot(int n_x, int n_y);
protected:
  void SignalOutOfMap();
  void DoExplosion();
};

class ClusterBomb : public WeaponProjectile
{
public:
  ClusterBomb(ClusterBombConfig& cfg,
              WeaponLauncher * p_launcher);
  void Refresh();
  DECLARE_GETWEAPONSTRING();
protected:
  void DoExplosion();
  void SignalOutOfMap();
};

class ClusterBombConfig : public ExplosiveWeaponConfig
{ 
public: 
  uint nb_fragments;
public:
  ClusterBombConfig();
  virtual void LoadXml(xmlpp::Element *elem);
};

class ClusterLauncher : public WeaponLauncher
{
 public:
  ClusterLauncher();
 protected:
  WeaponProjectile * GetProjectileInstance();
 private:
  ClusterBombConfig& cfg() ;
};

#endif
