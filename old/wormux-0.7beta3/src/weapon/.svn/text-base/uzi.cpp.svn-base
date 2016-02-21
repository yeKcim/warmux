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
 * Uzi !
 *****************************************************************************/

#include "../weapon/uzi.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "../game/game_loop.h"
#include "../map/map.h"
#include "../game/time.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../interface/game_msg.h"
#include "../interface/game_msg.h"
#include "../weapon/weapon_tools.h"
using namespace std;
//-----------------------------------------------------------------------------
namespace Wormux {
  Uzi uzi;
//-----------------------------------------------------------------------------

const double SOUFFLE_BALLE = 1;
const double MIN_TIME_BETWEEN_SHOOT = 70; // in milliseconds

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

BalleUzi::BalleUzi() : WeaponProjectile("balle_uzi")
{ 
  touche_ver_objet = true; 
}

//-----------------------------------------------------------------------------
void BalleUzi::Init()
{
  image = resource_manager.LoadSprite(weapons_res_profile,"gun_bullet");
  SetSize (image->GetWidth(), image->GetHeight());
  SetSize (2,2);
  SetMass (0.02);
  SetWindFactor(0.05);
  SetAirResistFactor(0);
}

//-----------------------------------------------------------------------------

void BalleUzi::SignalCollision()
{ 
  if ((dernier_ver_touche == NULL) && (dernier_obj_touche == NULL))
  {
    game_messages.Add (_("Your shot has missed!"));
  }
  is_active = false; 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Uzi::Uzi() : Weapon(WEAPON_UZI,"uzi")
{
  m_name = _("Uzi");
  override_keys = true ;

  m_first_shoot = 0;

  m_visibility = ALWAYS_VISIBLE;
  extra_params = new WeaponConfig();
}

//-----------------------------------------------------------------------------

void Uzi::p_Init()
{
  balle.Init();
  impact = resource_manager.LoadImage( weapons_res_profile, "uzi_impact");  
}

//-----------------------------------------------------------------------------

void Uzi::p_Deselect()
{
  m_is_active = false;
}

//-----------------------------------------------------------------------------

void Uzi::RepeatShoot()
{        
  uint time = Wormux::global_time.Read() - m_first_shoot; 
  uint tmp = Wormux::global_time.Read();

  if (time >= MIN_TIME_BETWEEN_SHOOT)
  {
    m_is_active = false;
    NewActionShoot();
    m_first_shoot = tmp;
  }
}

//-----------------------------------------------------------------------------

bool Uzi::p_Shoot()
{
  jukebox.Play("share", "weapon/uzi");

  // Calculate movement of the bullet
  
  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);

  // Equation of movement : y = ax + b
  double angle,a,b ;
  angle = ActiveTeam().crosshair.GetAngleRad();
  a=sin(angle)/cos(angle);
  b= y-(a*x) ;

  // Move the bullet !!
  balle.PrepareTir();  
  balle.SetXY (x,y);

  while (balle.is_active) {
    y = int(double((a*x) + b)) ;

    balle.SetXY(x,y);

    // the bullet in gone outside the map
    if (balle.IsGhost()) {
      balle.is_active=false;
      return true;
    }
    
    // is there a collision ??
    if (balle.CollisionTest(0,0)) 
    {
      balle.is_active=false;

      // Si la balle a touché un ver, lui inflige des dégats
      Character* ver = balle.LitDernierVerTouche();
      PhysicalObj* obj = balle.LitDernierObjTouche();
      if (ver) obj = ver;
      if (ver) ver -> SetEnergyDelta (-cfg().damage);
      if (obj) 
      {
	obj -> AddSpeed (SOUFFLE_BALLE, angle);
      }

      // Creuse le world
      if (!obj)
      {
	world.Creuse (balle.GetX() - impact->w/2,
		      balle.GetY() - impact->h/2,
		      impact);
      }
      return true;
    }
    x+=ActiveCharacter().GetDirection();
  }

  return true;
}

//-----------------------------------------------------------------------------

void Uzi::Refresh()
{
  m_image->Scale(ActiveCharacter().GetDirection(), 1);
}
//-----------------------------------------------------------------------------

void Uzi::HandleKeyEvent(int action, int event_type)
{

  switch (action) {    

  case ACTION_SHOOT:
    
    if (event_type == KEY_REFRESH)
      RepeatShoot();
    
    if (event_type == KEY_RELEASED)
      m_is_active = false;

    break ;
    
  default:
    break ;
  } ;
}

//-----------------------------------------------------------------------------

WeaponConfig& Uzi::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
