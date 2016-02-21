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
 * riot bomb : fire a rocket that doesn't make any damage but makes a big
 * hole with angle and force
 *****************************************************************************/

#include "weapon/riot_bomb.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "game/config.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

// Roquette du bazooka
class RiotBombRocket : public WeaponProjectile
{
  ParticleEngine smoke_engine;
  SoundSample flying_sound;
public:
  RiotBombRocket(ExplosiveWeaponConfig& cfg,
                   WeaponLauncher * p_launcher);
  void Refresh();
  void Explosion();
  void Shoot(double strength);
protected:
  void SignalOutOfMap();
  void SignalDrowning();
};

RiotBombRocket::RiotBombRocket(ExplosiveWeaponConfig& cfg,
                                   WeaponLauncher * p_launcher) :
  WeaponProjectile ("riot_rocket", cfg, p_launcher)
{
  explode_colliding_character = true;
}

void RiotBombRocket::Shoot(double strength)
{
  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  flying_sound.Play("share","weapon/riotbomb_rocket_flying", -1);

  WeaponProjectile::Shoot(strength);
}

void RiotBombRocket::Refresh()
{
  WeaponProjectile::Refresh();
  if(!IsDrowned())
  {
    image->SetRotation_rad(GetSpeedAngle());
    smoke_engine.AddPeriodic(Point2i(GetX() + GetWidth() / 2,
                                     GetY() + GetHeight()/ 2), particle_DARK_SMOKE, false, -1, 2.0);
  }
  else
  {
    image->SetRotation_rad(M_PI_2);
  }
}

void RiotBombRocket::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The rocket has left the battlefield..."));
  WeaponProjectile::SignalOutOfMap();

  flying_sound.Stop();
}

void RiotBombRocket::SignalDrowning()
{
  smoke_engine.Stop();
  WeaponProjectile::SignalDrowning();

  flying_sound.Stop();
}

void RiotBombRocket::Explosion()
{
  WeaponProjectile::Explosion();

  flying_sound.Stop();
}

//-----------------------------------------------------------------------------

RiotBomb::RiotBomb() :
  WeaponLauncher(WEAPON_RIOT_BOMB, "riot_bomb", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = HEAVY;
  ReloadLauncher();
}

void RiotBomb::UpdateTranslationStrings()
{
  m_name = _("Riot Bomb");
  m_help = _("Initial fire angle : Up/Down\nFire : keep space key pressed until the desired strength\nan ammo per turn");
}

WeaponProjectile * RiotBomb::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new RiotBombRocket(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

std::string RiotBomb::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u riot bomb!",
            "%s team has won %u riot bombs!",
            items_count), TeamName, items_count);
}

