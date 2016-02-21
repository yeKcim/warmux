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

#ifndef WEAPONS_LIST_H
#define WEAPONS_LIST_H
//-----------------------------------------------------------------------------
#include <list>
#include <map>
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>

#include "weapon.h"
//-----------------------------------------------------------------------------

class WeaponLauncher;
class XmlWriter;

// Classe de gestion des armes
class WeaponsList
{
public:
  typedef std::list<Weapon*> weapons_list_type;
  typedef std::list<Weapon*>::const_iterator iterator;
  typedef std::list<WeaponLauncher*> launcher_weapons_list_type;
  typedef std::list<WeaponLauncher*>::const_iterator launcher_iterator;

private:
  weapons_list_type m_weapons_list;
  launcher_weapons_list_type m_launcher_weapons_list;
  Weapon* GetNextWeapon(uint sort, uint index);

protected:
  /* if you need to use this, implement it */
  WeaponsList(const WeaponsList &a_list);

public:
  WeaponsList(const xmlNode* weapons_xml);
  ~WeaponsList();

  void Init(const xmlNode* weapons_xml) const;
  bool Save(XmlWriter& writer, xmlNode* weapons_xml) const;

  void UpdateTranslation() const;

  // Return a list of  weapons
  const weapons_list_type& GetList() const { return m_weapons_list; };
  Weapon* GetWeapon(Weapon::Weapon_type type) const;
  WeaponLauncher* GetWeaponLauncher(Weapon::Weapon_type type) const;
  bool GetWeaponBySort(Weapon::category_t num_sort, Weapon::Weapon_type &type);
  Weapon * GetRandomWeaponToDrop();
};

//-----------------------------------------------------------------------------
#endif
