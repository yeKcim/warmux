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
 * Arme Supertux : and now the flying magic pinguin !
 *****************************************************************************/

#include "../weapon/supertux.h"
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
//-----------------------------------------------------------------------------
TuxLauncher tux;

const uint time_delta = 40;
const uint animation_deltat = 50;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SuperTux::SuperTux() : WeaponProjectile ("supertux"), 
		       particle_engine(particle_STAR,40)
{
  SetWindFactor (0.8);
  m_allow_negative_y = true;
  touche_ver_objet = true;

}

//-----------------------------------------------------------------------------

void SuperTux::Tire()
{
  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);

  // Fixe la force de départ
  angle = ActiveTeam().crosshair.GetAngleRad();
  PutOutOfGround(angle);
  SetExternForce(tux.cfg().speed, angle);
  time_next_action = Wormux::global_time.Read();
  last_move = Wormux::global_time.Read();
  camera.ChangeObjSuivi((PhysicalObj*)this,true,true);
}

//-----------------------------------------------------------------------------

void SuperTux::Init()
{
#ifdef CL
  image = CL_Sprite("supertux", &graphisme.weapons);
  SetSize (image.get_width(), image.get_height());
#else
  image = resource_manager.LoadSprite(weapons_res_profile,"supertux");
  image->EnableLastFrameCache();
  SetSize(image->GetWidth(), image->GetHeight());
#endif

  SetMass (tux.cfg().mass);
  m_gravity_factor = 0.0;

  // Fixe le rectangle de test
#ifdef CL
  int dx = image.get_width()/2-1;
  int dy = image.get_height()/2-1;
#else
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
#endif

  SetTestRect (dx, dx, dy, dy);
}

//-----------------------------------------------------------------------------

void SuperTux::Refresh()
{
  if (!is_active) return;

  if (TestImpact()) { SignalCollision(); return; }

  image->SetRotation_deg((angle+M_PI_2)*180.0/M_PI);
  if ((last_move+animation_deltat)<Wormux::global_time.Read())
    {
      SetExternForce(tux.cfg().speed, angle);
#ifdef CL
      image.update();
#else
      image->Update();
#endif
      last_move = Wormux::global_time.Read();
  }

  particle_engine.AddPeriodic(GetX(),GetY(),angle, 0);
}


//----------------------------------------------------------------------------

void SuperTux::turn_left()
{  
  time_now = Wormux::global_time.Read();
  if (time_next_action<time_now)
    {
      time_next_action=time_now + time_delta;
      angle = angle - 15.0/180.0*M_PI;
    }
}


//----------------------------------------------------------------------------

void SuperTux::turn_right()
{
  time_now = Wormux::global_time.Read();
  if (time_next_action<time_now)
    {
      time_next_action=time_now + time_delta;
      angle = angle + 15.0/180.0*M_PI;
    }
}

//----------------------------------------------------------------------------

void SuperTux::SignalCollision()
{ 

  particle_engine.Stop();
  
  if (IsGhost())
  {
    game_messages.Add (_("Bye bye tux..."));
  }
  is_active = false; 
}

//----------------------------------------------------------------------------

void SuperTux::Draw()
{ 
  particle_engine.Draw();

  // supertux must be upper the particles
  WeaponProjectile::Draw(); 
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SuperTuxWeaponConfig::SuperTuxWeaponConfig()
{
}

void SuperTuxWeaponConfig::LoadXml(xmlpp::Element *elem) 
{
  WeaponConfig::LoadXml (elem);
  LitDocXml::LitUint (elem, "speed", speed);
}

//-----------------------------------------------------------------------------

TuxLauncher::TuxLauncher() : Weapon(WEAPON_SUPERTUX, "tux")
{ 
  m_name = _("SuperTux");   
  override_keys = true ;

  m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
  extra_params = new SuperTuxWeaponConfig();
}

//-----------------------------------------------------------------------------

bool TuxLauncher::p_Shoot()
{
  // Initialise le supertux
  supertux.Tire ();
  lst_objets.AjouteObjet (&supertux, true);

#ifdef CL
  jukebox.PlayProfile(ActiveTeam().GetSoundProfile(), "fire");
#else
  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");
#endif

  return true;
}

//-----------------------------------------------------------------------------

void TuxLauncher::Explosion()
{
  m_is_active = false;
  
  lst_objets.RetireObjet (&supertux);

  // On fait un trou ?
  if (supertux.IsGhost()) return;

  // Applique les degats et le souffle aux vers
#ifdef CL
  CL_Point pos = supertux.GetCenter();
#else
  Point2i pos = supertux.GetCenter();
#endif

  AppliqueExplosion (pos, pos, impact, cfg(), NULL);


}

//-----------------------------------------------------------------------------

void TuxLauncher::Refresh()
{
  if (!m_is_active) return;  
  if (!supertux.is_active) Explosion();
}

//-----------------------------------------------------------------------------

void TuxLauncher::p_Init()
{
  supertux.Init();
#ifdef CL
  impact = CL_Surface("tux_impact", &graphisme.weapons);
#else
  impact = resource_manager.LoadImage(weapons_res_profile,"tux_impact");
#endif
}


//-----------------------------------------------------------------------------
                                                                                    
void TuxLauncher::HandleKeyEvent(int action, int event_type)
{
  switch (action) {
  case ACTION_MOVE_LEFT:
    if (event_type != KEY_RELEASED)
      supertux.turn_left();
    break ;
    
  case ACTION_MOVE_RIGHT:
    if (event_type != KEY_RELEASED)
      supertux.turn_right();
    break ;
    
  default:
    break ;
  } ;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SuperTuxWeaponConfig& TuxLauncher::cfg()
{ return static_cast<SuperTuxWeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
