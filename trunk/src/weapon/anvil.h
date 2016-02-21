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
 * Anvil : appear in top of an enemy and crush down his head
 *****************************************************************************/

#ifndef ANVIL_H
#define ANVIL_H

#include <WARMUX_base.h>
#include "weapon/target_launcher.h"

class AnvilLauncher : public TargetLauncher
{
public:
  AnvilLauncher();
  void ChooseTarget(Point2i mouse_pos);
  void UpdateTranslationStrings();
  std::string GetWeaponWinString(const char *TeamName, uint items_count) const;
protected:
  WeaponProjectile * GetProjectileInstance();
  virtual bool p_Shoot();
  //virtual bool ShouldBeDrawn() { return !IsOnCooldownFromShot(); }
};

#endif /* ANVIL_H */
