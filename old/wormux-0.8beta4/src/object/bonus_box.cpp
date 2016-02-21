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

BonusBox::BonusBox():
  ObjBox("bonus_box"),
  nbr_ammo(2),
  contents(Weapon::WEAPON_MINE)
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

void BonusBox::Draw()
{
  anim->Draw(GetPosition());
}

void BonusBox::PickRandomWeapon() {
  weapon_num = 0;
  if(weapon_count <= 0) { //there was an error in the LoadXml function, or it wasn't called, so have it explode
    energy = 0;
    MSG_DEBUG("bonus","Weapon count is zero");
    return;
  }
  do {
    weapon_num = (int)Random::GetDouble(1, weapon_count);
    contents = (weapon_map[weapon_num].first)->GetType();
  } while (ActiveTeam().ReadNbAmmos(contents) == INFINITE_AMMO);
  nbr_ammo = weapon_map[weapon_num].second;
}

void BonusBox::ApplyBonus(Character * c) {
  if(weapon_count == 0 || nbr_ammo == 0) return;
  std::ostringstream txt;
  /*this next 'if' should never be true, but I am loath to remove it just in case. */
  if(c->AccessTeam().ReadNbAmmos(contents) != INFINITE_AMMO) {
    c->AccessTeam().m_nb_ammos[contents] += nbr_ammo;
    txt << WeaponsList::GetInstance()->GetWeapon(contents)->
                GetWeaponWinString(c->AccessTeam().GetName().c_str(), nbr_ammo);
  } else {
    txt << Format(gettext("%s team already has infinite ammo for the %s!"), //this should never appear
           c->AccessTeam().GetName().c_str(), WeaponsList::GetInstance()->GetWeapon(contents)->GetName().c_str());
  }
  GameMessages::GetInstance()->Add(txt.str());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Static methods
uint BonusBox::weapon_count = 0;
std::map<int,std::pair<Weapon*,int> > BonusBox::weapon_map;

/* Weapon probabilities could possibily be stored in the weapon section of classic.xml
  and retrieved by weapon.GetBonusProbability() and weapon.GetBonusAmmo()
  however, this is not the way that was chosen.
*/
void BonusBox::LoadXml(const xmlpp::Element * object)
{
  XmlReader::ReadInt(object, "life_points", start_life_points);
  object = XmlReader::GetMarker(object, "probability");
  std::list<Weapon*> l_weapons_list = WeaponsList::GetInstance()->GetList();
  std::list<Weapon*>::iterator
      itw = l_weapons_list.begin(),
      end = l_weapons_list.end();
  xmlpp::Element *elem;
  uint a = 0;
  int prob = 0;
  int ammo = 0;
  for(; itw != end; ++itw) {
    XmlReader::ReadInt(object, (*itw)->GetID().c_str(), prob);
    elem = XmlReader::GetMarker(object, (*itw)->GetID());
    if(elem != NULL)
      XmlReader::ReadIntAttr (elem, "ammo", ammo);
    if((*itw)->ReadInitialNbAmmo() == INFINITE_AMMO) {
      prob = 0;
    }
    for(a = weapon_count; a < weapon_count+prob; ++a) {
      weapon_map[a]=std::make_pair<Weapon*,int>(*itw,ammo);
    }
    weapon_count+=prob;
  }
}

void BonusBox::GetValueFromAction(Action * a)
{
  ObjBox::GetValueFromAction(a);
  weapon_num = a->PopInt();
  contents = (weapon_map[weapon_num].first)->GetType();
  nbr_ammo = weapon_map[weapon_num].second;
}

void BonusBox::Randomize()
{
  PickRandomWeapon();
}

void BonusBox::StoreValue(Action * a)
{
  ObjBox::StoreValue(a);
  a->Push(weapon_num);
}
