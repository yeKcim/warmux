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
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

WeaponsList::~WeaponsList()
{
  weapons_list_it it=m_weapons_list.begin(), end=m_weapons_list.end();
  for (; it != end; ++it)
    delete *it;

  delete weapons_res_profile;
  weapons_res_profile = NULL;
}

//-----------------------------------------------------------------------------

WeaponsList::WeaponsList()
{
  weapons_res_profile = GetResourceManager().LoadXMLProfile( "weapons.xml", false);
  m_weapons_list.push_back(new Bazooka);
  m_weapons_list.push_back(new SubMachineGun);
  m_weapons_list.push_back(new Gun);
  m_weapons_list.push_back(new Shotgun);
  m_weapons_list.push_back(new SnipeRifle);
  m_weapons_list.push_back(new RiotBomb);
  m_weapons_list.push_back(new Cluzooka);
  m_weapons_list.push_back(new AutomaticBazooka);
  m_weapons_list.push_back(new Dynamite);
  m_weapons_list.push_back(new GrenadeLauncher);
  m_weapons_list.push_back(new DiscoGrenadeLauncher);
  m_weapons_list.push_back(new ClusterLauncher);
  m_weapons_list.push_back(new FootBombLauncher);
  m_weapons_list.push_back(new FlameThrower);
  m_weapons_list.push_back(new Baseball);
  m_weapons_list.push_back(new Mine);
  m_weapons_list.push_back(new AirAttack);
  m_weapons_list.push_back(new AnvilLauncher);
  m_weapons_list.push_back(new TuxLauncher);
  m_weapons_list.push_back(new GnuLauncher);
  m_weapons_list.push_back(new PolecatLauncher);
  m_weapons_list.push_back(new BounceBallLauncher);
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
}

//-----------------------------------------------------------------------------

void WeaponsList::Refresh () const
{
  ActiveTeam().AccessWeapon().Manage();
}

void WeaponsList::UpdateTranslation()
{
  if (singleton == NULL) return;
  weapons_list_it it = GetInstance()->m_weapons_list.begin(), end=GetInstance()->m_weapons_list.end();
  for (; it != end; it++) {
    (*it)->UpdateTranslationStrings();
  }
}

//-----------------------------------------------------------------------------

bool WeaponsList::GetWeaponBySort(Weapon::category_t sort, Weapon::Weapon_type &type)
{
  weapons_list_it it, end=m_weapons_list.end();

  /* find the current position */
  it = find(m_weapons_list.begin(),
            m_weapons_list.end(),
            &ActiveTeam().GetWeapon());

  /* if the current weapon match the criteria */
  if (it != end && ActiveTeam().GetWeapon().Category() == sort)
    {
      /* try to find the next weapon matching our criteria */
      do {
        ++it;
      } while(it != end
              && ((*it)->Category() != sort
                  || ActiveTeam().ReadNbAmmos((*it)->GetType()) == 0
                  || (!((*it)->CanBeUsedOnClosedMap()) && !(ActiveMap()->IsOpened())))
              );

      /* Ok, a weapon was found let's return it */
      if (it != end && (*it)->Category() == sort)
        {
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
        || (!(*it)->CanBeUsedOnClosedMap() && ActiveMap()->IsOpened())))
    ++it;

  /* Ok, a weapon was found let's return it if it is not the one active */
  if (it != end && (*it)->Category() == sort && (*it) != &ActiveTeam().GetWeapon())
    {
      type = (*it)->GetType();
      return true;
    }

  /* we definitly found nothing... */
  return false;
}

//-----------------------------------------------------------------------------

class test_weapon_type {
  private:
    Weapon::Weapon_type m_type;
  public:
    test_weapon_type(const Weapon::Weapon_type &type) :  m_type(type){ }
    bool operator() (const Weapon* w) const { return w->GetType()==m_type; }
};

Weapon* WeaponsList::GetWeapon (Weapon::Weapon_type type)
{
  weapons_list_it it;
  it = std::find_if(m_weapons_list.begin(), m_weapons_list.end(), test_weapon_type(type));
  ASSERT (it != m_weapons_list.end());
  return *it;
}

//-----------------------------------------------------------------------------
