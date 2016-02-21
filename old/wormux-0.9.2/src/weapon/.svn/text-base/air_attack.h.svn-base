/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#include "weapon/weapon_launcher.h"

class AirAttackConfig;
class Sprite;

class Obus : public WeaponProjectile
{
  private:
    SoundSample falling_sound;
  public:
    Obus(AirAttackConfig& cfg);
    virtual ~Obus();
};

class Plane : public PhysicalObj
{
  private:
    static Obus * last_dropped_bomb;
    friend Obus::~Obus();

    uint nb_dropped_bombs;

    SoundSample flying_sound;

    int obus_dx, obus_dy;
    Sprite *image;
    int next_height;
    int cible_x;
    int distance_to_release;
    AirAttackConfig &cfg;

    bool OnTopOfTarget() const;
    int GetDirection() const;
    void DropBomb();
  public:
    Plane(AirAttackConfig& cfg);
    virtual ~Plane();
    void Shoot(Double speed, const Point2i& target);
    void Draw();
    void Refresh();
};

class AirAttack : public Weapon
{
  private:
    Point2i target;
    bool target_chosen;
  protected:
    bool p_Shoot();
    void p_Select();
    void Refresh() { };
  public:
    AirAttack();
    void ChooseTarget (Point2i mouse_pos);
    void UpdateTranslationStrings();
    std::string GetWeaponWinString(const char *TeamName, uint items_count ) const;
  private:
    AirAttackConfig& cfg();
};

#endif /* AIR_ATTACK_H */
