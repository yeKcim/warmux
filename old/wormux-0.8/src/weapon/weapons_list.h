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

#ifndef WEAPONS_LIST_H
#define WEAPONS_LIST_H
//-----------------------------------------------------------------------------
#include <list>
#include <map>
#include "weapon.h"
#include "include/base.h"
#include "include/singleton.h"
//-----------------------------------------------------------------------------

// Classe de gestion des armes
class WeaponsList : public Singleton<WeaponsList>
{
public:
  typedef std::list<Weapon*> weapons_list_type;
  typedef std::list<Weapon*>::const_iterator weapons_list_it;

private:
  weapons_list_type m_weapons_list;
  Weapon* GetNextWeapon(uint sort, uint index);

protected:
  friend class Singleton<WeaponsList>;
  /* if you need to use this, implement it */
  WeaponsList(const WeaponsList &a_list);
  WeaponsList();
  ~WeaponsList();

public:
  void Init();

  // Weapon refresh
  // Return true if end of turn
  void Refresh() const;

  static void UpdateTranslation();

  // Return a list of  weapons
  const weapons_list_type& GetList() const { return m_weapons_list; };
  Weapon* GetWeapon(Weapon::Weapon_type type);
  bool GetWeaponBySort(Weapon::category_t num_sort, Weapon::Weapon_type &type);
};

//-----------------------------------------------------------------------------
#endif
