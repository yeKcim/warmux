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
 * Slap. The target moves a bit in amazement (similar to the prod weapon).
 *****************************************************************************/

#include "weapon/slap.h"
#include "weapon/weapon_cfg.h"

#include <iostream>
#include "map/camera.h"
#include "character/character.h"
#include "character/body.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"
#include "include/action_handler.h"

class SlapConfig : public WeaponConfig
{
  public:
    uint range;
    uint damage;
    uint strength;
    SlapConfig();
    void LoadXml(const xmlNode* elem);
};

SlapConfig& Slap::cfg() {
  return static_cast<SlapConfig&>(*extra_params);
}

SlapConfig::SlapConfig(){
  range = 20;
  damage = 5;
  strength = 10;
}

void SlapConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
  XmlReader::ReadUint(elem, "damage", damage);
  XmlReader::ReadUint(elem, "strength", strength);
}

Slap::Slap() : Weapon(WEAPON_SLAP, "slap", new SlapConfig())
{
  UpdateTranslationStrings();

  m_category = DUEL;
}

void Slap::UpdateTranslationStrings()
{
  m_name = _("Slap");
  m_help = _("Slap the enemy to make him jump a bit.");
}

void Slap::Refresh()
{
  if (IsInUse()) m_is_active = false;
}

std::string Slap::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u slap!",
            "%s team has won %u slaps! Dodge this!",
            items_count), TeamName, items_count);
}

bool Slap::p_Shoot (){
  double angle = ActiveCharacter().GetFiringAngle();
  double radius = 0.0;
  bool end = false;

  JukeBox::GetInstance()->Play ("share","weapon/slap");

  do
  {
    // Did we finish the computation?
    radius += 1.0;
    if (cfg().range < radius)
    {
      radius = cfg().range;
      end = true;
    }

    // Compute point coordinates
    Point2i relative_pos(static_cast<int>(radius * cos(angle)),
                         static_cast<int>(radius * sin(angle)) );
    Point2i pos_to_check = ActiveCharacter().GetHandPosition() + relative_pos;
    FOR_ALL_LIVING_CHARACTERS(team, character)
    if (&(*character) != &ActiveCharacter())
    {
      // Did we touch somebody ?
      if( character->Contain(pos_to_check) )
      {
        // Apply damage
        character->SetEnergyDelta(-(int)cfg().damage);
        character->SetSpeed(cfg().strength / character->GetMass(), angle);
        character->SetMovement("fly");
        Camera::GetInstance()->FollowObject(&(*character), true, true);

        end = true;
      }
    }
  } while (!end);

  return true;
}

