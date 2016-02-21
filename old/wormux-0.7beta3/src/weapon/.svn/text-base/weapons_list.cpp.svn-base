/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "../weapon/weapons_list.h"
//-----------------------------------------------------------------------------
#include "../team/macro.h"
#include "../game/time.h"
#include "../game/game_loop.h"
#include "../object/objects_list.h"
#include "../map/camera.h"
#include "../interface/interface.h"
#include "../weapon/weapon_tools.h"
#include "all.h"
#include <algorithm>
using namespace Wormux;
//-----------------------------------------------------------------------------
WeaponsList weapons_list;
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

WeaponsList::WeaponsList()
{
}

//-----------------------------------------------------------------------------

WeaponsList::~WeaponsList()
{
  weapons_list_it it=todelete.begin(), end=todelete.end();
  for (; it != end; ++it)
  {
    delete *it;
  }
}

//-----------------------------------------------------------------------------

void WeaponsList::InitAndAddToList(Weapon* arme, uint num_sort)
{
  arme->Init();

  // insert the pointer
  m_weapons_list.push_back(arme);

  m_weapons_map.insert(keybind(num_sort, arme));

  interface.weapons_menu.NewItem(arme,num_sort);
}

//-----------------------------------------------------------------------------

void WeaponsList::Init()
{
  weapons_res_profile = resource_manager.LoadXMLProfile( "weapons.xml", false);
  Bazooka* bazooka = new Bazooka;
  todelete.push_back(bazooka);
  InitAndAddToList(bazooka, 1);
  
  InitAndAddToList(&auto_bazooka, 1);
  InitAndAddToList(&lance_grenade, 1);
  InitAndAddToList(&holly_grenade_launcher, 1);
  InitAndAddToList(&lance_cluster, 1);
  InitAndAddToList(&gun, 2);
  InitAndAddToList(&uzi, 2);
  InitAndAddToList(&baseball, 2);  


  Dynamite* dynamite = new Dynamite;
  todelete.push_back(dynamite);
  InitAndAddToList(dynamite,3);
  
  InitAndAddToList(&mine,3);
  InitAndAddToList(&air_attack,4);
  InitAndAddToList(&tux,4);  
  InitAndAddToList(&gnu_launcher,4); 
  InitAndAddToList(&teleportation,5);
  InitAndAddToList(&parachute,5);
  InitAndAddToList(&suicide,5);
  InitAndAddToList(&skipturn,5);
  InitAndAddToList(&jetpack,5);
  InitAndAddToList(&airhammer,5);
  InitAndAddToList(&lowgrav,5);
  InitAndAddToList(&ninjarope,5);
}

//-----------------------------------------------------------------------------

void WeaponsList::Refresh ()
{
  ActiveTeam().AccessWeapon().Manage();
}

//-----------------------------------------------------------------------------

WeaponsList::weapons_list_type& WeaponsList::GetList()
{
  return m_weapons_list;
}

//-----------------------------------------------------------------------------

bool WeaponsList::GetWeaponBySort(uint sort, Weapon_type &type)
{
  uint nb_weapons = m_weapons_map.count(sort);
  if (nb_weapons == 0) return false;
	
  // One or many weapons on this key
  std::pair<std::multimap<uint, Weapon*>::iterator, 
    std::multimap<uint, Weapon*>::iterator> p = m_weapons_map.equal_range(sort);
  
  std::multimap<uint, Weapon*>::iterator 
	it = p.first,
    end = p.second;
  
  // we turn between the differents possibility
  if (nb_weapons > 1)
    {
      while ( (*it).second != &(ActiveTeam().GetWeapon()) && it != end) it++ ;
      
      // the previous selected weapon has been founded
      if (it != end) it++;

      if (it == end) it = p.first;
    } 
  
  // then we selected the "next" one
  type = (*it).second -> GetType();
  return true;
}

//-----------------------------------------------------------------------------

class test_weapon_type {
  private:
	Weapon_type m_type;
  public:
    test_weapon_type(Weapon_type type) { m_type = type; } 
	bool operator() (const Weapon* w) const { return w->GetType()==m_type; }
};

Weapon* WeaponsList::GetWeapon (Weapon_type type)
{
  weapons_list_it it;
  it = std::find_if(m_weapons_list.begin(), m_weapons_list.end(), test_weapon_type(type));
  assert (it != m_weapons_list.end());
  return *it;
}

//-----------------------------------------------------------------------------
