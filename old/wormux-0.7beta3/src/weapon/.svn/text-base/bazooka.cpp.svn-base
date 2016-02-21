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

#include "../weapon/bazooka.h"
//-----------------------------------------------------------------------------
#include "../game/config.h"
#include "../game/time.h"
#include "../team/teams_list.h"
#include "../interface/interface.h"
#include "../graphic/video.h"
#include "../tool/math_tools.h"
#include "../game/game_loop.h"
#include "../tool/i18n.h"
#include "../map/camera.h"
#include "../weapon/weapon_tools.h"
#include "../interface/game_msg.h"
#include "../object/objects_list.h"
//-----------------------------------------------------------------------------
namespace Wormux {

// Bazooka bazooka;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RoquetteBazooka::RoquetteBazooka(Bazooka &p_bazooka) 
  : WeaponProjectile ("roquette_bazooka"), bazooka(p_bazooka)
{
  m_allow_negative_y = true;
  touche_ver_objet = true;
  m_wind_factor = 1.0;
}

//-----------------------------------------------------------------------------

void RoquetteBazooka::Tire (double force)
{
  SetAirResistFactor(bazooka.cfg().air_resist_factor);

  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);
  
  // Set the initial speed.
  double angle = ActiveTeam().crosshair.GetAngleRad();
  SetSpeed (force, angle);
  PutOutOfGround(angle);
}

//-----------------------------------------------------------------------------

void RoquetteBazooka::Init()
{
  image = resource_manager.LoadSprite( weapons_res_profile, "roquette");
  image->EnableRotationCache(32);
  SetSize (image->GetWidth(), image->GetHeight());

  SetMass (bazooka.cfg().mass);
  SetWindFactor(5.0);
  SetAirResistFactor(bazooka.cfg().air_resist_factor);


  // Fixe le rectangle de test
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy); 
}

//-----------------------------------------------------------------------------

void RoquetteBazooka::Refresh()
{
  if (!is_active) return;

  if (TestImpact()) { SignalCollision(); return; }

  double angle = GetSpeedAngle() *180/M_PI;
  image->SetRotation_deg( angle);
}

//-----------------------------------------------------------------------------

void RoquetteBazooka::SignalCollision()
{ 
  if (IsGhost())
  {
    game_messages.Add (_("The rocket left the battlefield..."));
  }
  is_active = false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Bazooka::Bazooka() 
  : Weapon(WEAPON_BAZOOKA, "bazooka"), roquette(*this)
{  
  m_name = _("Bazooka");
  extra_params = new ExplosiveWeaponConfig();
}

//-----------------------------------------------------------------------------

bool Bazooka::p_Shoot ()
{
  if (m_strength == max_strength)
  {
    m_strength = 0;
    ExplosionDirecte();
    return true;
  }

  // Initialise le roquette
  roquette.Tire (m_strength);
  lst_objets.AjouteObjet (&roquette, true);
  camera.ChangeObjSuivi(&roquette, 1, 1,1);

  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");
  return true;
}

//-----------------------------------------------------------------------------

// Le bazooka explose car il a été poussé à bout !
void Bazooka::ExplosionDirecte()
{
  Point2i pos = ActiveCharacter().GetCenter();
  AppliqueExplosion (pos, pos, impact, cfg(), NULL);
}

//-----------------------------------------------------------------------------

void Bazooka::Explosion()
{
  m_is_active = false;
  
  lst_objets.RetireObjet (&roquette);

  // On fait un trou ?
  if (roquette.IsGhost()) return;

  // Applique les degats et le souffle aux vers
  Point2i pos = roquette.GetCenter();
  AppliqueExplosion (pos, pos, impact, cfg(), NULL);
}

//-----------------------------------------------------------------------------

void Bazooka::Refresh()
{
  if (!m_is_active) return;
  if (!roquette.is_active) Explosion();
}

//-----------------------------------------------------------------------------

void Bazooka::p_Init()
{

  roquette.Init();
  impact = resource_manager.LoadImage( weapons_res_profile, "bazooka_impact");
}

//-----------------------------------------------------------------------------

ExplosiveWeaponConfig& Bazooka::cfg()
{ return static_cast<ExplosiveWeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
