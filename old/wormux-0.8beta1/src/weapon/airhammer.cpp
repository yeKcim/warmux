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
 * AirHammer - Use it to dig
 *****************************************************************************/

#include "weapon/airhammer.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "game/game.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "include/action_handler.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "team/macro.h"
#include "tool/i18n.h"
#include "interface/game_msg.h"
#include "weapon/explosion.h"

//-----------------------------------------------------------------------------

const uint MIN_TIME_BETWEEN_JOLT = 100; // in milliseconds

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Airhammer::Airhammer() : Weapon(WEAPON_AIR_HAMMER,"airhammer",new AirhammerConfig())
{
  m_name = _("Airhammer");
  m_help = _("Howto use it : keep space key pressed\nan ammo per turn");
  m_category = TOOL;

  impact = resource_manager.LoadImage( weapons_res_profile, "airhammer_impact");
  m_last_jolt = 0;
}

//-----------------------------------------------------------------------------

void Airhammer::p_Deselect()
{
  m_is_active = false;
}

//-----------------------------------------------------------------------------

bool Airhammer::p_Shoot()
{
  jukebox.Play("share","weapon/airhammer");

  // initiate movement ;-)
  ActiveCharacter().SetRebounding(false);

  // Little hack, so the character notices he is in the vaccum and begins to fall in the hole
  ActiveCharacter().SetXY( ActiveCharacter().GetPosition() );

  Point2i pos = Point2i(ActiveCharacter().GetX() + ActiveCharacter().GetWidth()/2 - impact.GetWidth()/2,
                        ActiveCharacter().GetTestRect().GetPositionY() +
                        ActiveCharacter().GetHeight()  -15);

  ParticleEngine::AddNow(pos + Point2i(impact.GetWidth()/4,9), 1, particle_AIR_HAMMER,
                         true, -3.0 * M_PI_4, 5.0 + Time::GetInstance()->Read() % 5);
  ParticleEngine::AddNow(pos + Point2i(3*impact.GetWidth()/4,9), 1, particle_AIR_HAMMER,
                         true, -M_PI_4, 5.0 + Time::GetInstance()->Read() % 5);
  world.Dig( pos, impact );

  uint range = 0;
  int x,y; // Testing coordinates
  bool end = false;
  do
  {
    // Did we have finished the computation
    range += 1;
    if (range > cfg().range)
    {
      range = cfg().range;
      end = true;
    }

    // Compute point coordinates
    y = ActiveCharacter().GetHandPosition().y + range;
    x = ActiveCharacter().GetHandPosition().x;

    FOR_ALL_LIVING_CHARACTERS(team, character)
    if (&(*character) != &ActiveCharacter())
    {
      // Did we touch somebody ?
      if( character->ObjTouche(Point2i(x, y)) )
      {
        // Apply damage (*ver).SetEnergyDelta (-cfg().damage);
        character->SetEnergyDelta(-cfg().damage);
        end = true;
      }
    }
  } while (!end);

  return true;
}

//-----------------------------------------------------------------------------

void Airhammer::RepeatShoot()
{
  uint time = Time::GetInstance()->Read() - m_last_jolt;
  uint tmp = Time::GetInstance()->Read();

  if (time >= MIN_TIME_BETWEEN_JOLT)
    {
      NewActionWeaponShoot();
      m_last_jolt = tmp;
    }
}

//-----------------------------------------------------------------------------

void Airhammer::Refresh()
{
}

void Airhammer::SignalTurnEnd()
{
  // It's too late !
  m_is_active = false;
}

void Airhammer::ActionStopUse()
{
  ActiveTeam().AccessNbUnits() = 0; // ammo units are lost
  m_is_active = false;
  GameLoop::GetInstance()->SetState(GameLoop::HAS_PLAYED);
}


//-----------------------------------------------------------------------------

void Airhammer::HandleKeyPressed_Shoot()
{
  HandleKeyRefreshed_Shoot();
}

void Airhammer::HandleKeyRefreshed_Shoot()
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
}

void Airhammer::HandleKeyReleased_Shoot()
{
  NewActionWeaponStopUse();
}

std::string Airhammer::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u airhammer!",
            "%s team has won %u airhammers!",
            items_count), TeamName, items_count);
}


//-----------------------------------------------------------------------------

AirhammerConfig& Airhammer::cfg() {
  return static_cast<AirhammerConfig&>(*extra_params);
}

//-----------------------------------------------------------------------------

AirhammerConfig::AirhammerConfig(){
  range =  30;
  damage = 3;
}

//-----------------------------------------------------------------------------

void AirhammerConfig::LoadXml(xmlpp::Element *elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
  XmlReader::ReadUint(elem, "damage", damage);
}
