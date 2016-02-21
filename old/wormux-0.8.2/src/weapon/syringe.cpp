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
 * Weapon Syringe
 *****************************************************************************/

#include "weapon/explosion.h"
#include "weapon/syringe.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/point.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"

class SyringeConfig : public WeaponConfig
{
  public:
    uint range;
    uint damage;
    uint turns;
    SyringeConfig();
    void LoadXml(const xmlNode* elem);
};

SyringeConfig& Syringe::cfg() {
  return static_cast<SyringeConfig&>(*extra_params);
}

SyringeConfig::SyringeConfig(){
  range =  45;
  turns = 10;
  damage = 10;
}

void SyringeConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
  XmlReader::ReadUint(elem, "turns", turns);
  XmlReader::ReadUint(elem, "damage", damage);
}

Syringe::Syringe() : Weapon(WEAPON_SYRINGE, "syringe", new SyringeConfig())
{
  UpdateTranslationStrings();

  m_category = DUEL;
}

void Syringe::UpdateTranslationStrings()
{
  m_name = _("Syringe");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

bool Syringe::p_Shoot (){
  double angle = ActiveCharacter().GetFiringAngle();
  double radius = 0.0;
  bool end = false;

  JukeBox::GetInstance()->Play ("share","weapon/syringe");

  do
  {
    // Did we have finished the computation
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
        // Apply damage (*ver).SetEnergyDelta (-cfg().damage);
        character->SetDiseaseDamage(cfg().damage, cfg().turns);
        end = true;
      }
    }
  } while (!end);

  return true;
}

std::string Syringe::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u syringe!",
            "%s team has won %u syringes!",
            items_count), TeamName, items_count);
}
