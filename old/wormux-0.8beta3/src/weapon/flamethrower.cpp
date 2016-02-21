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
 * Flame Thrower. Don't fire flame one by one but with burst fire (like
 * the submachine gun :)
 * The hack in order to firing multiple flames at once consist in using a
 * std::list of projectile and override the Refresh and the management of the keys
 *
 * This weapon is mostly copy-paste from submachine gun
 *****************************************************************************/

#include <sstream>

#include "character/character.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

#include "explosion.h"
#include "flamethrower.h"
#include "weapon_cfg.h"

const uint    FLAMETHROWER_BULLET_SPEED       = 5;
const uint    FLAMETHROWER_TIME_BETWEEN_SHOOT = 40;
const double  FLAMETHROWER_RANDOM_ANGLE       = 0.06;

class FlameThrowerBullet : public WeaponBullet
{
  public:
    FlameThrowerBullet(ExplosiveWeaponConfig& cfg,
		       WeaponLauncher * p_launcher);
  protected:
    ParticleEngine particle;
    void ShootSound();
    void RandomizeShoot(double &angle, double &strength);
    void DoExplosion();
};


FlameThrowerBullet::FlameThrowerBullet(ExplosiveWeaponConfig& cfg,
				       WeaponLauncher * p_launcher) :
  WeaponBullet("flamethrower_bullet", cfg, p_launcher), particle(40)
{
  explode_colliding_character = true;
}

void FlameThrowerBullet::RandomizeShoot(double &angle, double &/*strength*/)
{
  angle += M_PI * randomSync.GetDouble(-FLAMETHROWER_RANDOM_ANGLE, FLAMETHROWER_RANDOM_ANGLE);
}

void FlameThrowerBullet::ShootSound()
{
  jukebox.Play("share", "weapon/flamethrower");
}

void FlameThrowerBullet::DoExplosion()
{
  Point2i pos=GetPosition();
  particle.AddNow(pos, 1, particle_FIRE, true, 0, 1);
  particle.AddNow(pos, 2, particle_SMOKE, true, 0, 1);
}

//-----------------------------------------------------------------------------

FlameThrower::FlameThrower() : WeaponLauncher(WEAPON_FLAMETHROWER, "flamethrower", new ExplosiveWeaponConfig())
{
  m_name = _("Flame Thrower");
  m_category = RIFLE;

  ignore_collision_signal = true;
  ignore_explosion_signal = true;
  ignore_ghost_state_signal = true;
  ignore_drowning_signal = true;
  ignore_going_out_of_water_signal = true;
  announce_missed_shots = false;
  m_time_between_each_shot = FLAMETHROWER_TIME_BETWEEN_SHOOT;

  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile, m_id+"_fire"));
  m_weapon_fire->EnableRotationCache(32);

  ReloadLauncher();
}

// Return a projectile instance for the submachine gun
WeaponProjectile * FlameThrower::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new FlameThrowerBullet(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

void FlameThrower::IncMissedShots()
{
  if (missed_shots + 1 == ReadInitialNbUnit())
    announce_missed_shots = true;
  WeaponLauncher::IncMissedShots();
}

bool FlameThrower::p_Shoot()
{
  projectile->Shoot(FLAMETHROWER_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();

  Point2i pos = ActiveCharacter().GetHandPosition();
  double angle =  - M_PI_2 - ActiveCharacter().GetDirection()
               * (float)(Time::GetInstance()->Read() % 100) * M_PI_4 / 100.0;
  particle.AddNow(pos, 1, particle_SMOKE, true, angle,
                  5.0 + (Time::GetInstance()->Read() % 6));
  announce_missed_shots = false;
  return true;
}

// Overide regular Refresh method
void FlameThrower::RepeatShoot()
{
  uint tmp = Time::GetInstance()->Read();
  uint time = tmp - m_last_fire_time;

  if (time >= m_time_between_each_shot)
    {
      NewActionWeaponShoot();
      m_last_fire_time = tmp;
    }
}

void FlameThrower::HandleKeyRefreshed_Shoot(bool /*shift*/)
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
}

std::string FlameThrower::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u flame-thrower!",
            "%s team has won %u flame-throwers!",
            items_count), TeamName, items_count);
}

