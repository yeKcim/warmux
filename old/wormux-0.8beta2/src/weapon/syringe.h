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
 * Weapon Syringe
 *****************************************************************************/

#ifndef SYRINGE_H
#define SYRINGE_H
//-----------------------------------------------------------------------------
#include "include/base.h"
#include "weapon.h"
//-----------------------------------------------------------------------------

class SyringeConfig;

class Syringe : public Weapon
{
  protected:
    bool p_Shoot();
    void Refresh() { if (IsInUse()) m_is_active = false; };

    void Draw() { Weapon::Draw(); };

  public:
    Syringe();
    SyringeConfig &cfg();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

//-----------------------------------------------------------------------------
#endif /* SYRINGE_H */
