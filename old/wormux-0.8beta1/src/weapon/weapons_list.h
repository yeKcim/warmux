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
 * Refresh des armes.
 *****************************************************************************/

#ifndef WEAPONS_LIST_H
#define WEAPONS_LIST_H
//-----------------------------------------------------------------------------
#include <list>
#include <map>
#include "weapon.h"
#include "include/base.h"
//-----------------------------------------------------------------------------

// Classe de gestion des armes
class WeaponsList
{
public:
  typedef std::list<Weapon*> weapons_list_type;
  typedef std::list<Weapon*>::iterator weapons_list_it;

private:
  static WeaponsList * weapons_list; /* list by itself */
  /* reference counter on the list WARNING not thread safe */
  int ref_counter;

  WeaponsList();
  weapons_list_type m_weapons_list;

  Weapon* GetNextWeapon(uint sort, uint index);

  /* if you need to use this, implement it */
  WeaponsList(const WeaponsList &a_list);


public:
  ~WeaponsList();
  void Init();
  static WeaponsList * GetInstance();

  // Refresh des armes
  // Retourne true si c'est la fin d'un tour
  void Refresh();

  // Return a list of  weapons
  weapons_list_type& GetList();
  Weapon* GetWeapon(Weapon::Weapon_type type);
  bool GetWeaponBySort(Weapon::category_t num_sort, Weapon::Weapon_type &type);
};

//-----------------------------------------------------------------------------
#endif
