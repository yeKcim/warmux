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
 * Jet Pack :-)
 *****************************************************************************/

#include "jetpack.h"
#include "explosion.h"
#include "game/game.h"
#include "game/game_loop.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/network.h"
#include "object/physical_obj.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "character/move.h"
#include "include/action_handler.h"

const double JETPACK_FORCE = 2500.0;

const uint DELTA_FUEL_DOWN = 100 ;  // Delta time between 2 fuel unit consumption.

JetPack::JetPack() : Weapon(WEAPON_JETPACK, "jetpack",
                            new WeaponConfig(),
                            NEVER_VISIBLE)
{
  m_name = _("Jetpack");
  m_category = MOVE;
  m_unit_visibility = VISIBLE_ONLY_WHEN_ACTIVE;

  use_unit_on_first_shoot = false;

  m_x_force = 0.0;
  m_y_force = 0.0;
  channel = -1;
}

void JetPack::Refresh()
{
  if (m_is_active)
  {
    if (!ActiveTeam().IsLocal()) {
      return;
    }

    Point2d F;
    F.x = m_x_force ;
    F.y = m_y_force ;

    ActiveCharacter().SetExternForceXY(F);
    SendActiveCharacterInfo();

    if (!F.IsNull())
    {
      // We are using fuel !!!
      uint current = Time::GetInstance()->Read() ;
      double delta = (double)(current - m_last_fuel_down);

      while (delta >= DELTA_FUEL_DOWN)
      {
        if (EnoughAmmoUnit())
        {
          UseAmmoUnit();
          m_last_fuel_down += DELTA_FUEL_DOWN ;
          delta -= DELTA_FUEL_DOWN ;
        }
        else
        {
          p_Deselect();
          break;
        }
      }
    }
  }
}

void JetPack::p_Select()
{
  ActiveCharacter().SetClothe("jetpack");

  if (!ActiveTeam().IsLocal() && !ActiveTeam().IsLocalAI()) {
    m_unit_visibility = NEVER_VISIBLE; // do not show ammo units accross the network
  } else {
    m_unit_visibility = VISIBLE_ONLY_WHEN_ACTIVE;
  }
}

void JetPack::p_Deselect()
{
  m_is_active = false;
  m_x_force = 0;
  m_y_force = 0;
  ActiveCharacter().SetExternForce(0,0);
  StopUse();
  camera.SetCloseFollowing(false);
  ActiveCharacter().SetClothe("normal");
  ActiveCharacter().SetMovement("walk");
}

void JetPack::StartUse()
{
  ActiveCharacter().SetMovement("jetpack-fire");
  if ( (m_x_force == 0) && (m_y_force == 0))
    {
      m_last_fuel_down = Time::GetInstance()->Read();
      channel = jukebox.Play(ActiveTeam().GetSoundProfile(),"weapon/jetpack", -1);

      camera.FollowObject (&ActiveCharacter(),true, true, true);
      camera.SetCloseFollowing(true);
//                           bool suit, bool recentre,
//                           bool force_recentrage=false);
    }
}

void JetPack::StopUse()
{
  ActiveCharacter().SetMovement("jetpack-nofire");
  if (m_x_force == 0.0 && m_y_force == 0.0)
  {
    if(channel != -1)
      jukebox.Stop(channel);
    channel = -1;
  }
}

void JetPack::GoUp()
{
  StartUse();
  m_y_force = -(ActiveCharacter().GetMass() * GameMode::GetInstance()->gravity + JETPACK_FORCE);
}

void JetPack::GoLeft()
{
  StartUse();
  m_x_force = - JETPACK_FORCE ;
  if(ActiveCharacter().GetDirection() == Body::DIRECTION_RIGHT)
    ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
}

void JetPack::GoRight()
{
  StartUse();
  m_x_force = JETPACK_FORCE ;
  if(ActiveCharacter().GetDirection() == Body::DIRECTION_LEFT)
    ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
}

void JetPack::StopUp()
{
  m_y_force = 0.0 ;
  StopUse();
}

void JetPack::StopLeft()
{
  m_x_force = 0.0 ;
  StopUse();
}

void JetPack::StopRight()
{
  m_x_force = 0.0 ;
  StopUse();
}

void JetPack::HandleKeyPressed_Up()
{
  if (m_is_active)
    GoUp();
  else
    ActiveCharacter().HandleKeyPressed_Up();
}

void JetPack::HandleKeyReleased_Up()
{
  if (m_is_active)
    StopUp();
  else
    ActiveCharacter().HandleKeyReleased_Up();
}

void JetPack::HandleKeyPressed_MoveLeft()
{
  if (m_is_active)
    GoLeft();
  else
    ActiveCharacter().HandleKeyPressed_MoveLeft();
}

void JetPack::HandleKeyReleased_MoveLeft()
{
  if (m_is_active)
    StopLeft();
  else
    ActiveCharacter().HandleKeyReleased_MoveLeft();
}

void JetPack::HandleKeyPressed_MoveRight()
{
  if (m_is_active)
    GoRight();
  else
    ActiveCharacter().HandleKeyPressed_MoveRight();
}

void JetPack::HandleKeyReleased_MoveRight()
{
  if (m_is_active)
    StopRight();
  else
    ActiveCharacter().HandleKeyReleased_MoveRight();
}

void JetPack::HandleKeyPressed_Shoot()
{
  if (!m_is_active)
    NewActionWeaponShoot();
  else
    NewActionWeaponStopUse();
}

bool JetPack::p_Shoot()
{
  m_is_active = true;
  ActiveCharacter().SetClothe("jetpack-fire");

  return true;
}

void JetPack::SignalTurnEnd()
{
  p_Deselect();
}

void JetPack::ActionStopUse()
{
  p_Deselect();
}

std::string JetPack::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u jetpack!",
            "%s team has won %u jetpacks!",
            items_count), TeamName, items_count);
}

