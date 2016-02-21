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
 * Submachine gun. Don't fire bullet one by one but with burst fire (like
 * a submachine gun :)
 * The hack in order to firing multiple bullet at once consist in using a
 * std::list of projectile and override the Refresh and the management of the keys
 *****************************************************************************/

#include <sstream>
#include "map/map.h"
#include "game/time.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "interface/game_msg.h"
#include "interface/game_msg.h"
#include "weapon/explosion.h"
#include "weapon/submachine_gun.h"
#include "network/randomsync.h"

const uint    SUBMACHINE_BULLET_SPEED       = 30;
const double  SUBMACHINE_TIME_BETWEEN_SHOOT = 70;
const double  SUBMACHINE_RANDOM_ANGLE       = 0.01;

SubMachineGunBullet::SubMachineGunBullet(ExplosiveWeaponConfig& cfg,
                                         WeaponLauncher * p_launcher) :
  WeaponBullet("m16_bullet", cfg, p_launcher)
{
  camera_follow_closely = false;
}

void SubMachineGunBullet::RandomizeShoot(double &angle,double &strength)
{
  angle += M_PI * randomSync.GetDouble(-SUBMACHINE_RANDOM_ANGLE,SUBMACHINE_RANDOM_ANGLE);
}

void SubMachineGunBullet::ShootSound()
{
  jukebox.Play("share", "weapon/m16");
}

//-----------------------------------------------------------------------------

SubMachineGun::SubMachineGun() : WeaponLauncher(WEAPON_SUBMACHINE_GUN, "m16", new ExplosiveWeaponConfig())
{
  m_name = _("Submachine Gun");
  m_category = RIFLE;

  ignore_collision_signal = true;
  ignore_explosion_signal = true;
  ignore_ghost_state_signal = true;
  ignore_drowning_signal = true;
  announce_missed_shots = false;

  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_fire"));
  m_weapon_fire->EnableRotationCache(32);

  ReloadLauncher();
}

// Return a projectile instance for the submachine gun
WeaponProjectile * SubMachineGun::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new SubMachineGunBullet(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

void SubMachineGun::IncMissedShots()
{
  if(missed_shots + 1 == ReadInitialNbUnit())
    announce_missed_shots = true;
  WeaponLauncher::IncMissedShots();
}

bool SubMachineGun::p_Shoot()
{
  projectile->Shoot(SUBMACHINE_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();

  Point2i pos = ActiveCharacter().GetHandPosition();
  double angle =  - M_PI_2 - ActiveCharacter().GetDirection()
                * (float)(Time::GetInstance()->Read() % 100) * M_PI_4 / 100.0;
  particle.AddNow(pos, 1, particle_BULLET, true, angle,
  	                   5.0 + (Time::GetInstance()->Read() % 6));

  announce_missed_shots = false;
  return true;
}

void SubMachineGun::p_Deselect()
{
  m_is_active = false;
}

// Overide regular Refresh method
void SubMachineGun::RepeatShoot()
{
  uint tmp = Time::GetInstance()->Read();
  uint time = tmp - m_last_fire_time;

  if (time >= SUBMACHINE_TIME_BETWEEN_SHOOT)
    {
      NewActionWeaponShoot();
      m_last_fire_time = tmp;
    }
}

void SubMachineGun::SignalTurnEnd()
{
  // It's too late !
  m_is_active = false;
}

void SubMachineGun::HandleKeyPressed_Shoot()
{
  HandleKeyRefreshed_Shoot();
}

void SubMachineGun::HandleKeyRefreshed_Shoot()
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
}

void SubMachineGun::HandleKeyReleased_Shoot()
{
  m_is_active = false;
}

std::string SubMachineGun::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u submachine gun!",
            "%s team has won %u submachine guns!",
            items_count), TeamName, items_count);
}

