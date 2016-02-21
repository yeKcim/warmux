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
 * Mine : Detect if character is close and explode after a shot time.
 * Sometime the mine didn't explode randomly.
 *****************************************************************************/
#ifndef MINE_H
#define MINE_H

#include <SDL.h>
#include "launcher.h"
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "include/base.h"
#include "object/physical_obj.h"
#include "character/character.h"

class Mine;
class MineConfig;

class ObjMine : public WeaponProjectile
{
  private:
  // channel used for sound
    int channel;

  // this is a fake mine ?
    bool fake;

  // Is this mine active ?
    bool is_active;

  // Activation des mines ?
    bool animation;
    uint attente;
    uint escape_time;

  protected:
    void FakeExplosion();
  public:
    ObjMine(MineConfig &cfg,
            WeaponLauncher * p_launcher = NULL);

    void StartTimeout();
    void Detection();
    virtual bool IsImmobile() const;
    // Damage handling
    virtual void AddDamage(uint damage_points);

    void Draw();
    void Refresh();
};

class MineConfig : public ExplosiveWeaponConfig
{ 
  private:
    static MineConfig * singleton;
  public: 
    uint escape_time;
    double detection_range;
    double speed_detection;

  private:
    MineConfig();
  public:
    static MineConfig * GetInstance();
    virtual void LoadXml(xmlpp::Element *elem);
};

class Mine : public WeaponLauncher
{
  private:
    void Add (int x, int y);
  protected:
    WeaponProjectile * GetProjectileInstance();
    bool p_Shoot();
  public:
    Mine();
    MineConfig& cfg();

    DECLARE_GETWEAPONSTRING();

};

#endif /* MINE_H */
