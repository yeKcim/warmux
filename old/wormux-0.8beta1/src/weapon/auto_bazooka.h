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
#include "launcher.h"
#include "graphic/surface.h"
#include "gui/progress_bar.h"
#include "include/base.h"
#include "object/physical_obj.h"

class AutomaticBazooka;
class AutomaticBazookaConfig;

class RPG : public WeaponProjectile
{
  ParticleEngine smoke_engine;
  protected:
    double angle_local;
    Point2i m_target;
    bool m_targeted;
    double m_force;
    uint m_lastrefresh;
  public:
    RPG(AutomaticBazookaConfig& cfg,
                        WeaponLauncher * p_launcher);
    void Refresh();
    void Shoot(double strength);
    void SetTarget (int x,int y);

  protected:
    void SignalOutOfMap();
    void SignalDrowning();
};

class AutomaticBazooka : public WeaponLauncher
{
  private:
    struct target_t
    {
      Point2i pos;
      bool selected;
      Surface image;
    } m_target;
  public:
    AutomaticBazooka();
    void Draw ();
    bool IsReady() const;
    virtual void ChooseTarget(Point2i mouse_pos);
    AutomaticBazookaConfig &cfg();
    DECLARE_GETWEAPONSTRING();
  protected:
    void Refresh();
    void p_Select(); 
    void p_Deselect();

    void DrawTarget();

  protected:
    WeaponProjectile * GetProjectileInstance();
};

#endif /* AUTO_BAZOOKA_H */
