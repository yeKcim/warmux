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
 * Arme dynamite : lorqu'on "tire", un baton de dynamite est lâché. Celui
 * explos après un laps de temps. La baton fait alors un gros trou dans la
 * carte, souffle les vers qui sont autour en leur faisant perdre de l'énergie.
 *****************************************************************************/

#include "dynamite.h"
//-----------------------------------------------------------------------------
#include "../game/game_mode.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../game/config.h"
#include "../game/game_loop.h"
#include "../weapon/weapon_tools.h"
#include "../object/objects_list.h"
#ifdef CL
#else
#include "../tool/resource_manager.h"
#include "../include/app.h"
#endif

#ifdef __MINGW32__
#undef LoadImage
#endif
//-----------------------------------------------------------------------------
namespace Wormux {

#ifdef DEBUG
  //  #define DEBUG_CADRE_TEST
#endif

//-----------------------------------------------------------------------------

BatonDynamite::BatonDynamite(Dynamite &p_dynamite) 
  : WeaponProjectile("baton de dynamite"), dynamite(p_dynamite)
{}

//-----------------------------------------------------------------------------

void BatonDynamite::Init()
{
  SetMass (dynamite.cfg().mass);

  image = resource_manager.LoadSprite(weapons_res_profile,"dynamite_anim");
  
  double delay = dynamite.cfg().duree/image->GetFrameCount()/1000.0 ;
  for (uint i=0 ; i < image->GetFrameCount(); i++)
    ; // TODO // image.set_frame_delay(i, delay) ;
  
  image->Start();
  
  SetSize (image->GetWidth(), image->GetHeight());

  SetTestRect (0, 0, 2, 3);

  explosion = resource_manager.LoadSprite(weapons_res_profile, "explosion");
  delay = 60/explosion->GetFrameCount()/1000.0 ;
  for (uint i=0 ; i < explosion->GetFrameCount(); i++)
    ; // TODO explosion.set_frame_delay(i, delay) ;

  explosion->Start();
  
}

//-----------------------------------------------------------------------------

void BatonDynamite::Reset()
{
  Ready();
  is_active = false;
  explosion_active = false;

  image->Start();
  image->SetCurrentFrame(0);
  image->Scale(ActiveCharacter().GetDirection(), 1);
  explosion->Start();
  explosion->SetCurrentFrame(0);
}

//-----------------------------------------------------------------------------

void BatonDynamite::Refresh()
{

  if (!is_active) return;
  bool fin;
  assert (!IsGhost());
  if (!explosion_active) {
    image->Update(); 
    fin = image->GetCurrentFrame() == image->GetFrameCount()-1;
    if (fin) explosion_active = true;
  } else {
    explosion->Update();
    fin = explosion->GetCurrentFrame() == explosion->GetFrameCount()-1;
    if (fin) is_active = false;
  }

}

//-----------------------------------------------------------------------------


void BatonDynamite::Draw()
{
  if (!is_active) return;
  assert (!IsGhost());

  int x = GetX();
  int y = GetY();
  if (!explosion_active)
    image->Draw(x,y);
  else {
    x -= explosion->GetWidth()/2;
    y -= explosion->GetHeight()/2;
    explosion->Draw(x,y);
  }
}
   
//-----------------------------------------------------------------------------

void BatonDynamite::SignalCollision() {}

void BatonDynamite::SignalGhostState (bool) { is_active = false; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Dynamite::Dynamite() 
  : Weapon(WEAPON_DYNAMITE, "dynamite"), baton(*this)
{
  m_name = _("Dynamite");
  extra_params = new DynamiteConfig();
  channel = -1;

   m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
}

//-----------------------------------------------------------------------------

void Dynamite::p_Init()
{
  baton.Init();
  impact = resource_manager.LoadImage(weapons_res_profile,"dynamite_impact");
}

//-----------------------------------------------------------------------------

void Dynamite::p_Select()
{
  baton.Reset();
}

//-----------------------------------------------------------------------------

// Pose une dynamite
bool Dynamite::p_Shoot ()
{
  DoubleVector speed_vector ;

  // Ajoute la représentation
  int x,y;
  PosXY (x,y);
  baton.Reset ();
  baton.PrepareTir();
  baton.SetXY (x, y);
  lst_objets.AjouteObjet (&baton, true);

  // Ajoute la vitesse actuelle du ver
  ActiveCharacter().GetSpeedXY (speed_vector);
  baton.SetSpeedXY (speed_vector);

  // Active l'animation
  channel = jukebox.Play("share","weapon/dynamite_fuze");

  return true;
}

//-----------------------------------------------------------------------------

void Dynamite::Refresh()
{
  if (m_is_active) {
    // Fin de l'explwosion ?
    if (!baton.is_active) FinExplosion ();
  } else {
    // Change le sens de l'image si nécessaire
    m_image->Scale(ActiveCharacter().GetDirection(), 1);
  }
}

//-----------------------------------------------------------------------------

void Dynamite::FinExplosion ()
{
  m_is_active = false;

  lst_objets.RetireObjet (&baton);

  jukebox.Stop(channel);
  channel = -1;
   
  // Si la dynamite est sortie de l'écran, on ne fait rien
  if (baton.IsGhost()) return;

  // Applique les degats aux vers
  Point2i centre = baton.GetCenter();
  centre.y = baton.GetY()+baton.GetHeight();
  AppliqueExplosion (centre, centre, impact, cfg(), NULL, "weapon/dynamite_exp");
}

//-----------------------------------------------------------------------------

DynamiteConfig& Dynamite::cfg() 
{ return static_cast<DynamiteConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

DynamiteConfig::DynamiteConfig()
{
  duree = 2000;
}

void DynamiteConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  LitDocXml::LitUint (elem, "duree", duree);
}

//-----------------------------------------------------------------------------
} // namespace Wormux
