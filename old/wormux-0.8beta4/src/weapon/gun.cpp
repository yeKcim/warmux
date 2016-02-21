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
 * gun Weapon : The bullet made a great hole if we hit the ground or made damage
 * if we hit a character.
 *****************************************************************************/

#include "weapon/gun.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include <sstream>
#include "map/map.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

const uint GUN_BULLET_SPEED = 20;

class GunBullet : public WeaponBullet
{
  public:
    GunBullet(ExplosiveWeaponConfig& cfg,
              WeaponLauncher * p_launcher);
  protected:
    void ShootSound();
};

GunBullet::GunBullet(ExplosiveWeaponConfig& cfg,
                     WeaponLauncher * p_launcher) :
  WeaponBullet("gun_bullet", cfg, p_launcher)
{
}

void GunBullet::ShootSound()
{
  jukebox.Play("share","weapon/gun");
}

//-----------------------------------------------------------------------------

Gun::Gun() : WeaponLauncher(WEAPON_GUN, "gun", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = RIFLE;
  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_fire"));
  m_weapon_fire->EnableRotationCache(32);
  ReloadLauncher();
}

void Gun::UpdateTranslationStrings()
{
  m_name = _("Gun");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

WeaponProjectile * Gun::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new GunBullet(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

bool Gun::p_Shoot()
{
  if (IsInUse())
    return false;

  projectile->Shoot (GUN_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

std::string Gun::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u gun!",
            "%s team has won %u guns!",
            items_count), TeamName, items_count);
}

