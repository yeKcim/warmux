/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
#include "network/network.h"
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
  begin_time = global_time->Read();
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

  if(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI())
  {
    Action a(Action::ACTION_WEAPON_SUPERTUX);
    a.Push(0); // to ask for a position refresh
    a.Push(angle_rad);
    a.Push(GetPos());
    Network::GetInstance()->SendActionToAll(a);
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
  WeaponLauncher(WEAPON_SUPERTUX, "tux", new SuperTuxWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
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
  if (current_tux)
    return;

  if (tux_death_time && tux_death_time + 2000 < Time::GetInstance()->Read()) {
    UseAmmoUnit();
    tux_death_time = 0;
  }
}

bool TuxLauncher::IsInUse() const
{
  return (current_tux || tux_death_time);
}

void TuxLauncher::SignalEndOfProjectile()
{
  if (!current_tux)
    return;

  current_tux = NULL;
  tux_death_time = Time::GetInstance()->Read();
}

void TuxLauncher::HandleKeyPressed_Shoot(bool shift)
{
  if (current_tux || tux_death_time)
    return;

  Weapon::HandleKeyPressed_Shoot(shift);
}

void TuxLauncher::HandleKeyRefreshed_Shoot(bool shift)
{
  if (current_tux || tux_death_time)
    return;

  Weapon::HandleKeyRefreshed_Shoot(shift);
}

void TuxLauncher::HandleKeyReleased_Shoot(bool shift)
{
  if (current_tux) {
    Action* a = new Action(Action::ACTION_WEAPON_SUPERTUX);
    a->Push(1); // to ask for an explosion
    a->Push(current_tux->GetPos());
    ActionHandler::GetInstance()->NewAction(a);
    return;
  } else if (!tux_death_time)
    Weapon::HandleKeyReleased_Shoot(shift);
}

// Move right
void TuxLauncher::HandleKeyPressed_MoveRight(bool shift)
{
  if (current_tux)
    current_tux->turn_right();
  else if (!tux_death_time)
    ActiveCharacter().HandleKeyPressed_MoveRight(shift);
}

void TuxLauncher::HandleKeyRefreshed_MoveRight(bool shift)
{
  if (current_tux)
    current_tux->turn_right();
  else if (!tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_MoveRight(shift);
}

void TuxLauncher::HandleKeyReleased_MoveRight(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_MoveRight(shift);
}

// Move left
void TuxLauncher::HandleKeyPressed_MoveLeft(bool shift)
{
  if (current_tux)
    current_tux->turn_left();
  else if (!tux_death_time)
    ActiveCharacter().HandleKeyPressed_MoveLeft(shift);
}

void TuxLauncher::HandleKeyRefreshed_MoveLeft(bool shift)
{
  if (current_tux)
    current_tux->turn_left();
  else if (!tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_MoveLeft(shift);
}

void TuxLauncher::HandleKeyReleased_MoveLeft(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_MoveLeft(shift);
}

void TuxLauncher::HandleKeyPressed_Up(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyPressed_Up(shift);
}

void TuxLauncher::HandleKeyRefreshed_Up(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_Up(shift);
}

void TuxLauncher::HandleKeyReleased_Up(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_Up(shift);
}

void TuxLauncher::HandleKeyPressed_Down(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyPressed_Down(shift);
}

void TuxLauncher::HandleKeyRefreshed_Down(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_Down(shift);
}

void TuxLauncher::HandleKeyReleased_Down(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_Down(shift);
}

void TuxLauncher::HandleKeyPressed_Jump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyPressed_Jump(shift);
}

void TuxLauncher::HandleKeyRefreshed_Jump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_Jump(shift);
}

void TuxLauncher::HandleKeyReleased_Jump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_Jump(shift);
}

void TuxLauncher::HandleKeyPressed_HighJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyPressed_HighJump(shift);
}

void TuxLauncher::HandleKeyRefreshed_HighJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_HighJump(shift);
}

void TuxLauncher::HandleKeyReleased_HighJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_HighJump(shift);
}

void TuxLauncher::HandleKeyPressed_BackJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyPressed_BackJump(shift);
}

void TuxLauncher::HandleKeyRefreshed_BackJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyRefreshed_BackJump(shift);
}

void TuxLauncher::HandleKeyReleased_BackJump(bool shift)
{
  if (!current_tux && !tux_death_time)
    ActiveCharacter().HandleKeyReleased_BackJump(shift);
}

std::string TuxLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u tux launcher! Never seen a flying penguin?",
            "%s team has won %u tux launchers! Never seen a flying penguin?",
            items_count), TeamName, items_count);
}

void TuxLauncher::RefreshFromNetwork(double angle, Point2d pos)
{
  // Fix bug #9815 : Crash when changing tux angle in network mode.
  if (!current_tux)
    return;
  current_tux->SetAngle(angle);
  current_tux->SetPhysXY(pos);
  current_tux->SetSpeedXY(Point2d(0,0));
}

void TuxLauncher::ExplosionFromNetwork(Point2d tux_pos)
{
  if (!current_tux)
    return;

  current_tux->SetPhysXY(tux_pos);
  current_tux->Explosion();
}

SuperTuxWeaponConfig& TuxLauncher::cfg()
{
  return static_cast<SuperTuxWeaponConfig&>(*extra_params);
}
