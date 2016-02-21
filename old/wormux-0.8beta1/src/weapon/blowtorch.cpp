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
 * Blowtorch - burns holes into walls
 *****************************************************************************/

#include "weapon/blowtorch.h"
#include "include/action_handler.h"
#include "tool/i18n.h"
#include "map/map.h"
#include "team/teams_list.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "character/move.h"
#include "character/body.h"
#include "explosion.h"

static const uint pause_time = 200;	// milliseconds

Blowtorch::Blowtorch() : Weapon(WEAPON_BLOWTORCH, "blowtorch", new BlowtorchConfig())
{
  m_name = _("Blowtorch");
  m_help = _("Howto use it : keep space key pressed\nAngle : Up/Down\nan ammo per turn");
  m_category = TOOL;

  new_timer = 0;
  old_timer = 0;

  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile, "blowtorch_fire"));
}

void Blowtorch::Refresh()
{

}

void Blowtorch::p_Deselect()
{
  ActiveCharacter().body->ResetWalk();
  ActiveCharacter().body->StopWalk();
  ActiveTeam().AccessNbUnits() = 0;
  m_is_active = false;
}

void Blowtorch::SignalTurnEnd()
{
  p_Deselect();
}

void Blowtorch::ActionStopUse()
{
  SignalTurnEnd();
  GameLoop::GetInstance()->SetState(GameLoop::HAS_PLAYED);
}

bool Blowtorch::p_Shoot()
{
  Point2i hole = ActiveCharacter().GetCenter();

  double angle = ActiveCharacter().GetFiringAngle();
  uint h = cfg().range;
  double dx = cos(angle) * h;
  double dy = sin(angle) * h;

  Point2i pos = Point2i(hole.x+(int)dx, hole.y+(int)dy);
  world.Dig(pos, ActiveCharacter().GetHeight()/2);

  MoveCharacter(ActiveCharacter());

  return true;
}

void Blowtorch::RepeatShoot()
{
  uint time = Time::GetInstance()->Read() - old_timer;
  uint tmp = Time::GetInstance()->Read();

  if (time >= pause_time)
    {
      NewActionWeaponShoot();
      old_timer = tmp;
    }
}

void Blowtorch::HandleKeyPressed_Shoot()
{
  ActiveCharacter().BeginMovementRL(PAUSE_MOVEMENT);
  ActiveCharacter().SetRebounding(false);
  ActiveCharacter().body->StartWalk();

  HandleKeyRefreshed_Shoot();
}

void Blowtorch::HandleKeyRefreshed_Shoot()
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
}

void Blowtorch::HandleKeyReleased_Shoot()
{
  NewActionWeaponStopUse();
}

//-------------------------------------------------------------------------------------

BlowtorchConfig::BlowtorchConfig()
{
  range = 2;
}

BlowtorchConfig& Blowtorch::cfg()
{
  return static_cast<BlowtorchConfig&>(*extra_params);
}

void BlowtorchConfig::LoadXml(xmlpp::Element* elem)
{
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
}

std::string Blowtorch::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u blowtorch!",
            "%s team has won %u blowtorchs!",
            items_count), TeamName, items_count);
}

