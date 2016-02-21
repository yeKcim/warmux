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
 * WeaponLauncher: generic weapon to launch a projectile
 *****************************************************************************/

#include "launcher.h"

#include <sstream>

#include "weapon_tools.h"
#include "../game/config.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/macro.h"
#include "../team/teams_list.h"
#include "../tool/debug.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"

WeaponBullet::WeaponBullet(const std::string &name, ExplosiveWeaponConfig& cfg) :
  WeaponProjectile(name, cfg)
{ 
  m_gravity_factor = 0.1; 
  SetWindFactor(0.8);
  SetAirResistFactor(1.0);

  cfg.explosion_range = 1;
}

void WeaponBullet::SignalCollision()
{ 
  if ((dernier_ver_touche == NULL) && (dernier_obj_touche == NULL))
  {
    GameMessages::GetInstance()->Add (_("Your shot has missed!"));
  }
  is_active = false; 
}

void WeaponBullet::Explosion()
{
  if (IsGhost()) return;

  if (dernier_ver_touche == NULL) {
    // Applique les degats et le souffle aux vers
    Point2i pos = GetCenter();
    ApplyExplosion (pos, cfg, NULL, "", false, ParticleEngine::LittleESmoke);
  } else {
    dernier_ver_touche -> SetEnergyDelta (-cfg.damage);
  }
}

//-----------------------------------------------------------------------------


WeaponProjectile::WeaponProjectile (const std::string &name, 
				    ExplosiveWeaponConfig& p_cfg)
  : PhysicalObj (name, 0.0),
    cfg(p_cfg)
{
  dernier_ver_touche = NULL;
  dernier_obj_touche = NULL;

  m_allow_negative_y = true;
  touche_ver_objet = true;
  explode_colliding_character = false;
  m_wind_factor = 1.0;

  image = resource_manager.LoadSprite( weapons_res_profile, name);
  image->EnableRotationCache(32);
  SetSize(image->GetSize());

  // Please do not set the physical factors from cfg here since the xml 
  // config file is still not loading

  // WARNING: If you remove it, weapon which do not use method Shoot() 
  // are completely crazy...!!!
  // TODO: find a clean solution !!! -> BUG #5631
  SetMass (cfg.mass);
  SetWindFactor(cfg.wind_factor);
  SetAirResistFactor(cfg.air_resist_factor);
  m_rebound_factor = cfg.rebound_factor;

  // Set rectangle test
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy);   
}

WeaponProjectile::~WeaponProjectile()
{
  //delete image; -> it causes a segfault :-/
}

void WeaponProjectile::Shoot(double strength)
{    
  MSG_DEBUG("weapon_projectile", "shoot.\n");

  Ready();
  is_active = true;

  // Set the physical factors
  SetMass (cfg.mass);
  SetWindFactor(cfg.wind_factor);
  SetAirResistFactor(cfg.air_resist_factor);
  m_rebound_factor = cfg.rebound_factor;

  // Set the initial position.
  SetXY( ActiveCharacter().GetHandPosition() );
  
  // Set the initial speed.
  double angle = ActiveTeam().crosshair.GetAngleRad();
  SetSpeed (strength, angle);
  PutOutOfGround(angle); 

  begin_time = Time::GetInstance()->Read();  

  ShootSound();  

  lst_objects.AddObject (this);
  camera.ChangeObjSuivi(this, true, true, true);

}

void WeaponProjectile::ShootSound()
{
  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");
}


bool WeaponProjectile::TestImpact()
{
  if (IsReady()) 
  {
    MSG_DEBUG("weapon_collision", "Impact because was ready.\n");
    return true;
  }
  return CollisionTest (0,0);
}

bool WeaponProjectile::CollisionTest(int dx, int dy)
{
  dernier_ver_touche = NULL;
  dernier_obj_touche = NULL;

  if (!IsInVacuum ( Point2i(dx, dy)) ) return true;

  if (!touche_ver_objet) return false;

   Rectanglei test = GetTestRect();
   test.SetPositionX( test.GetPositionX() + dx);
   test.SetPositionY( test.GetPositionY() + dy);
   
  FOR_ALL_LIVING_CHARACTERS(equipe,ver)
  if (&(*ver) != &ActiveCharacter())
  {
    if (ver->GetTestRect().Intersect( test ))
       {
      dernier_ver_touche = &(*ver);
      MSG_DEBUG("weapon_collision", "Character %s has been damaged", ver -> m_name.c_str());

      if (explode_colliding_character) {
	MSG_DEBUG("weapon_collision", "Projectile explode before timeout because of a collision", ver -> m_name.c_str());
	is_active = false;
      }
      return true;
    }
  }

  FOR_EACH_OBJECT(objet)
  if (objet -> ptr != this)
  {
    if ( objet->ptr->GetTestRect().Intersect( test ) )
      {
      dernier_obj_touche = objet -> ptr;
      MSG_DEBUG("weapon_collision", "Object %s has been touched", objet -> ptr -> m_name.c_str());
      return true;
    }
  }

  return false;
}

void WeaponProjectile::Refresh()
{
  if( !is_active )
    return;

  // Explose after timeout
  double tmp = Time::GetInstance()->Read() - begin_time;
  
  if(cfg.timeout && tmp > 1000 * cfg.timeout) {
    is_active = false;      
    return;
  }

  if( TestImpact() ){
    SignalCollision();
    return;
  }  
}

void WeaponProjectile::Draw()
{
  if( !is_active )
    return;

  image->Draw(GetPosition());
  
  int tmp = cfg.timeout;

  if (tmp != 0) { 
    tmp -= (int)((Time::GetInstance()->Read() - begin_time) / 1000);

    if (tmp >= 0) {
      std::ostringstream ss;
      ss << tmp;
      int txt_x = GetX() + GetWidth() / 2;
      int txt_y = GetY() - GetHeight();
      (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop( Point2i(txt_x, txt_y) - camera.GetPosition(),
					    ss.str(), white_color);
    }
  }
}

void WeaponProjectile::SignalGhostState (bool){  
  SignalCollision();
}

void WeaponProjectile::SignalFallEnding(){  
  SignalCollision();
}

void WeaponProjectile::Explosion()
{
  if (IsGhost()) return;

  // Applique les degats et le souffle aux vers
  Point2i pos = GetCenter();
  ApplyExplosion (pos, cfg, NULL);
}

//-----------------------------------------------------------------------------

WeaponLauncher::WeaponLauncher(Weapon_type type, 
			       const std::string &id,
			       EmptyWeaponConfig * params,
			       uint visibility) :
  Weapon(type, id, params, visibility)
{  
  projectile = NULL;
}

WeaponLauncher::~WeaponLauncher()
{
  if (projectile != NULL) delete projectile;
}

bool WeaponLauncher::p_Shoot ()
{
//   if (m_strength == max_strength)
//   {
//     m_strength = 0;
//     DirectExplosion();
//     return true;
//   }
  projectile->Shoot (m_strength);

  return true;
}

// Le bazooka explose car il a été poussé à bout !
void WeaponLauncher::DirectExplosion()
{
  Point2i pos = ActiveCharacter().GetCenter();
  ApplyExplosion (pos, cfg(), NULL);
}

void WeaponLauncher::Explosion()
{
  m_is_active = false;
  lst_objects.RemoveObject (projectile);
  projectile->Explosion();
}

void WeaponLauncher::Refresh()
{
  if (!m_is_active) return;
  if (!projectile->is_active) Explosion();
}

ExplosiveWeaponConfig& WeaponLauncher::cfg()
{ return static_cast<ExplosiveWeaponConfig&>(*extra_params); }

