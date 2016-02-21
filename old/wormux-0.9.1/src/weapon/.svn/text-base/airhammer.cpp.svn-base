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
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

//-----------------------------------------------------------------------------

const uint MIN_TIME_BETWEEN_JOLT = 100; // in milliseconds

//-----------------------------------------------------------------------------

class AirhammerConfig : public WeaponConfig
{
  public:
    uint range;
    AirhammerConfig();
    void LoadXml(const xmlNode* elem);
};

//-----------------------------------------------------------------------------

Airhammer::Airhammer():
  Weapon(WEAPON_AIR_HAMMER, "airhammer", new AirhammerConfig()),
  active(false),
  deactivation_requested(false)
{
  UpdateTranslationStrings();

  m_category = TOOL;

  impact = GetResourceManager().LoadImage( weapons_res_profile, "airhammer_impact");
  m_time_between_each_shot = MIN_TIME_BETWEEN_JOLT;
  m_can_change_weapon = true;
}

//-----------------------------------------------------------------------------

void Airhammer::UpdateTranslationStrings()
{
  m_name = _("Airhammer");
  m_help = _("How to use it : keep the space key pressed\nan ammo per turn");
}

//-----------------------------------------------------------------------------

bool Airhammer::p_Shoot()
{
  // initiate movement ;-)
  ActiveCharacter().SetRebounding(false);

  Point2i pos = Point2i(ActiveCharacter().GetX() + ActiveCharacter().GetWidth()/2 - impact.GetWidth()/2,
                        ActiveCharacter().GetTestRect().GetPositionY() +
                        ActiveCharacter().GetHeight()  -16);

  ParticleEngine::AddNow(pos + Point2i(impact.GetWidth()/4,9), 1, particle_AIR_HAMMER,
                         true, -3.0 * M_PI_4, 5.0 + Time::GetInstance()->Read() % 5);
  ParticleEngine::AddNow(pos + Point2i(3*impact.GetWidth()/4,9), 1, particle_AIR_HAMMER,
                         true, -M_PI_4, 5.0 + Time::GetInstance()->Read() % 5);
  GetWorld().Dig( pos, impact );

  return true;
}

//-----------------------------------------------------------------------------
void Airhammer::p_Deselect()
{
  drill_sound.Stop();
  select_sound.Stop();
  ActiveCharacter().SetMovement("breathe");
  active = false;
}

void Airhammer::StartShooting()
{
  if (!EnoughAmmo())
    return;
  //if the sound isn't already playing, play it again.
  select_sound.Stop();
  if (!drill_sound.IsPlaying()) {
    drill_sound.Play("default","weapon/airhammer", -1);
  }

  active = true;
  deactivation_requested = false;
}

void Airhammer::StopShooting()
{
  // The airhammer does not get deactivated in this method,
  // because there could be a shot in progress.
  // Explantion:
  // The weapon calls PrepareShot of the active character
  // which will then call p_Shot when it's ready
  deactivation_requested = true;
}

bool Airhammer::ShouldAmmoUnitsBeDrawn() const
{
  // Hide that the units are actually at maximum and not at 0
  // when the ammo counter is at 0.
  return active || EnoughAmmo();
}
//-----------------------------------------------------------------------------

void Airhammer::Refresh()
{
  if (active && deactivation_requested && !ActiveCharacter().IsPreparingShoot()) {
    active = false;
    drill_sound.Stop();
    PlaySoundSelect();
    ActiveTeam().AccessNbUnits() = 0;
  }
  if (EnoughAmmoUnit() && active) {
    Weapon::RepeatShoot();
  }
}

void Airhammer::PlaySoundSelect()
{
  select_sound.Play("default","weapon/airhammer_select",-1);
}

void Airhammer::p_Select()
{
  PlaySoundSelect();
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
}

//-----------------------------------------------------------------------------

void AirhammerConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
}
