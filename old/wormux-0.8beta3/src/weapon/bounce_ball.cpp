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
 * Bounce ball:
 * bounce since it has not collide a character
 *****************************************************************************/

#include "weapon/bounce_ball.h"
#include "weapon/weapon_cfg.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
#include "weapon/explosion.h"
//-----------------------------------------------------------------------------

class BounceBall : public WeaponProjectile
{
  public:
    BounceBall(ExplosiveWeaponConfig& cfg,
               WeaponLauncher * p_launcher);
    void Refresh();
  protected:
    void SignalOutOfMap();
};


BounceBall::BounceBall(ExplosiveWeaponConfig& cfg,
                       WeaponLauncher * p_launcher) :
  WeaponProjectile ("bounce_ball", cfg, p_launcher)
{
  m_rebound_sound = "weapon/grenade_bounce";
  explode_colliding_character = true;
  explode_with_collision = false;
}

//-----------------------------------------------------------------------------

void BounceBall::Refresh()
{
  WeaponProjectile::Refresh();
  // rotation of ball image...
  image->SetRotation_rad(GetSpeedAngle());
}


//-----------------------------------------------------------------------------

void BounceBall::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The ball left the battlefield before exploding"));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

BounceBallLauncher::BounceBallLauncher() :
  WeaponLauncher(WEAPON_BOUNCE_BALL, "bounce_ball", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Bounce Ball");
  m_help = _("Timeout : Wheel mouse or Page Up/Down\nAngle : Up/Down\nFire : space key\nan ammo per turn");
  m_category = DUEL;
  ReloadLauncher();
}

WeaponProjectile * BounceBallLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new BounceBall(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

bool BounceBallLauncher::p_Shoot ()
{
  if (max_strength == 0)
    projectile->Shoot (10);
  else
    projectile->Shoot (m_strength);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

std::string BounceBallLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u bounce ball! Boing!",
            "%s team has won %u bounce balls! Boing!",
            items_count), TeamName, items_count);
}


