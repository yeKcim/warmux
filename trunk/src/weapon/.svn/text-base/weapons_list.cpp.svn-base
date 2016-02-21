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
 * Refresh des armes.
 *****************************************************************************/

#include "weapon/weapons_list.h"
//-----------------------------------------------------------------------------
#include <algorithm>
#include "weapon/all.h"
#include "weapon/explosion.h"
#include "interface/interface.h"
#include "map/camera.h"
#include "map/maps_list.h"
#include "object/objects_list.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "network/randomsync.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

WeaponsList::~WeaponsList()
{
  iterator it=m_weapons_list.begin(), end=m_weapons_list.end();
  for (; it != end; ++it)
    delete *it;

  // no need to delete objects in m_weapons_launcher_list nor clear the 2 lists !
  // no need either to release profile, ResourceManager will do it on exit
  weapons_res_profile = NULL;
}

//-----------------------------------------------------------------------------

WeaponsList::WeaponsList(const xmlNode* weapons_xml)
{
  weapons_res_profile = GetResourceManager().LoadXMLProfile("weapons.xml", false);

  // First launcher weapons
  m_launcher_weapons_list.push_back(new AnvilLauncher);
  m_launcher_weapons_list.push_back(new TuxLauncher);
  m_launcher_weapons_list.push_back(new GnuLauncher);
  m_launcher_weapons_list.push_back(new PolecatLauncher);
  m_launcher_weapons_list.push_back(new BounceBallLauncher);
  m_launcher_weapons_list.push_back(new Bazooka);
  m_launcher_weapons_list.push_back(new AutomaticBazooka);
  m_launcher_weapons_list.push_back(new GrenadeLauncher);
  m_launcher_weapons_list.push_back(new DiscoGrenadeLauncher);
  m_launcher_weapons_list.push_back(new ClusterLauncher);
  m_launcher_weapons_list.push_back(new FootBombLauncher);
  m_launcher_weapons_list.push_back(new RiotBomb);
  m_launcher_weapons_list.push_back(new Cluzooka);
  m_launcher_weapons_list.push_back(new SubMachineGun);
  m_launcher_weapons_list.push_back(new Gun);
  m_launcher_weapons_list.push_back(new Shotgun);
  m_launcher_weapons_list.push_back(new SnipeRifle);
  m_launcher_weapons_list.push_back(new RailGun);
  m_launcher_weapons_list.push_back(new Dynamite);
  m_launcher_weapons_list.push_back(new FlameThrower);
  m_launcher_weapons_list.push_back(new Mine);

  // Copy launcher weapons to normal list
  //m_weapons_list = m_launcher_weapons_list;
  m_weapons_list.insert(m_weapons_list.end(),
                        m_launcher_weapons_list.begin(),
                        m_launcher_weapons_list.end());

  // Add other weapons
  m_weapons_list.push_back(new Baseball);
  m_weapons_list.push_back(new AirAttack);
  m_weapons_list.push_back(new Slap);
  m_weapons_list.push_back(new Teleportation);
  m_weapons_list.push_back(new Parachute);
  m_weapons_list.push_back(new Suicide);
  m_weapons_list.push_back(new SkipTurn);
  m_weapons_list.push_back(new JetPack);
  m_weapons_list.push_back(new Airhammer);
  m_weapons_list.push_back(new Construct);
  m_weapons_list.push_back(new LowGrav);
  m_weapons_list.push_back(new Grapple);
  m_weapons_list.push_back(new Blowtorch);
  m_weapons_list.push_back(new Syringe);

  Init(weapons_xml);
}

void WeaponsList::Init(const xmlNode* weapons_xml) const
{
  for (iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
    (*it)->LoadXml(weapons_xml);
}

bool WeaponsList::Save(XmlWriter& writer, xmlNode* weapons_xml) const
{
  for (iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++) {
    if (!(*it)->SaveXml(writer, weapons_xml))
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

void WeaponsList::UpdateTranslation() const
{
  for (iterator it = m_weapons_list.begin(); it != m_weapons_list.end(); it++)
    (*it)->UpdateTranslationStrings();
}

//-----------------------------------------------------------------------------

bool WeaponsList::GetWeaponBySort(Weapon::category_t sort, Weapon::Weapon_type &type)
{
  iterator it, end=m_weapons_list.end();
  bool open = ActiveMap()->LoadedData()->IsOpened();

  /* find the current position */
  it = std::find(m_weapons_list.begin(),
                 m_weapons_list.end(),
                 &ActiveTeam().GetWeapon());

  /* if the current weapon match the criteria */
  if (it != end && ActiveTeam().GetWeapon().Category() == sort) {
    /* try to find the next weapon matching our criteria */
    do {
      ++it;
    } while( it != end
             && ((*it)->Category() != sort
                 || ActiveTeam().ReadNbAmmos((*it)->GetType()) == 0
                 || (!((*it)->CanBeUsedOnClosedMap()) && !open)) );

    /* Ok, a weapon was found let's return it */
    if (it != end && (*it)->Category() == sort) {
      type = (*it)->GetType();
      return true;
    }
  }
  /* we didn't find a valid weapon after the current one ; lets wrap:
   * restart from the begining and try to find the first one matching
   * our criteria */
  it = m_weapons_list.begin();
  while(it != end && (*it)->Category() != sort)
    ++it;

  /* no weapon of this sort was found -> give up... */
  if (it == end)
    return false;

  /* try to find the next weapon matching our criteria */
  while(it != end
        && ((*it)->Category() != sort
            || ActiveTeam().ReadNbAmmos((*it)->GetType()) == 0
            || (!(*it)->CanBeUsedOnClosedMap() && open)))
    ++it;

  /* Ok, a weapon was found let's return it if it is not the one active */
  if (it != end && (*it)->Category() == sort && (*it) != &ActiveTeam().GetWeapon()) {
    type = (*it)->GetType();
    return true;
  }

  /* we definitly found nothing... */
  return false;
}

Weapon * WeaponsList::GetRandomWeaponToDrop()
{
  std::list<Weapon*>::iterator it;
  Double probability_sum = 0;
  for (it = m_weapons_list.begin(); it != m_weapons_list.end(); ++it) {
    probability_sum += (*it)->GetDropProbability();
  }
  ASSERT(probability_sum > 0);

  MSG_DEBUG("random.get", "WeaponList::GetRandomWeaponToDrop()");
  Double num = RandomSync().GetDouble(0, probability_sum);
  Double total_bf_weapon = 0;
  Double total_after_weapon = 0;

  for (it = m_weapons_list.begin(); it != m_weapons_list.end(); it++) {
    Weapon * weapon = *it;
    total_after_weapon = total_bf_weapon + weapon->GetDropProbability();
    if (total_bf_weapon < num && num <= total_after_weapon) {
      MSG_DEBUG("bonus","Weapon choosed: %s", weapon->GetName().c_str());
      return weapon;
    }
    total_bf_weapon = total_after_weapon;
  }
  ASSERT(false);
  return NULL;
}

//-----------------------------------------------------------------------------

class test_weapon_type
{
  Weapon::Weapon_type m_type;
public:
  test_weapon_type(const Weapon::Weapon_type &type) : m_type(type){ }
  bool operator() (const Weapon* w) const { return w->GetType()==m_type; }
};

Weapon* WeaponsList::GetWeapon(Weapon::Weapon_type type) const
{
  iterator it;
  it = std::find_if(m_weapons_list.begin(), m_weapons_list.end(), test_weapon_type(type));
  ASSERT (it != m_weapons_list.end());
  return *it;
}

WeaponLauncher* WeaponsList::GetWeaponLauncher(Weapon::Weapon_type type) const
{
  launcher_iterator it;
  it = std::find_if(m_launcher_weapons_list.begin(), m_launcher_weapons_list.end(), test_weapon_type(type));
  ASSERT (it != m_launcher_weapons_list.end());
  return *it;
}

//-----------------------------------------------------------------------------
