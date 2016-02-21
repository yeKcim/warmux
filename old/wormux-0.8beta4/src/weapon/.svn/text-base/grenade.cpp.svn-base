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
 * Arme grenade :
 * Explose au bout de quelques secondes
 *****************************************************************************/

#include "weapon/grenade.h"
#include "weapon/weapon_cfg.h"

//-----------------------------------------------------------------------------
#include <sstream>
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "weapon/explosion.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"
#include "object/objects_list.h"
//-----------------------------------------------------------------------------

Grenade::Grenade(ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile ("grenade", cfg, p_launcher)
{
  m_rebound_sound = "weapon/grenade_bounce";
  explode_with_collision = false;
}

//-----------------------------------------------------------------------------

void Grenade::Refresh()
{
  WeaponProjectile::Refresh();
  image->SetRotation_rad(GetSpeedAngle());
}

//-----------------------------------------------------------------------------

void Grenade::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The grenade left the battlefield before exploding"));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

GrenadeLauncher::GrenadeLauncher() :
  WeaponLauncher(WEAPON_GRENADE, "grenade", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  UpdateTranslationStrings();

  m_category = THROW;
  m_allow_change_timeout = true;
  ReloadLauncher();
}

void GrenadeLauncher::UpdateTranslationStrings()
{
  m_name = _("Grenade");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

WeaponProjectile * GrenadeLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new Grenade(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

std::string GrenadeLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u grenade!",
            "%s team has won %u grenades!",
            items_count), TeamName, items_count);
}


