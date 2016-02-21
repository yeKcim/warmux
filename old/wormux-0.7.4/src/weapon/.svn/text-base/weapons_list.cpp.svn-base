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

#include "weapons_list.h"
//-----------------------------------------------------------------------------
#include <algorithm>
#include "all.h"
#include "weapon_tools.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../interface/interface.h"
#include "../object/objects_list.h"
#include "../map/camera.h"
#include "../team/macro.h"
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

void WeaponsList::AddToList(Weapon* arme, uint num_sort)
{
  // insert the pointer
  m_weapons_list.push_back(arme);
  todelete.push_back(arme);

  m_weapons_map.insert(keybind(num_sort, arme));

  Interface::GetInstance()->weapons_menu.NewItem(arme,num_sort);
}

//-----------------------------------------------------------------------------

void WeaponsList::Init()
{
  weapons_res_profile = resource_manager.LoadXMLProfile( "weapons.xml", false);
  
  Bazooka* bazooka = new Bazooka;
  AddToList(bazooka, 1);
  
  AutomaticBazooka* auto_bazooka = new AutomaticBazooka;
  AddToList(auto_bazooka, 1);

  GrenadeLauncher* grenade_launcher = new GrenadeLauncher;
  AddToList(grenade_launcher, 1);

  HollyGrenadeLauncher* holly_grenade_launcher = new HollyGrenadeLauncher;
  AddToList(holly_grenade_launcher, 1);

  ClusterLauncher* cluster_launcher = new ClusterLauncher;
  AddToList(cluster_launcher, 1);

  Gun* gun = new Gun;
  AddToList(gun, 2);

  Uzi* uzi = new Uzi;
  AddToList(uzi, 2);

  Baseball* baseball = new Baseball;
  AddToList(baseball, 2);  

  Dynamite* dynamite = new Dynamite;
  AddToList(dynamite,3);

  Mine* mine = new Mine;
  AddToList(mine,3);

  AirAttack* air_attack = new AirAttack;
  AddToList(air_attack,4);

  TuxLauncher* tux = new TuxLauncher;
  AddToList(tux,4);  

  GnuLauncher* gnu_launcher = new GnuLauncher;
  AddToList(gnu_launcher,4); 

  BounceBallLauncher* bounce_ball_launcher = new BounceBallLauncher;
  AddToList(bounce_ball_launcher,4); 

  Teleportation* teleportation = new Teleportation;
  AddToList(teleportation,5);

  Parachute* parachute = new Parachute;
  AddToList(parachute,5);

  Suicide* suicide = new Suicide;
  AddToList(suicide,5);

  SkipTurn* skipturn = new SkipTurn;
  AddToList(skipturn,5);

  JetPack* jetpack = new JetPack;
  AddToList(jetpack,5);

  Airhammer* airhammer = new Airhammer;
  AddToList(airhammer,5);

  LowGrav* lowgrav = new LowGrav;
  AddToList(lowgrav,5);

  NinjaRope* ninjarope = new NinjaRope;
  AddToList(ninjarope,5);
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
