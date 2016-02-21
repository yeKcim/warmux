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
 * Bonus Box
 *****************************************************************************/

#ifndef BONUS_BOX_H
#define BONUS_BOX_H
//-----------------------------------------------------------------------------
#include <WARMUX_base.h>
#include "object/objbox.h"

// Forward declarations
class Character;
class Action;
class Weapon;
//-----------------------------------------------------------------------------

class BonusBox : public ObjBox
{
  Weapon * weapon;
  static Sprite* icon;
  static int icon_ref;

  bool ExplodesInsteadOfBonus(Character * c);
  void ApplyBonus (Team &team, Character &character);
public:
  BonusBox(Weapon * weapon);
  ~BonusBox();

  void ApplyBonus(Character *);
  const Surface* GetIcon() const;
};

//-----------------------------------------------------------------------------
#endif /* BONUS_BOX_H */
