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
 * Arme gun : la balle part tout droit dans la direction donnée par
 * le viseur. Si la balle ne touche pas un ver, elle va faire un trou dans
 * le terrain. La balle peut également toucher les objets du plateau du jeu.
 *****************************************************************************/

#include "../weapon/gun.h"
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
#include "../weapon/gun.h"
#include "../weapon/weapon_tools.h"
using namespace std;
//-----------------------------------------------------------------------------
namespace Wormux {

Gun gun;

const uint VITESSE_CAPTURE_POS_BALLE = 10;
const uint BULLET_SPEED = 20;
const double BULLET_BLAST = 1;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

BalleGun::BalleGun() : WeaponProjectile("balle_gun")
{ 
  touche_ver_objet = true; 
}

//-----------------------------------------------------------------------------

void BalleGun::Init()
{
  image = resource_manager.LoadSprite(weapons_res_profile,"gun_bullet");
  SetSize (image->GetWidth(), image->GetHeight());
  SetMass (0.02);
  SetWindFactor(0.05);
  SetAirResistFactor(0);
  m_gravity_factor = 0.0;
}

//-----------------------------------------------------------------------------

void BalleGun::Tire()
{
  PrepareTir();

  // Set the initial position.
  int x,y;
  ActiveCharacter().GetHandPosition(x, y);
  SetXY (x,y);
   
  // Set the initial speed.
  SetSpeed (BULLET_SPEED, ActiveTeam().crosshair.GetAngleRad());
}

//-----------------------------------------------------------------------------

void BalleGun::SignalCollision()
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

Gun::Gun() : Weapon(WEAPON_GUN, "gun")
{
  m_name = _("Gun");
  extra_params = new WeaponConfig(); 
}

//-----------------------------------------------------------------------------

void Gun::p_Init()
{
  balle.Init();
  impact = resource_manager.LoadImage( weapons_res_profile, "gun_impact");  
}

//-----------------------------------------------------------------------------

void Gun::Draw ()
{
  Weapon::Draw();

  // Dessine le tracé de la trajectoire
  if (m_is_active)
  {
#ifdef CL
    bool noir=true;
    for (std::vector<CL_Point>::const_iterator it=lst_points.begin();
	 it != lst_points.end();
	 ++it)
    {
      if (noir)
	CL_Display::draw_rect (CL_Rect(it -> x, it -> y, it -> x+1, it -> y+1),
			       CL_Color::black);
      else
	CL_Display::draw_rect (CL_Rect(it -> x, it -> y, it -> x+1, it -> y+1),
			       CL_Color::white);
      noir = !noir;
    }
#else
      // TODO 
#endif
  }
}

//-----------------------------------------------------------------------------

bool Gun::p_Shoot()
{
  if (m_is_active)
    return false;

  // Initialise la balle
  balle.Tire();

  // Temps de capture
  temps_capture = global_time.Read()+VITESSE_CAPTURE_POS_BALLE;

  lst_points.clear();
  lst_objets.AjouteObjet (&balle, true);

  jukebox.Play("share","weapon/gun");

  return true;
}

//-----------------------------------------------------------------------------

void Gun::Refresh()
{
  m_image->Scale(ActiveCharacter().GetDirection(), 1);   

   
  if (balle.is_active)
    {
      // Une balle est en l'air : on capture sa position ?
      if (temps_capture < global_time.Read()) 
	{
	  temps_capture = global_time.Read()+VITESSE_CAPTURE_POS_BALLE;

	  Point2i pos_balle = balle.GetPos();
	  pos_balle.x += balle.GetWidth()/2;
	  pos_balle.y += balle.GetHeight()/2;
	  lst_points.push_back (pos_balle);
	}
    }
  else
    {
      if (!m_is_active) return;
      m_is_active = false;
      
      if (!balle.IsGhost())
	{
	  // Si la balle a touché un ver, lui inflige des dégats
	  Character* ver = balle.LitDernierVerTouche();
	  PhysicalObj* obj = balle.LitDernierObjTouche();
	  if (ver) obj = ver;
	  if (ver) ver -> SetEnergyDelta (-cfg().damage);
	  if (obj) 
	    {
	      obj -> AddSpeed (BULLET_BLAST, balle.GetSpeedAngle());
	    }
	  
	  // Creuse le monde
	  if (!obj)
	    {
	       world.Creuse (balle.GetX() - impact->w/2,
			    balle.GetY() - impact->h/2,
			    impact);
	    }
	}
      
      lst_objets.RetireObjet (&balle);
    }
}

//-----------------------------------------------------------------------------

WeaponConfig& Gun::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
