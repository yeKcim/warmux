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

#include "cluster_bomb.h"
#include <sstream>
#include <math.h>
#include "explosion.h"
#include "game/time.h"
#include "graphic/video.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"
#include "network/randomsync.h"

Cluster::Cluster(ClusterBombConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("cluster", cfg, p_launcher)
{
  explode_colliding_character = true;
}

void Cluster::Shoot (int x, int y)
{
  camera.FollowObject(this, true, false);
  ResetConstants();
  SetXY( Point2i(x, y) );
}

void Cluster::Refresh()
{
  image->SetRotation_rad(GetSpeedAngle());
}

void Cluster::SignalOutOfMap()
{
  WeaponProjectile::SignalOutOfMap();
}

void Cluster::DoExplosion()
{
  ApplyExplosion (GetPosition(), cfg, "weapon/explosion", false, ParticleEngine::LittleESmoke);
}

//-----------------------------------------------------------------------------

ClusterBomb::ClusterBomb(ClusterBombConfig& cfg,
                         WeaponLauncher * p_launcher)
  : WeaponProjectile ("cluster_bomb", cfg, p_launcher)
{
  m_rebound_sound = "weapon/grenade_bounce";
  explode_with_collision = false;
}

void ClusterBomb::Refresh()
{
  WeaponProjectile::Refresh();
  image->SetRotation_rad(GetSpeedAngle());
}

void ClusterBomb::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The Cluster Bomb has left the battlefield before it could explode."));
  WeaponProjectile::SignalOutOfMap();
}

void ClusterBomb::DoExplosion()
{
  const uint nb = static_cast<ClusterBombConfig &>(cfg).nb_fragments;
  Cluster * cluster;
  for (uint i=0; i<nb; ++i) {
    double angle = randomSync.GetDouble(2.0 * M_PI);
    int x = GetX()+(int)(cos(angle) * (double)cfg.blast_range * 0.9);
    int y = GetY()+(int)(sin(angle) * (double)cfg.blast_range * 0.9);
    cluster = new Cluster(static_cast<ClusterBombConfig &>(cfg), launcher);
    cluster->Shoot(x,y);
    lst_objects.AddObject(cluster);
  }
  WeaponProjectile::DoExplosion();
}

std::string ClusterBomb::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u custer bomb!",
            "%s team has won %u custer bombs!",
            items_count), TeamName, items_count);
}
//-----------------------------------------------------------------------------

ClusterLauncher::ClusterLauncher() :
  WeaponLauncher(WEAPON_CLUSTER_BOMB, "cluster_bomb", new ClusterBombConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Cluster Bomb");
  m_help = _("Timeout : Wheel mouse or Page Up/Down\nAngle : Up/Down\nFire : keep space key pressed until the desired strength\nan ammo per turn");
  m_category = THROW;
  ignore_collision_signal = true;
  ReloadLauncher();
}

WeaponProjectile * ClusterLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new ClusterBomb(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

ClusterBombConfig& ClusterLauncher::cfg()
{
  return static_cast<ClusterBombConfig&>(*extra_params);
}

//-----------------------------------------------------------------------------

ClusterBombConfig::ClusterBombConfig() :
  ExplosiveWeaponConfig()
{
  nb_fragments = 5;
}

void ClusterBombConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "nb_fragments", nb_fragments);
}
