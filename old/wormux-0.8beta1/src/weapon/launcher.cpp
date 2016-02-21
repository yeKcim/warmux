/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "explosion.h"
#include "game/config.h"
#include "game/time.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/macro.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

#ifdef DEBUG
//#define DEBUG_EXPLOSION_CONFIG
#endif

WeaponBullet::WeaponBullet(const std::string &name,
                           ExplosiveWeaponConfig& cfg,
                           WeaponLauncher * p_launcher) :
  WeaponProjectile(name, cfg, p_launcher)
{
  explode_colliding_character = true;
  ResetTimeOut();
}

// Signal that the bullet has hit the ground
void WeaponBullet::SignalGroundCollision()
{
  jukebox.Play("share", "weapon/ricoche1");
  WeaponProjectile::SignalGroundCollision();
  launcher->IncMissedShots();
}

void WeaponBullet::SignalOutOfMap()
{
  WeaponProjectile::SignalOutOfMap();
  launcher->IncMissedShots();
}

void WeaponBullet::SignalObjectCollision(PhysicalObj * obj)
{
  if (typeid(*obj) != typeid(Character)) {
    obj->AddDamage(cfg.damage);
    Explosion();
  } else {
    Character * tmp = (Character*)(obj);
    tmp -> SetEnergyDelta (-cfg.damage);
    tmp -> AddSpeed (2, GetSpeedAngle());
    Ghost();
  }
}

void WeaponBullet::Refresh()
{
  WeaponProjectile::Refresh();
  image->SetRotation_rad(GetSpeedAngle());
}

void WeaponBullet::DoExplosion()
{
  Point2i pos = GetCenter();
  ApplyExplosion (pos, cfg, "", false, ParticleEngine::LittleESmoke);
}
//-----------------------------------------------------------------------------


WeaponProjectile::WeaponProjectile (const std::string &name,
                                    ExplosiveWeaponConfig& p_cfg,
                                    WeaponLauncher * p_launcher)
  : PhysicalObj (name), cfg(p_cfg)
{
  m_allow_negative_y = true;
  SetCollisionModel(false, true, true);
  launcher = p_launcher;

  explode_colliding_character = false;
  explode_with_timeout = true;
  explode_with_collision = true;

  image = resource_manager.LoadSprite( weapons_res_profile, name);
  image->EnableRotationCache(32);
  SetSize(image->GetSize());
  camera_follow_closely = true;

  // Set rectangle test
  int dx = image->GetWidth()/2-1;
  int dy = image->GetHeight()/2-1;
  SetTestRect (dx, dx, dy, dy);

  ResetTimeOut();
}

WeaponProjectile::~WeaponProjectile()
{
  //  delete image; /*-> it causes a segfault :-/ */
}

void WeaponProjectile::Shoot(double strength)
{
  MSG_DEBUG("weapon.projectile", "shoot with strength:%f", strength);

  Init();

  if (launcher != NULL) launcher->IncActiveProjectile();

  // Set the physical factors
  ResetConstants();

  // Set the initial position.
  SetXY(launcher->GetGunHolePosition());
  SetOverlappingObject(&ActiveCharacter());

  // Set the initial speed.
  double angle = ActiveCharacter().GetFiringAngle();
  RandomizeShoot(angle,strength);
  SetSpeed (strength, angle);
  PutOutOfGround(angle);

  MSG_DEBUG("weapon.projectile", "shoot with hand position:%d,%d",
	    ActiveCharacter().GetHandPosition().GetX(),
	    ActiveCharacter().GetHandPosition().GetY());
  MSG_DEBUG("weapon.projectile", "shoot with strength:%f, angle:%f, position:%d,%d",
	    strength, angle, GetX(), GetY());

  begin_time = Time::GetInstance()->Read();

  ShootSound();

  lst_objects.AddObject (this);
  camera.FollowObject(this, true, true, true);
  if (camera_follow_closely)
    camera.SetCloseFollowing(true);
}

void WeaponProjectile::ShootSound()
{
  jukebox.Play(ActiveTeam().GetSoundProfile(), "fire");
}

void WeaponProjectile::Refresh()
{
  // Explose after timeout
  double tmp = Time::GetInstance()->Read() - begin_time;

  if(cfg.timeout && tmp > 1000 * (GetTotalTimeout())) SignalTimeout();
}

void WeaponProjectile::Draw()
{
  image->Draw(GetPosition());

  int tmp = GetTotalTimeout();

  if (cfg.timeout && tmp != 0)
  {
    tmp -= (int)((Time::GetInstance()->Read() - begin_time) / 1000);
    if (tmp >= 0)
    {
      std::ostringstream ss;
      ss << tmp ;
      int txt_x = GetX() + GetWidth() / 2;
      int txt_y = GetY() - GetHeight();
      (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop( Point2i(txt_x, txt_y) - camera.GetPosition(),
      ss.str(), white_color);
    }
  }
}

bool WeaponProjectile::IsImmobile() const
{
  if(explode_with_timeout && begin_time + GetTotalTimeout() * 1000 > Time::GetInstance()->Read())
    return false;
  return PhysicalObj::IsImmobile();
}

// projectile explode and signal to the launcher the collision
void WeaponProjectile::SignalObjectCollision(PhysicalObj * obj)
{
  assert (obj != NULL);
  MSG_DEBUG ("weapon.projectile", "SignalObjectCollision");
  if (explode_colliding_character)
    Explosion();
}

// projectile explode when hiting the ground
void WeaponProjectile::SignalGroundCollision()
{
  MSG_DEBUG ("weapon.projectile", "SignalGroundCollision");
  if (explode_with_collision)
    Explosion();
}

// Default behavior : signal to launcher a collision and explode
void WeaponProjectile::SignalCollision()
{
  MSG_DEBUG ("weapon.projectile", "SignalCollision");
  if (launcher != NULL && !launcher->ignore_collision_signal) launcher->SignalProjectileCollision();
}

// Default behavior : signal to launcher projectile is drowning
void WeaponProjectile::SignalDrowning()
{
  MSG_DEBUG ("weapon.projectile", "SignalDrowning");
  PhysicalObj::SignalDrowning();
  if (launcher != NULL && !launcher->ignore_drowning_signal)
    launcher->SignalProjectileDrowning();
}

// Signal a ghost state
void WeaponProjectile::SignalGhostState(bool)
{
  MSG_DEBUG ("weapon.projectile", "SignalGhostState");
  if (launcher != NULL && !launcher->ignore_ghost_state_signal)
    launcher->SignalProjectileGhostState();
  camera.SetCloseFollowing(false);
}

void WeaponProjectile::SignalOutOfMap()
{
  MSG_DEBUG ("weapon.projectile", "SignalOutOfMap");
}

// Implement it in subclass to randomize fire
void WeaponProjectile::RandomizeShoot(double &angle,double &strength)
{
}

// the projectile explode and signal the explosion to launcher
void WeaponProjectile::Explosion()
{
  MSG_DEBUG("weapon.projectile", "Explosion");
  DoExplosion();
  SignalExplosion();
  Ghost();
}

void WeaponProjectile::SignalExplosion()
{
  MSG_DEBUG ("weapon.projectile", "SignalExplosion");
  if (launcher != NULL && !launcher->ignore_explosion_signal)
    launcher->SignalProjectileExplosion();
}

void WeaponProjectile::DoExplosion()
{
  Point2i pos = GetCenter();
  ApplyExplosion (pos, cfg);
}

void WeaponProjectile::IncrementTimeOut()
{
  if (cfg.allow_change_timeout && GetTotalTimeout()<(int)cfg.timeout*2)
    m_timeout_modifier += 1 ;
}

void WeaponProjectile::DecrementTimeOut()
{
  // -1s for grenade timout. 1 is min.
  if (cfg.allow_change_timeout && GetTotalTimeout()>1)
    m_timeout_modifier -= 1 ;
}

void WeaponProjectile::SetTimeOut(int timeout)
{
  if (cfg.allow_change_timeout && timeout <= (int)cfg.timeout*2 && timeout >= 1)
    m_timeout_modifier = timeout - cfg.timeout ;
}

void WeaponProjectile::ResetTimeOut()
{
  m_timeout_modifier = 0 ;
}

int WeaponProjectile::GetTotalTimeout() const
{
  return (int)(cfg.timeout)+m_timeout_modifier;
}

// Signal a projectile timeout and explode
void WeaponProjectile::SignalTimeout()
{
  if (launcher != NULL && !launcher->ignore_timeout_signal) launcher->SignalProjectileTimeout();
  if (explode_with_timeout) Explosion();
}

//Public function which let know if changing timeout is allowed.
bool WeaponProjectile::change_timeout_allowed()
{
  return cfg.allow_change_timeout;
}

//-----------------------------------------------------------------------------

WeaponLauncher::WeaponLauncher(Weapon_type type,
                               const std::string &id,
                               EmptyWeaponConfig * params,
                               weapon_visibility_t visibility) :
    Weapon(type, id, params, visibility)
{
  projectile = NULL;
  nb_active_projectile = 0;
  missed_shots = 0;
  announce_missed_shots = true;
  ignore_timeout_signal = false;
  ignore_collision_signal = false;
  ignore_explosion_signal = false;
  ignore_ghost_state_signal = false;
  ignore_drowning_signal = false;
}

WeaponLauncher::~WeaponLauncher()
{
  if (projectile) delete projectile;
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
  projectile = NULL;
  ReloadLauncher();
  return true;
}

bool WeaponLauncher::ReloadLauncher()
{
  if (projectile) return false;
  projectile = GetProjectileInstance();
  return true;
}

// Direct Explosion when pushing weapon to max power !
void WeaponLauncher::DirectExplosion()
{
  Point2i pos = ActiveCharacter().GetCenter();
  ApplyExplosion (pos, cfg());
}

// Signal that a projectile explosion
void WeaponLauncher::SignalProjectileExplosion()
{
  DecActiveProjectile();
  m_is_active = false;
}

// Signal that a projectile fired by this weapon has hit something (ground, character etc)
void WeaponLauncher::SignalProjectileCollision()
{
  m_is_active = false;
}

// Signal a ghost state
void WeaponLauncher::SignalProjectileGhostState()
{
  m_is_active = false;
}

// Signal a projectile timeout (for exemple: grenade, disco grenade ... etc.)
void WeaponLauncher::SignalProjectileTimeout()
{
  m_is_active = false;
}

// Signal a projectile is drowning
void WeaponLauncher::SignalProjectileDrowning()
{
  m_is_active = false;
}

// Keep the total amount of active projectile
void WeaponLauncher::IncActiveProjectile()
{
  ++nb_active_projectile;
}

void WeaponLauncher::DecActiveProjectile()
{
  --nb_active_projectile;
}

// Call by the object list class to refresh the weapon's state
void WeaponLauncher::Refresh()
{
}

void WeaponLauncher::Draw()
{
  //Display timeout for projectil if can be changed.
  if (projectile->change_timeout_allowed())
  {
    if( IsInUse() ) //Do not display after launching.
      return;

    int tmp = projectile->GetTotalTimeout();
    std::ostringstream ss;
    ss << tmp;
    ss << "s";
    int txt_x = ActiveCharacter().GetX() + ActiveCharacter().GetWidth() / 2;
    int txt_y = ActiveCharacter().GetY() - ActiveCharacter().GetHeight();
    (*Font::GetInstance(Font::FONT_SMALL)).WriteCenterTop( Point2i(txt_x, txt_y) - camera.GetPosition(),
    ss.str(), white_color);
  }

  Weapon::Draw();

#ifdef DEBUG_EXPLOSION_CONFIG
  ExplosiveWeaponConfig* cfg = dynamic_cast<ExplosiveWeaponConfig*>(extra_params);
  if( cfg != NULL )
  {
    Point2i p = ActiveCharacter().GetHandPosition() - camera.GetPosition();
    // Red color for the blast range (should be superior to the explosion_range)
    AppWormux::GetInstance()->video.window.CircleColor(p.x, p.y, (int)cfg->blast_range, c_red);
    // Yellow color for the blast range (should be superior to the explosion_range)
    AppWormux::GetInstance()->video.window.CircleColor(p.x, p.y, (int)cfg->explosion_range, c_black);
  }
  AppWormux::GetInstance()->video.window.CircleColor(GetGunHolePosition().x-camera.GetPositionX(), GetGunHolePosition().y-camera.GetPositionY(), 5, c_black);
#endif
}

void WeaponLauncher::p_Select()
{
  missed_shots = 0;
  if (projectile->change_timeout_allowed())
  {
    projectile->ResetTimeOut();
  }
  Weapon::p_Select();
}

void WeaponLauncher::IncMissedShots()
{
  missed_shots++;
  if(announce_missed_shots)
    GameMessages::GetInstance()->Add (_("Your shot has missed!"));
}

void WeaponLauncher::HandleKeyReleased_Num1()
{
  projectile->SetTimeOut(1);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num2()
{
  projectile->SetTimeOut(2);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num3()
{
  projectile->SetTimeOut(3);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num4()
{
  projectile->SetTimeOut(4);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num5()
{
  projectile->SetTimeOut(5);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num6()
{
  projectile->SetTimeOut(6);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num7()
{
  projectile->SetTimeOut(7);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num8()
{
  projectile->SetTimeOut(8);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Num9()
{
  projectile->SetTimeOut(9);
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_Less()
{
  projectile->IncrementTimeOut();
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleKeyReleased_More()
{
  projectile->DecrementTimeOut();
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::NetworkSetTimeoutProjectile()
{
  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_WEAPON_SET_TIMEOUT, 
						     projectile->m_timeout_modifier));
}

void WeaponLauncher::HandleMouseWheelUp()
{
  projectile->IncrementTimeOut();
  NetworkSetTimeoutProjectile();
}

void WeaponLauncher::HandleMouseWheelDown()
{
  projectile->DecrementTimeOut();
  NetworkSetTimeoutProjectile();
}

ExplosiveWeaponConfig& WeaponLauncher::cfg()
{
  return static_cast<ExplosiveWeaponConfig&>(*extra_params);
}
