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
 * Weapon Supertux : Look ! it's the famous flying magic pinguin !
 *****************************************************************************/

#ifndef SUPERTUX_H
#define SUPERTUX_H

#include "weapon/weapon_launcher.h"
#include "particles/particle.h"

class SuperTux;

class SuperTuxWeaponConfig;

class TuxLauncher : public WeaponLauncher
{
  private:
    SuperTux * current_tux;

    friend class SuperTux;
    void EndOfTurn() const; // should be called only by SuperTux
  public:
    TuxLauncher();
    bool IsInUse() const;

    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;

    void SignalEndOfProjectile();
    void HandleKeyPressed_MoveRight(bool shift);
    void HandleKeyRefreshed_MoveRight(bool shift);
    void HandleKeyReleased_MoveRight(bool shift);
    void HandleKeyPressed_MoveLeft(bool shift);
    void HandleKeyRefreshed_MoveLeft(bool shift);
    void HandleKeyReleased_MoveLeft(bool shift);

    void RefreshFromNetwork(double angle, Point2d pos);

  protected:
    WeaponProjectile * GetProjectileInstance();
    bool p_Shoot();
  private:
    SuperTuxWeaponConfig& cfg();
};

#endif
