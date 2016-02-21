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
 * Air attack.
 *****************************************************************************/

#ifndef AIR_ATTACK_H
#define AIR_ATTACK_H
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "include/base.h"
#include "launcher.h"

class AirAttack;

class AirAttackConfig : public ExplosiveWeaponConfig
{ 
  public:
    double speed;
    uint nbr_obus; 
  public:
    AirAttackConfig();
    virtual void LoadXml(xmlpp::Element *elem);
};

class Obus : public WeaponProjectile
{
  public:
    Obus(AirAttackConfig& cfg);
};

class Plane : public PhysicalObj
{
  private:
    uint nb_dropped_bombs;
    Obus * last_dropped_bomb;

    int obus_dx, obus_dy;
    Sprite *image;

    int cible_x;
    int distance_to_release;
    AirAttackConfig &cfg;

    bool OnTopOfTarget() const;
    int GetDirection() const;
    void DropBomb();

  public:
    Plane(AirAttackConfig& cfg);
    void Shoot(double speed, Point2i& target);
    void Draw();
    void Refresh();
};

class AirAttack : public Weapon
{
  private:
    Point2i target;
    bool target_chosen;
    //Plane plane;
  protected:
    bool p_Shoot();
    void p_Select();
    void p_Deselect();
    void Refresh();
  public:
    AirAttack();
    virtual void ChooseTarget (Point2i mouse_pos);
    DECLARE_GETWEAPONSTRING();

  private:
    AirAttackConfig& cfg();
};

#endif /* AIR_ATTACK_H */
