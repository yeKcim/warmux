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
 * Medkit
 *****************************************************************************/

#ifndef MEDKIT_H
#define MEDKIT_H
//-----------------------------------------------------------------------------
#include <WARMUX_base.h>
#include "object/objbox.h"
#include "tool/config_element.h"

class Team;
class Character;
class Action;
class XmlWriter;

//-----------------------------------------------------------------------------
class MedkitSettings : public ConfigElementList
{
public:
  int nbr_health;
  int start_points;
  MedkitSettings()
  {
    push_back(new IntConfigElement("life_points", &start_points, 41));
    push_back(new IntConfigElement("energy_boost", &nbr_health, 24));
  }
};

class Medkit : public ObjBox
{
  static Sprite* icon;
  static int icon_ref;
  static MedkitSettings settings;

  void ApplyMedkit(Team &team, Character &character) const;
public:
  Medkit();
  ~Medkit();

  static ConfigElementList* GetConfigList() { return &settings; }
  void ApplyBonus(Character *);
  const Surface* GetIcon() const;
};

//-----------------------------------------------------------------------------
#endif /* MEDKIT_H */
