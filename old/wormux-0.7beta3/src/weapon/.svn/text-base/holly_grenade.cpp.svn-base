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
 * Holly Grenade
 *****************************************************************************/

#include "holly_grenade.h"
//-----------------------------------------------------------------------------
#include <sstream>
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
#include "../include/global.h"
//-----------------------------------------------------------------------------
namespace Wormux {

HollyGrenadeLauncher holly_grenade_launcher;

#ifdef DEBUG

// #define MSG_DBG

#define COUT_DBG std::cout << "[HollyGrenade] "

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

HollyGrenade::HollyGrenade() : WeaponProjectile ("holly_grenade"), 
			       smoke_engine(particle_SMOKE,40)
{
  m_allow_negative_y = true;
  m_rebound_sound = "weapon/holly_grenade_bounce";
  m_rebounding = true;
  touche_ver_objet = false;
  sing_alleluia = false;
}

//-----------------------------------------------------------------------------

void HollyGrenade::Tire (double force)
{
  SetAirResistFactor(holly_grenade_launcher.cfg().air_resist_factor);

  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);

  // Set the initial speed.
  SetSpeed (force, ActiveTeam().crosshair.GetAngleRad());

#ifdef MSG_DBG
  COUT_DBG << "HollyGrenade::Tire()" << std::endl;
#endif

  // Recupere le moment du départ
  temps_debut_tir = Wormux::global_time.Read();
  sing_alleluia = false;
}

//-----------------------------------------------------------------------------

void HollyGrenade::Init()
{
#ifdef CL
  image = CL_Sprite("holly_grenade_sprite", &graphisme.weapons);
  SetSize (image.get_width(), image.get_height());
#else
  image = resource_manager.LoadSprite( weapons_res_profile, "holly_grenade_sprite");
  image->EnableRotationCache(32);
  SetSize (image->GetWidth(), image->GetHeight());
#endif

  SetMass (holly_grenade_launcher.cfg().mass);
  SetAirResistFactor(holly_grenade_launcher.cfg().air_resist_factor);
  m_rebound_factor = double(holly_grenade_launcher.cfg().rebound_factor);

  // Fixe le rectangle de test
#ifdef CL
  int dx = image.get_width()/2-1;
  int dy = image.get_height()/2-1;
#else
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
#endif

  SetTestRect (dx, dx, dy, dy);

#ifdef MSG_DBG
  COUT_DBG << "HollyGrenade::Init()" << std::endl;
#endif
}

//-----------------------------------------------------------------------------

void HollyGrenade::Refresh()
{
  if (!is_active) return;

  smoke_engine.AddPeriodic(GetX(),GetY());

#ifdef MSG_DBG
  COUT_DBG << "HollyGrenade::Refresh()" << std::endl;
#endif

  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding.");
    smoke_engine.Stop();
    is_active = false;
    return;
  }

  //5 sec après avoir été tirée, la grenade explose
  double tmp = Wormux::global_time.Read() - temps_debut_tir;
  if(tmp>1000 * holly_grenade_launcher.cfg().timeout) {
    smoke_engine.Stop();
    is_active = false;
    return;
  }

  // Sing Alleluia ;-)
  if (tmp > (1000 * holly_grenade_launcher.cfg().timeout - 2000) && !sing_alleluia) {
#ifdef CL
    jukebox.Play("weapon/alleluia") ;
#else
    jukebox.Play("share","weapon/alleluia") ;
#endif
    sing_alleluia = true;
  }
  

  if (TestImpact()) { SignalCollision(); return; }

  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180.0/M_PI ;
  image->SetRotation_deg(angle);
}

//-----------------------------------------------------------------------------

void HollyGrenade::Draw()
{
  if (!is_active) return;
  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding");
    is_active = false;
    return;
  }
  
  // draw smoke particles below the grenade
  smoke_engine.Draw();

  image->Draw(GetX(),GetY());
  int tmp = holly_grenade_launcher.cfg().timeout;
  tmp -= (int)((Wormux::global_time.Read() - temps_debut_tir) / 1000);
  std::ostringstream ss;
  ss << tmp;
  int txt_x = GetX() + GetWidth() / 2;
  int txt_y = GetY() - GetHeight();
  global().normal_font().WriteCenterTop (txt_x-camera.GetX(), txt_y-camera.GetY(), ss.str(),white_color);
}

//-----------------------------------------------------------------------------

void HollyGrenade::SignalCollision()
{   
  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding");
    smoke_engine.Stop();
    is_active = false ;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

HollyGrenadeLauncher::HollyGrenadeLauncher() : Weapon(WEAPON_HOLLY_GRENADE, "holly_grenade")
{  
  m_name = _("HollyGrenade");

  m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
  extra_params = new GrenadeConfig();
}

//-----------------------------------------------------------------------------

bool HollyGrenadeLauncher::p_Shoot ()
{
  // Initialise la grenade
  grenade.Tire (m_strength);
  camera.ChangeObjSuivi (&grenade, true, false);
  lst_objets.AjouteObjet (&grenade, true);

#ifdef CL
  jukebox.PlayProfile(ActiveTeam().GetSoundProfile(), "fire");
#else
  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");
#endif

  return true;
}

//-----------------------------------------------------------------------------

void HollyGrenadeLauncher::Explosion()
{
  m_is_active = false;

#ifdef MSG_DBG
  COUT_DBG << "LanceHollyGrenade::Explosion()" << std::endl;
#endif
  
  lst_objets.RetireObjet (&grenade);

  // On fait un trou ?
  if (grenade.IsGhost()) return;

  // Applique les degats et le souffle aux vers
#ifdef CL
  CL_Point pos = grenade.GetCenter();
#else
  Point2i pos = grenade.GetCenter();
#endif 
  AppliqueExplosion (pos, pos, impact, cfg(), NULL);
}

//-----------------------------------------------------------------------------

void HollyGrenadeLauncher::Refresh()
{
  if (m_is_active)
  {
    if (!grenade.is_active) Explosion();
  } 
}

//-----------------------------------------------------------------------------

void HollyGrenadeLauncher::p_Init()
{
  grenade.Init();
#ifdef CL
  impact = CL_Surface("holly_grenade_impact", &graphisme.weapons);
#else
  impact = resource_manager.LoadImage( weapons_res_profile, "holly_grenade_impact");
#endif
}

//-----------------------------------------------------------------------------

GrenadeConfig& HollyGrenadeLauncher::cfg() 
{ return static_cast<GrenadeConfig&>(*extra_params); }
//-----------------------------------------------------------------------------

} // namespace Wormux
