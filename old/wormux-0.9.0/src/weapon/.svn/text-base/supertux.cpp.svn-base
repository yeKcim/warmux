/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Weapon Supertux : Look ! it's the famous flying magic pinguin !
 *****************************************************************************/

#include "weapon/supertux.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "game/config.h"
#include "game/game.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/math_tools.h"
#include "tool/xml_document.h"

const uint time_delta = 40;
const uint animation_deltat = 50;

class SuperTuxWeaponConfig : public ExplosiveWeaponConfig
{
  public:
    uint speed;
    SuperTuxWeaponConfig();
    virtual void LoadXml(const xmlNode* elem);
};
//-----------------------------------------------------------------------------

class SuperTux : public WeaponProjectile
{
  private:
    bool swimming; // Supertux is not in the air, it is swimming!
    ParticleEngine particle_engine;
    double angle_rad;
    SoundSample flying_sound;

  public:
    uint speed;
    uint time_now;
    uint time_next_action;
    uint last_move;

    SuperTux(SuperTuxWeaponConfig& cfg,
             WeaponLauncher * p_launcher);
    void Refresh();

    inline void SetAngle(double angle) {angle_rad = angle;}
    void turn_left();
    void turn_right();
    void Shoot(double strength);
    virtual void Explosion();
    virtual void SignalDrowning();
    virtual void SignalGoingOutOfWater();
  protected:
    void SignalOutOfMap();
};

//-----------------------------------------------------------------------------

SuperTux::SuperTux(SuperTuxWeaponConfig& cfg,
                   WeaponLauncher * p_launcher) :
  WeaponProjectile ("supertux", cfg, p_launcher),
  particle_engine(40)
{
  swimming = false;
  explode_colliding_character = true;
  explode_with_timeout = true;

  camera_follow_closely = true;

  SetSize(image->GetSize());
  SetTestRect(1, 1, 2, 2);
}

void SuperTux::Shoot(double strength)
{
  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  swimming = false;
  flying_sound.Play("default","weapon/supertux_flying", -1);

  if (strength == 0)
    strength = 1; // To please the camera with follow_closely

  WeaponProjectile::Shoot(strength);
  angle_rad = ActiveCharacter().GetFiringAngle();

  Time * global_time = Time::GetInstance();
  time_next_action = global_time->Read();
  last_move = global_time->Read();
  StartTimeout();
}

void SuperTux::Refresh()
{
  WeaponProjectile::Refresh();

  image->SetRotation_rad(angle_rad + M_PI_2);
  if ((last_move+animation_deltat)<Time::GetInstance()->Read())
  {
    SetExternForce(static_cast<SuperTuxWeaponConfig&>(cfg).speed, angle_rad);
    image->Update();
    last_move = Time::GetInstance()->Read();
  }

  if (!swimming)
    particle_engine.AddPeriodic(GetPosition(), particle_STAR, false, angle_rad, 0);
  // else
  // particle_engine.AddPeriodic(GetPosition(), particle_WATERBUBBLE, false, angle_rad, 0);

  Camera::GetInstance()->FollowObject(this, camera_follow_closely);

}

void SuperTux::turn_left()
{
  time_now = Time::GetInstance()->Read();
  if (time_next_action<time_now)
    {
      time_next_action=time_now + time_delta;
      angle_rad = angle_rad - M_PI / 12;
    }
}

void SuperTux::turn_right()
{
  time_now = Time::GetInstance()->Read();
  if (time_next_action<time_now)
    {
      time_next_action=time_now + time_delta;
      angle_rad = angle_rad + M_PI / 12;
    }
}

void SuperTux::SignalDrowning()
{
  swimming = true;
  WeaponProjectile::SignalDrowning();
  flying_sound.Stop();
  flying_sound.Play("default","weapon/supertux_swimming", -1);
}

void SuperTux::SignalGoingOutOfWater()
{
  swimming = false;
  WeaponProjectile::SignalGoingOutOfWater();
  flying_sound.Stop();
  flying_sound.Play("default","weapon/supertux_flying", -1);
}

void SuperTux::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("Bye bye Tux..."));
  WeaponProjectile::SignalOutOfMap();

  flying_sound.Stop();
}

void SuperTux::Explosion()
{
  WeaponProjectile::Explosion();

  flying_sound.Stop();
}

//-----------------------------------------------------------------------------

SuperTuxWeaponConfig::SuperTuxWeaponConfig()
{
  speed = 2;
}

void SuperTuxWeaponConfig::LoadXml(const xmlNode* elem)
{
  ExplosiveWeaponConfig::LoadXml (elem);
  XmlReader::ReadUint(elem, "speed", speed);
}

//-----------------------------------------------------------------------------

TuxLauncher::TuxLauncher() :
  WeaponLauncher(WEAPON_SUPERTUX, "tux", new SuperTuxWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = SPECIAL;
  current_tux = NULL;
  tux_death_time = 0;
  ReloadLauncher();

  // unit will be used when the supertux disappears
  use_unit_on_first_shoot = false;

  // Supertux doesn't drown! it swims!
  ignore_drowning_signal = true;
}

void TuxLauncher::UpdateTranslationStrings()
{
  m_name = _("SuperTux");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

WeaponProjectile * TuxLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new SuperTux(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

bool TuxLauncher::p_Shoot ()
{
  if (current_tux || tux_death_time)
    return false;

  current_tux = static_cast<SuperTux *>(projectile);
  tux_death_time = 0;
  bool r = WeaponLauncher::p_Shoot();

  return r;
}

void TuxLauncher::Refresh()
{
  if (current_tux) {
    const LRMoveIntention * lr_move_intention = ActiveCharacter().GetLastLRMoveIntention();
    if (lr_move_intention) {
      if (lr_move_intention->GetDirection() == DIRECTION_LEFT)
        current_tux->turn_left();
      else
        current_tux->turn_right();
    }
  } else {
    if (tux_death_time && tux_death_time + 2000 < Time::GetInstance()->Read()) {
      UseAmmoUnit();
      tux_death_time = 0;
    }
  }
}

bool TuxLauncher::ShouldBeDrawn()
{
  return !(current_tux || tux_death_time);
}

void TuxLauncher::SignalEndOfProjectile()
{
  if (!current_tux)
    return;

  current_tux = NULL;
  tux_death_time = Time::GetInstance()->Read();
}

void TuxLauncher::StartShooting()
{
  if (current_tux || tux_death_time)
    return;

  Weapon::StartShooting();
}


void TuxLauncher::StopShooting()
{
  if (current_tux) {
    current_tux->Explosion();
    return;
  } else if (!tux_death_time)
    Weapon::StopShooting();
}

bool TuxLauncher::IsPreventingLRMovement()
{
  return (current_tux || tux_death_time);
}

bool TuxLauncher::IsPreventingJumps()
{
  return (current_tux || tux_death_time);
}

bool TuxLauncher::IsPreventingWeaponAngleChanges()
{
  return (current_tux || tux_death_time);
}

std::string TuxLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u tux launcher! Never seen a flying penguin?",
            "%s team has won %u tux launchers! Never seen a flying penguin?",
            items_count), TeamName, items_count);
}

SuperTuxWeaponConfig& TuxLauncher::cfg()
{
  return static_cast<SuperTuxWeaponConfig&>(*extra_params);
}
