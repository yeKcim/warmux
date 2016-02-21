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
 * Arme grenade à fragmentation(cluster bomb)
 * Explose au bout de quelques secondes 
 *****************************************************************************/

#include "cluster_bomb.h"
#include <sstream>
#include <math.h>
#include "weapon_tools.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"
#include "../tool/random.h"

Cluster::Cluster(ClusterBombConfig& cfg) :
  WeaponProjectile ("cluster", cfg)
{
}

void Cluster::Shoot (int x, int y)
{
  Ready();
  camera.ChangeObjSuivi(this, true, false);
  is_active = true;
  SetXY( Point2i(x, y) );
}

void Cluster::Refresh()
{
  if (!is_active) return;

  double angle = GetSpeedAngle() * 180/M_PI ;

  image->SetRotation_deg( angle);
}

void Cluster::SignalCollision()
{
  is_active = false;
  lst_objects.RemoveObject((PhysicalObj*)this);

  if (IsGhost())
  {
    GameMessages::GetInstance()->Add (_("The rocket left the battlefield..."));
    return;
  }

  ApplyExplosion (GetPosition(), cfg, NULL, "weapon/explosion", false, ParticleEngine::LittleESmoke);
}

//-----------------------------------------------------------------------------

ClusterBomb::ClusterBomb(ClusterBombConfig& cfg) :
  WeaponProjectile ("cluster_bomb", cfg)
{
  m_rebound_sound = "weapon/grenade_bounce";
  touche_ver_objet = false;
  m_rebounding = true;

  tableau_cluster.clear();
  const uint nb = cfg.nb_fragments;

  for (uint i=0; i<nb; ++i)
  {
    Cluster cluster(cfg);
    tableau_cluster.push_back( cluster );
  }
}

void ClusterBomb::Refresh()
{
  WeaponProjectile::Refresh();
  
  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180/M_PI ;
  image->SetRotation_deg( angle);
}


void ClusterBomb::SignalCollision()
{   
  if (IsGhost())
  {
    GameMessages::GetInstance()->Add ("The Cluster Bomb left the battlefield before it could explode.");
    is_active = false ;
  }
}

void ClusterBomb::Explosion()
{
  if (IsGhost()) return;

  Point2d speed_vector;
  int x, y;

  GetSpeedXY(speed_vector);

  iterator it=tableau_cluster.begin(), end=tableau_cluster.end();
  for (; it != end; ++it)
    {
      Cluster &cluster = *it;
      
      double angle = randomObj.GetDouble(2.0 * M_PI);
      x = GetX()+(int)(cos(angle) * (double)cfg.blast_range*5);
      y = GetY()+(int)(sin(angle) * (double)cfg.blast_range*5);

      cluster.Shoot(x,y);
      cluster.SetSpeedXY(speed_vector);
      lst_objects.AddObject((PhysicalObj*)&cluster);
    }
  is_active = false;
}

//-----------------------------------------------------------------------------

ClusterLauncher::ClusterLauncher() : 
  WeaponLauncher(WEAPON_CLUSTER_BOMB, "cluster_bomb", new ClusterBombConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{  
  m_name = _("ClusterBomb");  
  
  projectile = new ClusterBomb(cfg());
}

ClusterBombConfig& ClusterLauncher::cfg() 
{ return static_cast<ClusterBombConfig&>(*extra_params); }

//-----------------------------------------------------------------------------

ClusterBombConfig::ClusterBombConfig() : 
  ExplosiveWeaponConfig()
{
  nb_fragments = 5;
}


void ClusterBombConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  LitDocXml::LitUint (elem, "nb_fragments", nb_fragments);
}

