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
 * Weapon Supertux : Look ! it's the famous flying magic pinguin !
 *****************************************************************************/

#include "supertux.h"
#include "explosion.h"
#include "game/config.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/network.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

const uint time_delta = 40;
const uint animation_deltat = 50;

SuperTux::SuperTux(SuperTuxWeaponConfig& cfg,
                   WeaponLauncher * p_launcher) :
  WeaponProjectile ("supertux", cfg, p_launcher),
  particle_engine(40)
{
  explode_colliding_character = true;
  SetSize(image->GetSize());
  SetTestRect(1, 1, 2, 2);
  sound_channel = -1;
}

void SuperTux::Shoot(double strength)
{
  WeaponProjectile::Shoot(strength);
  angle_rad = ActiveCharacter().GetFiringAngle();

  Time * global_time = Time::GetInstance();
  time_next_action = global_time->Read();
  last_move = global_time->Read();
  begin_time = global_time->Read();

  sound_channel = jukebox.Play("share","weapon/supertux_flying", -1);
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
    a.Push(angle_rad);
    a.Push(GetPhysX());
    a.Push(GetPhysY());
    Network::GetInstance()->SendAction(&a);
  }
  particle_engine.AddPeriodic(GetPosition(), particle_STAR, false, angle_rad, 0);
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

void SuperTux::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("Bye bye tux..."));
  WeaponProjectile::SignalOutOfMap();

  jukebox.Stop(sound_channel);

  // To go further in the game loop
  static_cast<TuxLauncher *>(launcher)->EndOfTurn();
}

void SuperTux::Explosion()
{
  WeaponProjectile::Explosion();

  jukebox.Stop(sound_channel);

  // To go further in the game loop
  static_cast<TuxLauncher *>(launcher)->EndOfTurn();
}

//-----------------------------------------------------------------------------

SuperTuxWeaponConfig::SuperTuxWeaponConfig()
{
  speed = 2;
}

void SuperTuxWeaponConfig::LoadXml(xmlpp::Element *elem)
{
  ExplosiveWeaponConfig::LoadXml (elem);
  XmlReader::ReadUint(elem, "speed", speed);
}

//-----------------------------------------------------------------------------

TuxLauncher::TuxLauncher() :
  WeaponLauncher(WEAPON_SUPERTUX, "tux", new SuperTuxWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("SuperTux");
  m_category = SPECIAL;
  ReloadLauncher();

  // unit will be used when the supertux disappears
  use_unit_on_first_shoot = false;
}

WeaponProjectile * TuxLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new SuperTux(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

bool TuxLauncher::p_Shoot ()
{
  current_tux = static_cast<SuperTux *>(projectile);
  bool r = WeaponLauncher::p_Shoot();

  if (r) m_is_active = true;
  return r;
}

void TuxLauncher::EndOfTurn()
{
  // To go further in the game loop
  m_is_active = false;
  GameLoop::GetInstance()->SetState(GameLoop::HAS_PLAYED);
}

void TuxLauncher::HandleKeyPressed_MoveRight()
{
  if (m_is_active)
    current_tux->turn_right();
  else
    ActiveCharacter().HandleKeyPressed_MoveRight();
}

void TuxLauncher::HandleKeyRefreshed_MoveRight()
{
  if (m_is_active)
    current_tux->turn_right();
  else
    ActiveCharacter().HandleKeyRefreshed_MoveRight();
}

void TuxLauncher::HandleKeyReleased_MoveRight()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyReleased_MoveRight();
}

void TuxLauncher::HandleKeyPressed_MoveLeft()
{
  if (m_is_active)
    current_tux->turn_left();
  else
    ActiveCharacter().HandleKeyPressed_MoveLeft();
}

void TuxLauncher::HandleKeyRefreshed_MoveLeft()
{
  if (m_is_active)
    current_tux->turn_left();
  else
    ActiveCharacter().HandleKeyRefreshed_MoveLeft();
}

void TuxLauncher::HandleKeyReleased_MoveLeft()
{
  if (!m_is_active)
    ActiveCharacter().HandleKeyReleased_MoveLeft();
}

std::string TuxLauncher::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u tux launcher!",
            "%s team has won %u tux launchers!",
            items_count), TeamName, items_count);
}

SuperTuxWeaponConfig& TuxLauncher::cfg()
{
  return static_cast<SuperTuxWeaponConfig&>(*extra_params);
}
