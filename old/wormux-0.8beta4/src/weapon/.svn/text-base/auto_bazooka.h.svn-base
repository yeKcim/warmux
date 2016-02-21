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
 * auto bazooka : launch a homing missile
 *****************************************************************************/

#ifndef AUTO_BAZOOKA_H
#define AUTO_BAZOOKA_H

#include "weapon/weapon_launcher.h"
#include "include/base.h"

class AutomaticBazookaConfig;
struct target_t;

class AutomaticBazooka : public WeaponLauncher
{
  target_t       *m_target;
  public:
    AutomaticBazooka();
    ~AutomaticBazooka();
    void Draw ();
    bool IsReady() const;
    virtual void ChooseTarget(Point2i mouse_pos);
    AutomaticBazookaConfig &cfg();
    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
  protected:
    void Refresh();
    void p_Select();
    void p_Deselect();

    void DrawTarget() const;

    WeaponProjectile * GetProjectileInstance();
};

#endif /* AUTO_BAZOOKA_H */
