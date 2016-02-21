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
 * Arme bazooka : projette une roquette avec un angle et une force donnée.
 *****************************************************************************/

#include "auto_bazooka.h"
#include "weapon_tools.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../interface/game_msg.h"
#include "../interface/mouse.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../map/wind.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"
#include "../object/objects_list.h"
#ifdef __MINGW32__
#undef LoadImage
#endif


//Temps en seconde à partir duquel la roquette se dirige vers la cible
const uint TPS_AV_ATTIRANCE = 1;

//-----------------------------------------------------------------------------

RoquetteTeteCherche::RoquetteTeteCherche(ExplosiveWeaponConfig& cfg) :
  WeaponProjectile("rocket", cfg)
{
  m_attire = false;
  SetGravityFactor(0.0);
}

void RoquetteTeteCherche::Shoot (double strength) 
{
  WeaponProjectile::Shoot(strength);

  angle_local=ActiveTeam().crosshair.GetAngleRad();
}

void RoquetteTeteCherche::Refresh()
{
  double angle, tmp ;

  if (!is_active) return;

  if (TestImpact()) { SignalCollision(); return; }

  if (!m_attire)
    {
      //La roquette tourne sur elle-même
      angle_local += M_PI / 8;
      if(angle_local > M_PI) angle_local = - M_PI;
      angle = angle_local;
  
      image->SetRotation_deg(angle *180/M_PI);
      
      //2 sec après avoir été tirée, la roquette se dirige vers la cible:
      tmp = Time::GetInstance()->Read() - begin_time;
      if(tmp>1000 * TPS_AV_ATTIRANCE)
	{
	  m_attire = true;
	  SetSpeed(0,0);
	  angle = GetPosition().ComputeAngle( m_cible );
	  image->SetRotation_deg(angle *180/M_PI);
	  SetExternForce(2000, angle);
	}
    }  
  else 
    {
      angle = GetSpeedAngle() *180/M_PI;
      image->SetRotation_deg( angle);
    }
}

void RoquetteTeteCherche::SignalCollision()
{ 
  m_attire = false;
  if (IsGhost())
  {
    GameMessages::GetInstance()->Add (_("The automatic rocket has left the battlefield..."));
  }
  is_active = false; 
}

// Choisit les coordonnées de la cible 	 
void RoquetteTeteCherche::SetTarget (int x, int y) 	 
{ 
  m_cible.x = x; 	 
  m_cible.y = y; 	 
} 	 

//-----------------------------------------------------------------------------

AutomaticBazooka::AutomaticBazooka() : 
  WeaponLauncher(WEAPON_AUTOMATIC_BAZOOKA, "automatic_bazooka",new ExplosiveWeaponConfig() )
{  
  m_name = _("Automatic bazooka");

  m_is_active = false;
  cible.choisie = false;

  projectile = new RoquetteTeteCherche(cfg());

  cible.image = resource_manager.LoadImage( weapons_res_profile, "baz_cible");
}

void AutomaticBazooka::Draw()
{
  Weapon::Draw();
  DrawTarget();
}


void AutomaticBazooka::Refresh()
{
  DrawTarget();

  WeaponLauncher::Refresh();
}

void AutomaticBazooka::p_Select()
{
  cible.choisie = false;
}

void AutomaticBazooka::p_Deselect()
{
  if (cible.choisie) {
    // need to clear the old target
    world.ToRedrawOnMap(Rectanglei(cible.pos.x-cible.image.GetWidth()/2,
				   cible.pos.y-cible.image.GetHeight()/2,
				   cible.image.GetWidth(),
				   cible.image.GetHeight()));
  }
}

void AutomaticBazooka::ChooseTarget()
{
  if (cible.choisie) {
    // need to clear the old target
    world.ToRedrawOnMap(Rectanglei(cible.pos.x-cible.image.GetWidth()/2,
				   cible.pos.y-cible.image.GetHeight()/2,
				   cible.image.GetWidth(),
				   cible.image.GetHeight()));
  }

  cible.pos = Mouse::GetInstance()->GetWorldPosition();
  DrawTarget();
  cible.choisie = true;
  static_cast<RoquetteTeteCherche *>(projectile)->SetTarget(cible.pos.x, cible.pos.y);
}

void AutomaticBazooka::DrawTarget()
{
  if( !cible.choisie )
	return;

  AppWormux::GetInstance()->video.window.Blit(cible.image, cible.pos - cible.image.GetSize()/2 - camera.GetPosition());
}

bool AutomaticBazooka::IsReady() const
{
  return (EnoughAmmo() && cible.choisie);  
}
