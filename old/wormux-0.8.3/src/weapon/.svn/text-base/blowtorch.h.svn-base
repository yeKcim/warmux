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
 * Blowtorch - burns holes into walls
 *****************************************************************************/

#ifndef __BLOWTORCH_H__
#define __BLOWTORCH_H__

#include "weapon.h"

class BlowtorchConfig;

class Blowtorch : public Weapon
{
  protected:
    bool p_Shoot();
    void p_Deselect();
    void Refresh() { };

    void RepeatShoot() const ;
  public:
    Blowtorch();
    BlowtorchConfig& cfg();

    virtual void SignalTurnEnd() { p_Deselect(); };
    virtual void ActionStopUse();

    virtual void HandleKeyPressed_Shoot(bool shift);
    virtual void HandleKeyRefreshed_Shoot(bool shift);
    virtual void HandleKeyReleased_Shoot(bool) { NewActionWeaponStopUse(); };
    bool IsInUse() const;

    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
};

#endif  // __BLOWTORCH_H__
