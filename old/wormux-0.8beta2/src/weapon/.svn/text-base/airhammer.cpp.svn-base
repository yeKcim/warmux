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

#include "airhammer.h"
#include "explosion.h"
#include "weapon_cfg.h"

//-----------------------------------------------------------------------------
#include <sstream>
#include "character/character.h"
#include "game/game.h"
#include "game/game_loop.h"
#include "game/time.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

//-----------------------------------------------------------------------------

const uint MIN_TIME_BETWEEN_JOLT = 100; // in milliseconds

//-----------------------------------------------------------------------------

class AirhammerConfig : public WeaponConfig
{
  public:
    uint range;
    uint damage;
    AirhammerConfig();
    void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

Airhammer::Airhammer() : Weapon(WEAPON_AIR_HAMMER,"airhammer",new AirhammerConfig())
{
  m_name = _("Airhammer");
  m_help = _("Howto use it : keep space key pressed\nan ammo per turn");
  m_category = TOOL;

  impact = resource_manager.LoadImage( weapons_res_profile, "airhammer_impact");
  m_time_between_each_shot = MIN_TIME_BETWEEN_JOLT;
}

//-----------------------------------------------------------------------------

bool Airhammer::p_Shoot()
{
  //if the sound isn't already playing, play it again.
   if(!drill_sound.IsPlaying()) {
    drill_sound.Play("share","weapon/airhammer", -1);
  }

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
        character->SetEnergyDelta(-(int)cfg().damage);
        end = true;
      }
    }
  } while (!end);

  return true;
}

//-----------------------------------------------------------------------------

void Airhammer::RepeatShoot() const
{
  uint time = Time::GetInstance()->Read() - m_last_fire_time;

  if (time >= m_time_between_each_shot) {
    NewActionWeaponShoot();
  }
}

//-----------------------------------------------------------------------------

void Airhammer::ActionStopUse()
{
  ActiveTeam().AccessNbUnits() = 0; // ammo units are lost
  GameLoop::GetInstance()->SetState(GameLoop::HAS_PLAYED);
  p_Deselect();
}

void Airhammer::p_Deselect()
{
  drill_sound.Stop();
}

//-----------------------------------------------------------------------------

void Airhammer::HandleKeyRefreshed_Shoot(bool)
{
  if (EnoughAmmoUnit()) {
    RepeatShoot();
  }
}

bool Airhammer::IsInUse() const
{
  return m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
}

void Airhammer::p_Select()
{
  jukebox.Play("share","weapon/airhammer_select");
}

std::string Airhammer::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u airhammer! Don't make too much noise with it! Thanks, your neighbours.",
            "%s team has won %u airhammers! Don't make too much noise with them! Thanks, your neighbours.",
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
