/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

//-----------------------------------------------------------------------------
#include <sstream>
#include "character/character.h"
#include "game/game.h"
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
    void LoadXml(const xmlNode* elem);
};

//-----------------------------------------------------------------------------

Airhammer::Airhammer() : Weapon(WEAPON_AIR_HAMMER,"airhammer",new AirhammerConfig())
{
  UpdateTranslationStrings();

  m_category = TOOL;

  impact = resource_manager.LoadImage( weapons_res_profile, "airhammer_impact");
  m_time_between_each_shot = MIN_TIME_BETWEEN_JOLT;
}

//-----------------------------------------------------------------------------

void Airhammer::UpdateTranslationStrings()
{
  m_name = _("Airhammer");
  m_help = _("Howto use it : keep space key pressed\nan ammo per turn");
}

//-----------------------------------------------------------------------------

bool Airhammer::p_Shoot()
{
  //if the sound isn't already playing, play it again.
  select_sound.Stop();
  if (!drill_sound.IsPlaying()) {
    drill_sound.Play("share","weapon/airhammer", -1);
  }

  // initiate movement ;-)
  ActiveCharacter().SetRebounding(false);

  Point2i pos = Point2i(ActiveCharacter().GetX() + ActiveCharacter().GetWidth()/2 - impact.GetWidth()/2,
                        ActiveCharacter().GetTestRect().GetPositionY() +
                        ActiveCharacter().GetHeight()  -16);

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
      if( character->Contain(Point2i(x, y)) )
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

void Airhammer::ActionStopUse()
{
  ActiveTeam().AccessNbUnits() = 0; // ammo units are lost
  Game::GetInstance()->SetState(Game::HAS_PLAYED);
  p_Deselect();
}

void Airhammer::p_Deselect()
{
  drill_sound.Stop();
  select_sound.Stop();
  ActiveCharacter().SetMovement("breathe");
}

//-----------------------------------------------------------------------------

void Airhammer::HandleKeyRefreshed_Shoot(bool)
{
  if (EnoughAmmoUnit()) {
    Weapon::RepeatShoot();
  }
}

bool Airhammer::IsInUse() const
{
  return m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
}

void Airhammer::p_Select()
{
  select_sound.Play("share","weapon/airhammer_select",-1);
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

void AirhammerConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
  XmlReader::ReadUint(elem, "damage", damage);
}
