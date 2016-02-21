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
#include "weapon/weapon_launcher.h"
#include "include/base.h"

class ClusterBombConfig;

class ClusterLauncher : public WeaponLauncher
{
 public:
  ClusterLauncher();
  std::string GetWeaponWinString(const char *TeamName, uint items_count) const;

 protected:
  WeaponProjectile * GetProjectileInstance();
 private:
  ClusterBombConfig& cfg() ;
};

#endif
