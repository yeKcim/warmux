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
 * WeaponLauncher: generic weapon to launch a projectile
 *****************************************************************************/

#ifndef WEAPON_LAUNCHER_H
#define WEAPON_LAUNCHER_H

#include "weapon.h"
#include "object/physical_obj.h"

class Sprite;
class WeaponLauncher;
class ExplosiveWeaponConfig;

class WeaponProjectile : public PhysicalObj
{
  protected:
    Sprite *image;
    bool explode_colliding_character; // before timeout.
    bool explode_with_timeout;
    bool explode_with_collision;
    bool can_drown;
    bool camera_in_advance;
    uint begin_time;

    ExplosiveWeaponConfig& cfg;

  public:
    Character* dernier_ver_touche;
    PhysicalObj* dernier_obj_touche;
    WeaponLauncher * launcher;
    int m_timeout_modifier;

  public:
    WeaponProjectile(const std::string &nom,
                     ExplosiveWeaponConfig& cfg,
                     WeaponLauncher * p_launcher);
    virtual ~WeaponProjectile();

    virtual void Draw();
    // Call by the object list class to refresh the weapon's state
    virtual void Refresh();
    virtual void Shoot(double strength);
    virtual bool IsImmobile() const;
    virtual void SetEnergyDelta(int delta, bool do_report = true);

    void IncrementTimeOut();
    void DecrementTimeOut();
    void SetTimeOut(int timeout);
    int GetTotalTimeout() const;
    void ResetTimeOut() { m_timeout_modifier = 0; };
    bool change_timeout_allowed() const;
  protected:
    virtual void SignalObjectCollision(PhysicalObj * obj, const Point2d& my_speed_before);
    virtual void SignalGroundCollision(const Point2d& speed_before);
    virtual void SignalCollision(const Point2d& speed_before);
    virtual void SignalOutOfMap();
    virtual void SignalTimeout();
    virtual void SignalExplosion();
    virtual void SignalDrowning();
    virtual void SignalGoingOutOfWater();
    void SignalGhostState (bool was_dead);

    virtual void ShootSound();
    virtual void Explosion();
    // Implement it in subclass to randomize fire
    virtual void RandomizeShoot(double &/*angle*/,double &/*strength*/) { };
    virtual void DoExplosion();
};

class WeaponBullet : public WeaponProjectile
{
  public:
    WeaponBullet(const std::string &name,
                 ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher);
    virtual ~WeaponBullet(){};
    virtual void Refresh();
  protected:
    virtual void SignalGroundCollision(const Point2d& speed_before);
    virtual void SignalOutOfMap();
    virtual void SignalObjectCollision(PhysicalObj * obj, const Point2d& my_speed_before);
    void DoExplosion();
};


class WeaponLauncher : public Weapon
{
  public:
    bool ignore_timeout_signal;
    bool ignore_collision_signal;
    bool ignore_explosion_signal;
    bool ignore_ghost_state_signal;
    bool ignore_drowning_signal;
    bool ignore_going_out_of_water_signal;
  protected:
    WeaponProjectile * projectile;
    uint nb_active_projectile;
    bool m_allow_change_timeout;
    int missed_shots;
    bool announce_missed_shots;
  protected:
    virtual bool p_Shoot();
    virtual void p_Select();
    virtual WeaponProjectile * GetProjectileInstance() = 0;
    virtual bool ReloadLauncher();
    void Refresh() { };
  private:
    void DirectExplosion();
    void NetworkSetTimeoutProjectile() const;
  public:
    WeaponLauncher(Weapon_type type,
                   const std::string &id,
                   EmptyWeaponConfig * params,
                   weapon_visibility_t visibility = ALWAYS_VISIBLE);
    virtual ~WeaponLauncher();

    virtual void Draw();

    std::string GetWeaponWinString(const char *TeamName, uint items_count)const = 0;

    // Handle of projectile events
    // Signal the end of a projectile for any reason possible
    virtual void SignalEndOfProjectile() { DecActiveProjectile(); };
    // Signal that a projectile explosion
    virtual void SignalProjectileExplosion() { SignalEndOfProjectile(); };
    // Signal that a projectile fired by this weapon has hit something (ground, character etc)
    virtual void SignalProjectileCollision(const Point2d& /*speed_before*/) { SignalEndOfProjectile(); };
    // Signal a projectile is drowning
    virtual void SignalProjectileDrowning() { SignalEndOfProjectile(); };
    // Signal a projectile is fishing out of water (supertux)
    virtual void SignalProjectileGoingOutOfWater() {};
    // Signal a ghost state
    virtual void SignalProjectileGhostState() { SignalEndOfProjectile(); };
    // Signal a projectile timeout (for exemple: grenade, disco grenade ... etc.)
    virtual void SignalProjectileTimeout() { SignalEndOfProjectile(); };

    // Keep the total amount of active projectile
    void IncActiveProjectile() { ++nb_active_projectile; };
    void DecActiveProjectile() { --nb_active_projectile; };

    virtual void IncMissedShots();
    virtual bool IsInUse() const;
    // Handle mouse events
    virtual void HandleMouseWheelUp(bool shift);
    virtual void HandleMouseWheelDown(bool shift);

    // Handle special keys
    virtual void HandleKeyReleased_Num1(bool shift);
    virtual void HandleKeyReleased_Num2(bool shift);
    virtual void HandleKeyReleased_Num3(bool shift);
    virtual void HandleKeyReleased_Num4(bool shift);
    virtual void HandleKeyReleased_Num5(bool shift);
    virtual void HandleKeyReleased_Num6(bool shift);
    virtual void HandleKeyReleased_Num7(bool shift);
    virtual void HandleKeyReleased_Num8(bool shift);
    virtual void HandleKeyReleased_Num9(bool shift);
    virtual void HandleKeyReleased_Less(bool shift);
    virtual void HandleKeyReleased_More(bool shift);

    WeaponProjectile* GetProjectile() { return projectile; };
    ExplosiveWeaponConfig& cfg();
};

#endif /* WEAPON_LAUNCHER_H */
