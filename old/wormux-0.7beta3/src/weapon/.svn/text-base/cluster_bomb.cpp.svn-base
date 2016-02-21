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
//-----------------------------------------------------------------------------
#include <sstream>
#include <math.h>
#include "../game/time.h"
#include "../team/teams_list.h"
#include "../interface/interface.h"
#include "../graphic/video.h"
#include "../tool/math_tools.h"
#include "../game/game_loop.h"
#include "../map/camera.h"
#include "../weapon/weapon_tools.h"
#include "../interface/game_msg.h"
#include "../tool/i18n.h"
#include "../object/objects_list.h"
#include "../tool/random.h"
#include "../include/global.h"
//-----------------------------------------------------------------------------
namespace Wormux {

LanceCluster lance_cluster;

#ifdef DEBUG

#define COUT_DBG std::cout << "[ClusterBomb] "

#endif


//-----------------------------------------------------------------------------

Cluster::Cluster() : WeaponProjectile ("Cluster")
{
  m_allow_negative_y = true;
  touche_ver_objet = true;
}

//-----------------------------------------------------------------------------

void Cluster::Tire (int x, int y)
{
  PrepareTir();
  SetXY(x,y);
}

//-----------------------------------------------------------------------------

void Cluster::Init()
{
  image = resource_manager.LoadSprite( weapons_res_profile, "cluster");
  image->EnableRotationCache(32);
  SetSize (image->GetWidth(), image->GetHeight()); 

  SetMass (lance_cluster.cfg().mass / (lance_cluster.cfg().nbr_fragments+1));
  SetAirResistFactor(lance_cluster.cfg().air_resist_factor);

  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy);
}

//-----------------------------------------------------------------------------

void Cluster::Refresh()
{
  if (!is_active) return;

  double angle = GetSpeedAngle() * 180/M_PI ;

  image->SetRotation_deg( angle);
}

//-----------------------------------------------------------------------------

void Cluster::Draw()
{
  image->Draw(GetX(), GetY());
}

//-----------------------------------------------------------------------------

void Cluster::SignalCollision()
{
  is_active = false;
  lst_objets.RetireObjet((PhysicalObj*)this);
  if (IsGhost())
  {
    game_messages.Add (_("The rocket left the battlefield..."));
    return;
  }
  AppliqueExplosion (GetPos(), GetPos(), lance_cluster.impact, lance_cluster.cfg(), NULL);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ClusterBomb::ClusterBomb() : WeaponProjectile ("cluster_bomb")
{
  m_allow_negative_y = true;
  m_rebound_sound = "weapon/grenade_bounce";
  touche_ver_objet = false;
  m_rebounding = true;
}

//-----------------------------------------------------------------------------

ClusterBomb::~ClusterBomb()
{
  delete []tableau_cluster;
}

//-----------------------------------------------------------------------------

void ClusterBomb::Tire (double force)
{
  SetAirResistFactor(lance_cluster.cfg().air_resist_factor);

  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);

  // Set the initial speed
  double angle = ActiveTeam().crosshair.GetAngleRad();
  SetSpeed (force, angle);

  PutOutOfGround(angle);
#ifdef MSG_DBG
  COUT_DBG << "ClusterBomb::Tire()" << std::endl;
#endif

  // Recupere le moment du départ
  temps_debut_tir = Wormux::global_time.Read();
}

//-----------------------------------------------------------------------------

void ClusterBomb::Init()
{
  image = resource_manager.LoadSprite( weapons_res_profile, "clusterbomb_sprite");
  image->EnableRotationCache(32);
  SetSize (image->GetWidth(), image->GetHeight()); 

  SetMass (lance_cluster.cfg().mass);
  m_rebound_factor = lance_cluster.cfg().rebound_factor;

  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy);

#ifdef MSG_DBG
  COUT_DBG << "ClusterBomb::Init()" << std::endl;
#endif

  tableau_cluster = new Cluster[lance_cluster.cfg().nbr_fragments];
  for(uint i=0;i<lance_cluster.cfg().nbr_fragments;i++)
    tableau_cluster[i].Init();
}

//-----------------------------------------------------------------------------

void ClusterBomb::Refresh()
{
  if (!is_active) return;

#ifdef MSG_DBG
  COUT_DBG << "ClusterBomb::Refresh()" << std::endl;
#endif

  if (IsGhost())
  {
    game_messages.Add ("The Cluster Bomb left the battlefield before it could explode.");
    is_active = false;
    return;
  }

  // 5 sec après avoir été tirée, la grenade explose
  double tmp = Wormux::global_time.Read() - temps_debut_tir;
  if(tmp>1000 * lance_cluster.cfg().tps_avt_explosion)
  {
    DoubleVector speed_vector ;
    int x, y ;

    GetSpeedXY(speed_vector);

    for(uint i=0;i<lance_cluster.cfg().nbr_fragments;i++)
    {
      double angle = (double)RandomLong ((long)0.0, (long)(2.0 * M_PI));
      x = GetX()+(int)(cos(angle) * (double)lance_cluster.cfg().blast_range*5);
      y = GetY()+(int)(sin(angle) * (double)lance_cluster.cfg().blast_range*5);

      tableau_cluster[i].Tire(x,y);
      tableau_cluster[i].SetSpeedXY(speed_vector);
      lst_objets.AjouteObjet((PhysicalObj*)&tableau_cluster[i],false);
    }
    is_active = false;
    return;
  }

  if (TestImpact()) { SignalCollision(); return; }

  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180/M_PI ;
  image->SetRotation_deg( angle);
}

//-----------------------------------------------------------------------------

void ClusterBomb::Draw()
{
  if (!is_active) return;
  if (IsGhost())
  {
    game_messages.Add ("The Cluster Bomb left the battlefield before it could explode.");
    is_active = false;
    return;
  }

  image->Draw(GetX(), GetY());

  int tmp = lance_cluster.cfg().tps_avt_explosion;
  tmp -= (int)((Wormux::global_time.Read() - temps_debut_tir) / 1000);
  std::ostringstream ss;
  ss << tmp;
  int txt_x = GetX() + GetWidth() / 2;
  int txt_y = GetY() - GetHeight();

  global().small_font().WriteCenterTop (txt_x-camera.GetX(), txt_y-camera.GetY(), ss.str(), white_color);
}

//-----------------------------------------------------------------------------

void ClusterBomb::SignalCollision()
{   
  if (IsGhost())
  {
    game_messages.Add ("The Cluster Bomb left the battlefield before it could explode.");
    is_active = false ;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

LanceCluster::LanceCluster() : Weapon(WEAPON_CLUSTER_BOMB, "cluster_bomb")
{  
  m_name = _("ClusterBomb");  

  m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
  extra_params = new ClusterBombConfig();
}

//-----------------------------------------------------------------------------

bool LanceCluster::p_Shoot ()
{
  // Initialise la grenade
  cluster_bomb.Tire (m_strength);
  camera.ChangeObjSuivi (&cluster_bomb, true, false);
  lst_objets.AjouteObjet (&cluster_bomb, true);

  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");  

  return true;
}

//-----------------------------------------------------------------------------

void LanceCluster::Explosion()
{
  m_is_active = false;

#ifdef MSG_DBG
  COUT_DBG << "LanceCluster::Explosion()" << std::endl;
#endif
  
  lst_objets.RetireObjet (&cluster_bomb);

  // On fait un trou ?
  if (cluster_bomb.IsGhost()) return;

  // Applique les degats et le souffle aux vers
  Point2i pos = cluster_bomb.GetCenter();
  AppliqueExplosion (pos, pos, impact, cfg(), NULL);
}

//-----------------------------------------------------------------------------

void LanceCluster::Refresh()
{
  if (m_is_active)
  {
    if (!cluster_bomb.is_active) Explosion();
  } 
}

//-----------------------------------------------------------------------------

void LanceCluster::p_Init()
{
  cluster_bomb.Init();
  impact = resource_manager.LoadImage( weapons_res_profile, "grenade_impact");
}

//-----------------------------------------------------------------------------

ClusterBombConfig& LanceCluster::cfg() 
{ return static_cast<ClusterBombConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ClusterBombConfig::ClusterBombConfig()
{
  tps_avt_explosion = 10;
  rebound_factor = 0.75;
  nbr_fragments = 5;
}


void ClusterBombConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  LitDocXml::LitUint (elem, "tps_avt_explosion", tps_avt_explosion);
  LitDocXml::LitDouble (elem, "rebound_factor", rebound_factor);
  LitDocXml::LitUint (elem, "nbr_fragments", nbr_fragments);
}

//-----------------------------------------------------------------------------
} // namespace Wormux
