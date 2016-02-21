/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
#include "explosion.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../interface/interface.h"
#include "../object/objects_list.h"
#include "../map/camera.h"
#include "../team/macro.h"
#include "../map/maps_list.h"
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
  weapons_list_it it=m_weapons_list.begin(), end=m_weapons_list.end();
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

  m_weapons_map.insert(keybind(num_sort, arme));

  Interface::GetInstance()->weapons_menu.NewItem(arme,num_sort);
}

//-----------------------------------------------------------------------------

void WeaponsList::Init()
{
  weapons_res_profile = resource_manager.LoadXMLProfile( "weapons.xml", false);
  Bazooka* bazooka = new Bazooka;
  SubMachineGun* submachine_gun = new SubMachineGun;
  Gun* gun = new Gun;
  Shotgun* shotgun = new Shotgun;
  SnipeRifle* snipe_rifle = new SnipeRifle;
  RiotBomb* riot_bomb = new RiotBomb;
  AutomaticBazooka* auto_bazooka = new AutomaticBazooka;
  Dynamite* dynamite = new Dynamite;
  GrenadeLauncher* grenade_launcher = new GrenadeLauncher;
  HollyGrenadeLauncher* holly_grenade_launcher = new HollyGrenadeLauncher;
  ClusterLauncher* cluster_launcher = new ClusterLauncher;
  Baseball* baseball = new Baseball;
  Mine* mine = new Mine;
  AirAttack* air_attack = new AirAttack;
  AnvilLauncher* anvil = new AnvilLauncher;
  TuxLauncher* tux = new TuxLauncher;
  GnuLauncher* gnu_launcher = new GnuLauncher;
  PolecatLauncher* polecat_launcher = new PolecatLauncher;
  BounceBallLauncher* bounce_ball_launcher = new BounceBallLauncher;
  Teleportation* teleportation = new Teleportation;
  Parachute* parachute = new Parachute;
  Suicide* suicide = new Suicide;
  SkipTurn* skipturn = new SkipTurn;
  JetPack* jetpack = new JetPack;
  Airhammer* airhammer = new Airhammer;
  Construct* construct = new Construct;
  LowGrav* lowgrav = new LowGrav;
  NinjaRope* ninjarope = new NinjaRope;
  Blowtorch* blowtorch = new Blowtorch;
  Syringe* syringe = new Syringe;

  // Category 1
  AddToList(bazooka, 1);
  AddToList(submachine_gun, 1);
  AddToList(snipe_rifle, 1);
  AddToList(gun, 1);
  AddToList(shotgun, 1);
  AddToList(riot_bomb, 1);
  AddToList(auto_bazooka, 1);

  // Category 2
  AddToList(dynamite,2);
  AddToList(grenade_launcher, 2);
  AddToList(cluster_launcher, 2);
  AddToList(holly_grenade_launcher, 2);
  AddToList(mine,2);

  // Category 3
  AddToList(baseball, 3);
  AddToList(syringe,3);
  AddToList(tux,3);
  AddToList(gnu_launcher,3);
  AddToList(polecat_launcher,3);
  AddToList(air_attack,3);
  AddToList(anvil,3);
  AddToList(bounce_ball_launcher,3);

  // Category 4
  AddToList(ninjarope,4);
  AddToList(jetpack,4);
  AddToList(parachute,4);
  AddToList(teleportation,4);
  AddToList(lowgrav,4);

  // Category 5
  AddToList(skipturn,5);
  AddToList(airhammer,5);
  AddToList(construct,5);
  AddToList(blowtorch,5);
  AddToList(suicide,5);
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

Weapon* WeaponsList::GetNextWeapon(uint sort, uint index)
{
	uint n = m_weapons_map.count(sort);
	index = index % n;  // index can't be greater than number of weapons
	weapons_map_it it = m_weapons_map.lower_bound(sort);

	if(index + 1 < n)
	{
		for(uint i=0; i < index + 1; i++)
			it++;
	}

	return it->second;
}

//-----------------------------------------------------------------------------

bool WeaponsList::GetWeaponBySort(uint sort, Weapon_type &type)
{
  uint nb_weapons = m_weapons_map.count(sort);
  if (nb_weapons == 0) return false;

  // One or many weapons on this key
  std::pair<weapons_map_it, weapons_map_it> p = m_weapons_map.equal_range(sort);
  weapons_map_it it = p.first, end = p.second;

  Weapon* next_weapon = it->second;

  if (nb_weapons > 1)
    {
    	// Find index of current weapon
    	uint current_weapon;
    	for(current_weapon=0; current_weapon < nb_weapons-1; current_weapon++, it++)
	{
		if(it->second == &(ActiveTeam().GetWeapon()))
			break;
	}

	// Get next weapon that has enough ammo and can be used on the map
	uint i = 0;
	do
	{
		next_weapon = GetNextWeapon(sort, current_weapon++);
	} while(i++ < nb_weapons && (ActiveTeam().ReadNbAmmos(next_weapon->GetName()) == 0 || !(next_weapon->CanBeUsedOnClosedMap() || ActiveMap().IsOpened())));
	// this corresponds to:  while (stop-condition && (not-enoughammo || not-usable-on-map))

	// no right weapon has been found
	if(i > nb_weapons)
		return false;
    }

  type = next_weapon->GetType();
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
