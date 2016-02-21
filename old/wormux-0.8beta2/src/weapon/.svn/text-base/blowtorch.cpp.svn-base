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

#include "blowtorch.h"
#include "explosion.h"
#include "weapon_cfg.h"

#include "character/character.h"
#include "include/action_handler.h"
#include "tool/i18n.h"
#include "map/map.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "character/move.h"
#include "character/body.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

static const uint MIN_TIME_BETWEEN_DIG = 200;        // milliseconds

class BlowtorchConfig : public WeaponConfig
{
  public:
    BlowtorchConfig();
    virtual void LoadXml(xmlpp::Element* elem);

    uint range;
};

Blowtorch::Blowtorch() : Weapon(WEAPON_BLOWTORCH, "blowtorch", new BlowtorchConfig())
{
  m_name = _("Blowtorch");
  m_help = _("Howto use it : keep space key pressed\nAngle : Up/Down\nan ammo per turn");
  m_category = TOOL;
  m_time_between_each_shot = MIN_TIME_BETWEEN_DIG;
  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile, "blowtorch_fire"));
}

void Blowtorch::p_Deselect()
{
  ActiveCharacter().body->ResetWalk();
  ActiveCharacter().body->StopWalk();
  ActiveTeam().AccessNbUnits() = 0;
}

bool Blowtorch::IsInUse() const
{
  return m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
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

void Blowtorch::RepeatShoot() const
{
  uint time = Time::GetInstance()->Read() - m_last_fire_time;

  if (time >= m_time_between_each_shot) {
    NewActionWeaponShoot();
  }
}

void Blowtorch::HandleKeyPressed_Shoot(bool shift)
{
  ActiveCharacter().BeginMovementRL(PAUSE_MOVEMENT);
  ActiveCharacter().SetRebounding(false);
  ActiveCharacter().body->StartWalk();

  HandleKeyRefreshed_Shoot(shift);
}

void Blowtorch::HandleKeyRefreshed_Shoot(bool)
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
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

std::string Blowtorch::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u blowtorch! If you're under 18, ask your parents to use it.",
            "%s team has won %u blowtorchs! If you're under 18, ask your parents to use it.",
            items_count), TeamName, items_count);
}
