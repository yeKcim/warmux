/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include "tool/xml_document.h"
#include "include/action_handler.h"

class SlapConfig : public WeaponConfig
{
public:
  Double range;
  uint strength;
  SlapConfig()
  {
    push_back(new DoubleConfigElement("range", &range, 20, 1, 50));
    push_back(new UintConfigElement("strength", &strength, 300, 100, 500));
  }
};


//-----------------------------------------------------------------

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

std::string Slap::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u slap!",
            "%s team has won %u slaps! Dodge this!",
            items_count), TeamName, items_count);
}

bool Slap::p_Shoot (){
  Double angle = ActiveCharacter().GetFiringAngle();
  Double radius = 0.0;
  bool end = false;

  JukeBox::GetInstance()->Play ("default","weapon/slap");

  Character *player = &ActiveCharacter();
  do {
    // Did we finish the computation?
    radius += ONE;
    if (cfg().range < radius)
    {
      radius = cfg().range;
      end = true;
    }

    // Compute point coordinates
    Point2i relative_pos(static_cast<int>(radius * cos(angle)),
                         static_cast<int>(radius * sin(angle)) );
    Point2i hand_position;
    player->GetHandPosition(hand_position);
    Point2i pos_to_check = hand_position + relative_pos;
    FOR_ALL_LIVING_CHARACTERS(team, character)
    if (&(*character) != player)
    {
      // Did we touch somebody ?
      if (character->Contain(pos_to_check))
      {
        // Apply damage
        character->SetEnergyDelta(-(int)cfg().damage, player);
        character->SetSpeed(cfg().strength / character->GetMass(), angle);
        character->SetMovement("fly");
        Camera::GetInstance()->FollowObject(&(*character));

        end = true;
      }
    }
  } while (!end);

  return true;
}

SlapConfig& Slap::cfg()
{
  return static_cast<SlapConfig&>(*extra_params);
}

