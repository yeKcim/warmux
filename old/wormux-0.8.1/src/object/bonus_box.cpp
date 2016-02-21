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
 * Bonus box : fall from the sky at random time.
 * The box can contain any weapon in the game.
 *****************************************************************************/

#include "object/bonus_box.h"
#include <sstream>
#include <iostream>
#include "character/character.h"
#include "graphic/sprite.h"
#include "include/action.h"
#include "interface/game_msg.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/random.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"
#include "network/randomsync.h"

BonusBox::BonusBox():
  ObjBox("bonus_box")
{
  SetTestRect (29, 29, 63, 6);

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  anim = resource_manager.LoadSprite( res, "object/bonus_box");
  resource_manager.UnLoadXMLProfile(res);
  weapon_num = 0;

  SetSize(anim->GetSize());
  anim->animation.SetLoopMode(false);
  anim->SetCurrentFrame(0);
}

void BonusBox::PickRandomWeapon()
{
  Weapon::Weapon_type w_type;

  ASSERT(weapon_list.size() != 0);

  weapon_num = 0;
  int nb_try = 0;
  do {
    double num = RandomLocal().GetDouble(0, total_probability);
    double total_bf_weapon = 0, total_after_weapon = 0;

    for (uint i=0; i < weapon_list.size(); i++) {
      total_after_weapon = total_bf_weapon + weapon_list[i].probability;

      if (total_bf_weapon < num && num <= total_after_weapon) {
	weapon_num = i;
	break;
      }
      total_bf_weapon = total_after_weapon;
    }
    w_type = weapon_list[weapon_num].weapon->GetType();
    nb_try++;
  } while (ActiveTeam().ReadNbAmmos(w_type) == INFINITE_AMMO
	   && nb_try <= 50);
  MSG_DEBUG("bonus","Weapon choosed: %s", weapon_list[weapon_num].weapon->GetName().c_str());
}

void BonusBox::ApplyBonus(Character * c)
{
  std::ostringstream txt;
  if ( ExplodesInsteadOfBonus(c) ) {
    GameMessages::GetInstance()->Add( _("Someone put a booby trap into the crate!") );
    Explode();
    return;
  };
  Weapon::Weapon_type w_type = weapon_list[weapon_num].weapon->GetType();

  /*this next 'if' should never be true, but I am loath to remove it just in case. */
  if (c->AccessTeam().ReadNbAmmos(w_type) != INFINITE_AMMO) {
    c->AccessTeam().m_nb_ammos[w_type] += weapon_list[weapon_num].nb_ammos;
    txt << weapon_list[weapon_num].weapon->GetWeaponWinString(c->AccessTeam().GetName().c_str(), weapon_list[weapon_num].nb_ammos);
  } else {
    txt << Format(gettext("%s team already has infinite ammo for the %s!"), //this should never appear
           c->AccessTeam().GetName().c_str(), weapon_list[weapon_num].weapon->GetName().c_str());
  }
  GameMessages::GetInstance()->Add(txt.str());
}

bool BonusBox::ExplodesInsteadOfBonus(Character * c)
{
  ASSERT(NULL != c);

  // Empyric formula:
  // 1% chance of explosion for each 5 points of energy
  // (with max 20% for 100 energy)
  float explosion_probability = (float)c->GetEnergy() / 5.0f;
  // clamp to some reasonable values
  if ( explosion_probability < 5.0f )
    explosion_probability = 5.0f;
  else if ( explosion_probability > 40.0f )
    explosion_probability = 40.0f;

  float randval = RandomSync().GetDouble( 1, 100 );
  bool exploding = randval < explosion_probability;
  MSG_DEBUG("bonus","explosion chance: %.2f%%, actual value: %.2f, %s",
    explosion_probability, randval, exploding ? "exploding!" : "not exploding");

  return exploding;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
double BonusBox::total_probability;
std::vector<struct WeaponProba> BonusBox::weapon_list;

/* Weapon probabilities could possibily be stored in the weapon section of classic.xml
  and retrieved by weapon.GetBonusProbability() and weapon.GetBonusAmmo()
  however, this is not the way that was chosen.
*/
void BonusBox::LoadXml(const xmlNode* object)
{
  total_probability = 0;
  struct WeaponProba w;

  XmlReader::ReadInt(object, "life_points", start_life_points);
  const xmlNode* node = XmlReader::GetMarker(object, "probability");
  std::list<Weapon*> l_weapons_list = WeaponsList::GetInstance()->GetList();
  std::list<Weapon*>::iterator
    itw = l_weapons_list.begin(),
    end = l_weapons_list.end();

  for(; itw != end; ++itw) {
    w.weapon = *itw;

    if (!XmlReader::ReadDouble(node, w.weapon->GetID().c_str(), w.probability)) {
      std::cerr << "No bonus probability defined for weapon "
		<< w.weapon->GetID().c_str() << std::endl;
      continue;
    }
    if (w.probability == 0.0) {
      continue;
    }
    total_probability += w.probability;

    const xmlNode* elem = XmlReader::GetMarker(node, w.weapon->GetID());
    ASSERT(elem != NULL);
    XmlReader::ReadIntAttr (elem, "ammo", w.nb_ammos);

    if ((*itw)->ReadInitialNbAmmo() == INFINITE_AMMO) {
      w.probability = 0;
    }
    MSG_DEBUG("bonus","+ %s: %f", w.weapon->GetName().c_str(), w.probability);
    weapon_list.push_back(w);
  }
  ASSERT(total_probability != 0.0);
}

void BonusBox::GetValueFromAction(Action * a)
{
  ObjBox::GetValueFromAction(a);
  weapon_num = (uint)(a->PopInt());
}

void BonusBox::Randomize()
{
  PickRandomWeapon();
}

void BonusBox::StoreValue(Action * a)
{
  ObjBox::StoreValue(a);
  a->Push(int(weapon_num));
}
