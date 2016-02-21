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
 * Air attack.
 *****************************************************************************/

#include "air_attack.h"
#include <sstream>
#include "../game/game_loop.h"
#include "../graphic/sprite.h"
#include "../include/action_handler.h"
#include "../interface/mouse.h"
#include "../map/map.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/random.h"
#include "../tool/i18n.h"
#include "../weapon/weapon_tools.h"

const uint FORCE_X_MIN = 10;
const uint FORCE_X_MAX = 120;
const uint FORCE_Y_MIN = 1;
const uint FORCE_Y_MAX = 40;

const double OBUS_SPEED = 7 ;

Obus::Obus(AirAttackConfig& cfg) :
  WeaponProjectile("obus", cfg)
{
  is_active = true;
  Ready();
}

void Obus::SignalCollision()
{ 
  is_active = false; 

  if (IsGhost()) return;
}



//-----------------------------------------------------------------------------

Avion::Avion(AirAttackConfig &p_cfg) : 
  PhysicalObj("Avion", 0.0),
  cfg(p_cfg)
{
  m_type = objUNBREAKABLE;
  SetWindFactor(1.0);  
  SetAirResistFactor(1.0);
  m_gravity_factor = 0.0;
  m_alive = GHOST;

  image = new Sprite( resource_manager.LoadImage( weapons_res_profile, "air_attack_plane"));
  SetSize(image->GetSize());
  SetMass (3000);
  obus_dx = 100;
  obus_dy = 50;
}

void Avion::Shoot(double speed)
{  
  obus_laches = false;
  obus_actifs = false;
  Point2d speed_vector ;
  int dir = ActiveCharacter().GetDirection();
  cible_x = Mouse::GetInstance()->GetWorldPosition().x;
  SetY (0);

  image->Scale(dir, 1);
   
  Ready();

  if (dir == 1)
    {
      speed_vector.SetValues( speed, 0);
      SetX (-image->GetWidth()+1);
    }
  else
    {
      speed_vector.SetValues( -speed, 0) ;
      SetX (world.GetWidth()-1);
    }

  SetSpeedXY (speed_vector);

  camera.ChangeObjSuivi (this, true, true);

  lst_objects.AddObject(this);
}

void Avion::Refresh()
{
  if (IsGhost()) return;  
  UpdatePosition();

  // L'avion est arrivé au bon endroit ? Largue les obus
  if (!obus_laches && PeutLacherObus())
  {
    obus_laches = true;
    obus_actifs = true;
    
    int x=LitCibleX();
    Obus * instance;

    for (uint i=0; i<cfg.nbr_obus; ++i) 
    {
      instance = new Obus(cfg);
      instance->SetXY( Point2i(x, obus_dy) );

      Point2d speed_vector;

      int fx = randomObj.GetLong (FORCE_X_MIN, FORCE_X_MAX);
      fx *= GetDirection();
      int fy = randomObj.GetLong (FORCE_Y_MIN, FORCE_Y_MAX);

      speed_vector.SetValues( fx/30.0, fy/30.0);
      instance->SetSpeedXY (speed_vector);
      obus.push_back (instance);
      
      camera.ChangeObjSuivi (instance, true, true);
    }
  }

  obus_actifs = false;
  iterator it=obus.begin(), end=obus.end();

  while (it != end) {     
    (*it)->Refresh();
    (*it)->UpdatePosition();

    if (!(*it)->is_active){
      (*it)->Explosion();
      it = obus.erase(it);
    } else {
      obus_actifs = true;
      ++it;
    }
  }

  if (!obus_actifs && IsGhost()) {  
    obus.clear();
    GameLoop::GetInstance()->interaction_enabled=true;
  }
}

int Avion::LitCibleX() const { return cible_x; }

int Avion::GetDirection() const { 
  float x,y;
  image->GetScaleFactors(x,y);
  return (x<0)?-1:1;
}

void Avion::Draw()
{
  if (IsGhost()) return;  
  image->Draw(GetPosition());  

  // Dessine les obus
  if (obus_actifs)
  {
    iterator it=obus.begin(), fin=obus.end();
    for (; it != fin; ++it) (*it)->Draw ();
  }
}

bool Avion::PeutLacherObus() const
{
  if (GetDirection() == 1) 
    return (cible_x <= GetX()+obus_dx);
  else
    return (GetX()+(int)image->GetWidth()-obus_dx <= cible_x);
}

void Avion::SignalGhostState (bool was_dead)
{
  lst_objects.RemoveObject(this);
}

//-----------------------------------------------------------------------------

AirAttack::AirAttack() :
  Weapon(WEAPON_AIR_ATTACK, "air_attack",new AirAttackConfig(), ALWAYS_VISIBLE),
  avion(cfg())
{  
  m_name = _("Air attack");
  can_be_used_on_closed_map = false;
}

void AirAttack::Refresh()
{
  if (!avion.obus_actifs && avion.obus_laches) m_is_active = false;
}

void AirAttack::ChooseTarget()
{
  ActiveTeam().GetWeapon().NewActionShoot();
}

bool AirAttack::p_Shoot ()
{
  GameLoop::GetInstance()->interaction_enabled=false;
  avion.Shoot (cfg().speed);
  m_is_active = false;
  return true;
}

AirAttackConfig& AirAttack::cfg() 
{ return static_cast<AirAttackConfig&>(*extra_params); }

//-----------------------------------------------------------------------------

AirAttackConfig::AirAttackConfig()
{
  nbr_obus = 3;
  speed = 7;
}

void AirAttackConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml(elem);
  LitDocXml::LitUint (elem, "nbr_obus", nbr_obus);
  LitDocXml::LitDouble (elem, "speed", speed);
}

