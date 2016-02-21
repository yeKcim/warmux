/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Rail Gun. In addition to Snipe Rifle, change the collision handling.
 *****************************************************************************/

#include "weapon/weapon_cfg.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "weapon/rail_gun.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"

#define RAIL_BULLET_SPEED  40

class RailBullet : public WeaponBullet
{
public:
  RailBullet(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher);

protected:
  ParticleEngine particle;
  void ShootSound();
  void SignalDrowning();

  void SignalGroundCollision(const Point2d& speed_before, const Double& contactAngle);
  void SignalObjectCollision(const Point2d& my_speed_before,
                             PhysicalObj * obj,
                             const Point2d& /*obj_speed*/);
};


RailBullet::RailBullet(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher)
  : WeaponBullet("rail_bullet", cfg, p_launcher)
{
  explode_colliding_character = false;
  m_is_fire = true;
  SetCollisionModel(true, true, true, true /* very important last*/);
}

void RailBullet::ShootSound()
{
  JukeBox::GetInstance()->Play("default","weapon/gun");
}

void RailBullet::SignalDrowning()
{
  launcher->IncMissedShots();
  Ghost();
}

void RailBullet::SignalGroundCollision(const Point2d& speed_before, const Double& contactAngle)
{
  // Change that sound?
  //JukeBox::GetInstance()->Play("default", "weapon/ricoche1");
  WeaponProjectile::SignalGroundCollision(speed_before, contactAngle);
  launcher->IncMissedShots();
}

void RailBullet::SignalObjectCollision(const Point2d& /*my_speed_before*/,
                                       PhysicalObj * obj,
                                       const Point2d& /*obj_speed*/)
{
  int delta = (int)cfg.damage;
  obj->SetEnergyDelta(-delta, &ActiveCharacter());
  static_cast<RailGun*>(launcher)->IncreaseHits();
}

//---------------------------------------------------------------------------

RailGun::RailGun()
  : BaseSnipeRifle(WEAPON_RAIL_GUN, "rail_gun")
  , hits(0)
{
  UpdateTranslationStrings();
  ReloadLauncher();
}

void RailGun::UpdateTranslationStrings()
{
  m_name = _("Rail Gun");
  m_help = _("Shoots all players in a straight line!");
}

WeaponProjectile * RailGun::GetProjectileInstance()
{
  return new RailBullet(cfg(), this);
}

bool RailGun::p_Shoot()
{
  if (IsOnCooldownFromShot())
    return false;

  hits = 0;
  projectile->Shoot(RAIL_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

void RailGun::IncMissedShots()
{
  if (!hits) {
    WeaponLauncher::IncMissedShots();
  } else if (hits > 1) {
    // The shot efficiency was improved by having more than one target hit
    Weapon::Message(Format(_("Woah! Combo of %u!"), hits));
  }
  hits = 0;
}

std::string RailGun::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  // The weapon bullet goes through characters, so it's like you're having a BBQ out of them
  return Format(ngettext(
            "%s team has won %u rail gun! It's skewer time!",
            "%s team has won %u rail guns! It's skewer time!",
            items_count), TeamName, items_count);
}
