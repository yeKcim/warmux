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
 * baseball bat
 *****************************************************************************/

#include "weapon/baseball.h"
#include "weapon/weapon_cfg.h"
#include "character/character.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "map/camera.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/point.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/explosion.h"

class BaseballConfig : public WeaponConfig
{
  public:
    uint range;
    uint strength;
    BaseballConfig();
    void LoadXml(const xmlNode* elem);
};

Baseball::Baseball() : Weapon(WEAPON_BASEBALL, "baseball", new BaseballConfig())
{
  UpdateTranslationStrings();

  m_category = DUEL;
  m_weapon_fire = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_fire"));
  m_weapon_fire->EnableRotationCache(32);
}

void Baseball::UpdateTranslationStrings()
{
  m_name = _("Baseball Bat");
  m_help = _("Angle : Up/Down\nFire : space key\na hit per turn");
}

bool Baseball::p_Shoot()
{

  double angle = ActiveCharacter().GetFiringAngle();
  double rayon = 0.0;
  bool end = false;

  JukeBox::GetInstance()->Play ("share","weapon/baseball");

  do
  {
    // Did we have finished the computation
    rayon += 1.0;
    if (cfg().range < rayon)
    {
      rayon = cfg().range;
      end = true;
    }

    // Compute point coordinates
    Point2i relative_pos(static_cast<int>(rayon * cos(angle)),
                         static_cast<int>(rayon * sin(angle)) );
    Point2i pos_to_check = ActiveCharacter().GetHandPosition() + relative_pos;

    FOR_ALL_LIVING_CHARACTERS(team, character)
    if (&(*character) != &ActiveCharacter())
    {
      // Did we touch somebody ?
      if( character->Contain(pos_to_check) )
      {
        // Apply damage (*ver).SetEnergyDelta (-cfg().damage);
        character->SetSpeed(cfg().strength / character->GetMass(), angle);
        character->SetMovement("fly");
        Camera::GetInstance()->FollowObject(&(*character), true, true);
        return true;
      }
    }
  } while (!end);

  return true;
}

bool Baseball::IsInUse() const
{
  return m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
}

BaseballConfig& Baseball::cfg()
{
  return static_cast<BaseballConfig&>(*extra_params);
}

std::string Baseball::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u baseball bat!",
            "%s team has won %u baseball bats!",
            items_count), TeamName, items_count);
}


BaseballConfig::BaseballConfig()
{
  range =  70;
  strength = 250;
}

void BaseballConfig::LoadXml(const xmlNode* elem)
{
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadUint(elem, "range", range);
  XmlReader::ReadUint(elem, "strength", strength);
}
