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
 * Arme Sainte grenade : Comme la grenade mais en plus puissant :-)
 * Explose au bout de quelques secondes
 *****************************************************************************/

#include "grenade.h"
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
GrenadeLauncher lance_grenade;
//-----------------------------------------------------------------------------

#ifdef DEBUG

// #define MSG_DBG

#define COUT_DBG std::cout << "[Grenade] "

#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Grenade::Grenade() : WeaponProjectile ("grenade")
{
  m_allow_negative_y = true;
  m_rebound_sound = "weapon/grenade_bounce";
  m_rebounding = true;
  touche_ver_objet = false;
}

//-----------------------------------------------------------------------------

void Grenade::Tire (double force)
{
  SetAirResistFactor(lance_grenade.cfg().air_resist_factor);

  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);

  // Set the initial speed.
  double angle = ActiveTeam().crosshair.GetAngleRad();
  SetSpeed (force, angle);

  PutOutOfGround(angle);

#ifdef MSG_DBG
  COUT_DBG << "Grenade::Tire()" << std::endl;
#endif

  // Recupere le moment du départ
  temps_debut_tir = Wormux::global_time.Read();
}

//-----------------------------------------------------------------------------

void Grenade::Init()
{
#ifdef CL
  image = CL_Sprite("grenade_sprite", &graphisme.weapons);
  SetSize (image.get_width(), image.get_height());
#else
  image = resource_manager.LoadSprite( weapons_res_profile, "grenade_sprite");
  image->EnableRotationCache(32);
  SetSize (image->GetWidth(), image->GetHeight());
#endif 

  SetMass (lance_grenade.cfg().mass);
  SetAirResistFactor(lance_grenade.cfg().air_resist_factor);
  m_rebound_factor = double(lance_grenade.cfg().rebound_factor);

  // Fixe le rectangle de test
#ifdef CL
  int dx = image.get_width()/2-1;
  int dy = image.get_height()/2-1;
  SetTestRect (dx, dx, dy, dy);
#else
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy);   
#endif
   
#ifdef MSG_DBG
  COUT_DBG << "Grenade::Init()" << std::endl;
#endif
}

//-----------------------------------------------------------------------------

void Grenade::Refresh()
{
  if (!is_active) return;

#ifdef MSG_DBG
  COUT_DBG << "Grenade::Refresh()" << std::endl;
#endif

  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding.");
    is_active = false;
    return;
  }

  // Grenade explose after timeout
  double tmp = Wormux::global_time.Read() - temps_debut_tir;
  if(tmp>1000 * lance_grenade.cfg().timeout) {
    is_active = false;
    return;
  }

  if (TestImpact()) { SignalCollision(); return; }

  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180/M_PI ;
#ifdef CL
   image.set_angle(angle);
#else
  image->SetRotation_deg( angle);
#endif
}

//-----------------------------------------------------------------------------

void Grenade::Draw()
{
  if (!is_active) return;
  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding");
    is_active = false;
    return;
  }

#ifdef CL
  image.draw(GetX(),GetY());
#else
  image->Draw(GetX(),GetY());
#endif 
  int tmp = lance_grenade.cfg().timeout;
  tmp -= (int)((Wormux::global_time.Read() - temps_debut_tir) / 1000);
  std::ostringstream ss;
  ss << tmp;
  int txt_x = GetX() + GetWidth() / 2;
  int txt_y = GetY() - GetHeight();
#ifdef CL
  police_mix.WriteCenterTop (txt_x, txt_y, ss.str());
#else
  global().small_font().WriteCenterTop (txt_x-camera.GetX(), txt_y-camera.GetY(), ss.str(), white_color);
#endif
}

//-----------------------------------------------------------------------------

void Grenade::SignalCollision()
{   
  if (IsGhost())
  {
    game_messages.Add ("The grenade left the battlefield before exploding");
    is_active = false ;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GrenadeLauncher::GrenadeLauncher() : Weapon(WEAPON_GRENADE, "grenade")
{  
  m_name = _("Grenade");
 
  m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
  extra_params = new GrenadeConfig();
}

//-----------------------------------------------------------------------------

bool GrenadeLauncher::p_Shoot ()
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

#ifdef MSG_DBG
  COUT_DBG << "GrenadeLauncher::Tire()" << std::endl;
#endif

  return true;
}

//-----------------------------------------------------------------------------

void GrenadeLauncher::Explosion()
{
  m_is_active = false;

  printf ("GrenadeLauncher::Explosion : m_is_actrive = false\n");

#ifdef MSG_DBG
  COUT_DBG << "GrenadeLauncher::Explosion()" << std::endl;
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

void GrenadeLauncher::Refresh()
{
  if (m_is_active)
  {
    if (!grenade.is_active) Explosion();
  } 
}

//-----------------------------------------------------------------------------

void GrenadeLauncher::p_Init()
{
  grenade.Init();
#ifdef CL
  impact = CL_Surface("grenade_impact", &graphisme.weapons);
#else
  impact = resource_manager.LoadImage( weapons_res_profile, "grenade_impact");
#endif
}

//-----------------------------------------------------------------------------

GrenadeConfig& GrenadeLauncher::cfg() 
{ return static_cast<GrenadeConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GrenadeConfig::GrenadeConfig()
{
  timeout = 5;
  rebound_factor = 0.50;
}

void GrenadeConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  LitDocXml::LitUint (elem, "tps_avt_explosion", timeout);
  LitDocXml::LitDouble (elem, "rebound_factor", rebound_factor);
}

//-----------------------------------------------------------------------------

} // namespace Wormux
