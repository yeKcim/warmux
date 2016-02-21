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
 * FootBomb Bomb : similar to cluster bomb, but much stronger
 *****************************************************************************/

#include "weapon/footbomb.h"
#include "weapon/weapon_cfg.h"
#include <sstream>
#include <WARMUX_types.h>
#include "weapon/explosion.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/xml_document.h"
#include "game/game_time.h"

class FootBombConfig : public ExplosiveWeaponConfig
{
public:
  uint nb_fragments;
  uint nb_recursions;
  Double nb_angle_dispersion;
  Double nb_min_speed;
  Double nb_max_speed;

  FootBombConfig()
  {
    push_back(new UintConfigElement("nb_fragments", &nb_fragments, 2, 1, 4));
    push_back(new UintConfigElement("nb_recursions", &nb_recursions, 2));
    push_back(new DoubleConfigElement("nb_angle_dispersion", &nb_angle_dispersion, 0));
    push_back(new DoubleConfigElement("nb_min_speed", &nb_min_speed, 0));
    push_back(new DoubleConfigElement("nb_max_speed", &nb_max_speed, 0));
  }
};

class FootBomb : public WeaponProjectile
{
  uint m_recursions;

public:
  FootBomb(FootBombConfig& cfg,
              WeaponLauncher * p_launcher);
  void Refresh();
  virtual void SetEnergyDelta(int delta, bool do_report = true);
  void Shoot(const Point2i & pos, Double strength, Double angle, int recursions);

protected:
  void DoExplosion();
  void SignalOutOfMap();
};

//-----------------------------------------------------------------------------

FootBomb::FootBomb(FootBombConfig& cfg, WeaponLauncher * p_launcher)
  : WeaponProjectile("footbomb", cfg, p_launcher)
{
  m_rebound_sound = "weapon/footbomb_bounce";
  explode_with_collision = false;
  explode_with_timeout = true;
  m_recursions = cfg.nb_recursions;

  // ensure that all football bomb have the same UniqueId
  // it prevents bomb to be blasted by explosion of its parent (fix network bug #11154)
  SetUniqueId("footbomb");
}

void FootBomb::Shoot(const Point2i & pos, Double strength, Double angle, int recursions)
{
  m_recursions = recursions;
  SetCollisionModel(true, true, false ); // a bit hackish...
  // we do need to collide with objects, but if we allow for this, the clusters
  // will explode on spawn (because of colliding with each other)

  StartTimeout();
  Camera::GetInstance()->FollowObject(this);
  ResetConstants();
  SetXY( pos );
  SetSpeed(strength, angle);
}

void FootBomb::Refresh()
{
  WeaponProjectile::Refresh();
//  image->SetRotation_rad(GetSpeedAngle());
  if (IsMoving()) {
#define ROTATIONS_PER_SECOND  4
    image->SetRotation_rad((ROTATIONS_PER_SECOND*GetMSSinceTimeoutStart()*2) * PI / 1000 );
  }
}

void FootBomb::SignalOutOfMap()
{
  Weapon::Message(_("The Football Bomb has left the battlefield before it could explode."));
  WeaponProjectile::SignalOutOfMap();
}

void FootBomb::DoExplosion()
{
  WeaponProjectile::DoExplosion();

  if (!m_recursions)
    return;

  FootBombConfig &config = static_cast<FootBombConfig &>(cfg);
  Double half_angle_range = config.nb_angle_dispersion * PI / 180;
  Point2i pos = GetPosition();

  for (uint i=0; i<config.nb_fragments; ++i) {
    Double angle = -PI / 2; // this angle is "upwards" here
    Double cluster_deviation = RandomSync().GetDouble(-half_angle_range, half_angle_range);
    Double speed = RandomSync().GetDouble(config.nb_min_speed, config.nb_max_speed);

    FootBomb *cluster = new FootBomb(config, launcher);
    cluster->Shoot(pos, speed, angle+cluster_deviation, m_recursions-1);
    cluster->SetTimeOut(cfg.timeout + m_timeout_modifier);

    ObjectsList::GetRef().AddObject(cluster);
  }
}

void FootBomb::SetEnergyDelta(int /* delta */, bool /* do_report */){};

//-----------------------------------------------------------------------------

FootBombLauncher::FootBombLauncher() :
  WeaponLauncher(WEAPON_FOOTBOMB, "footbomb", new FootBombConfig())
{
  UpdateTranslationStrings();

  m_category = THROW;
  ignore_collision_signal = true;
  ReloadLauncher();
}

void FootBombLauncher::UpdateTranslationStrings()
{
  m_name = _("Football Bomb");
  m_help = _("Timeout: Mouse wheel or Page Up/Down\nAngle: Up/Down\nFire: Press space until desired strength is reached\nOne ammo per turn");
}

WeaponProjectile * FootBombLauncher::GetProjectileInstance()
{
  return new FootBomb(cfg(), this);
}

FootBombConfig& FootBombLauncher::cfg()
{
  return static_cast<FootBombConfig&>(*extra_params);
}

std::string FootBombLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext("%s team has won %u football bomb!",
                         "%s team has won %u football bombs!",
                         items_count), TeamName, items_count);
}
