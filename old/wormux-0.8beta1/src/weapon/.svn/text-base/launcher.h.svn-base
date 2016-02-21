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
 * WeaponLauncher: generic weapon to launch a projectile
 *****************************************************************************/

#ifndef WEAPON_LAUNCHER_H
#define WEAPON_LAUNCHER_H
#include "weapon.h"
#include "graphic/surface.h"
#include "gui/progress_bar.h"
#include "include/base.h"
#include "object/physical_obj.h"

class WeaponLauncher;

class WeaponProjectile : public PhysicalObj
{
  protected:
    Sprite *image;
    bool camera_follow_closely;
    bool explode_colliding_character; // before timeout.
    bool explode_with_timeout;
    bool explode_with_collision;
    double begin_time;

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
    virtual void Refresh();
    virtual void Shoot(double strength);
    virtual bool IsImmobile() const;

    void IncrementTimeOut();
    void DecrementTimeOut();
    void SetTimeOut(int timeout);
    int GetTotalTimeout() const;
    void ResetTimeOut();
    bool change_timeout_allowed();
  protected:
    virtual void SignalObjectCollision(PhysicalObj * obj);
    virtual void SignalGroundCollision();
    virtual void SignalCollision();
    virtual void SignalOutOfMap();
    virtual void SignalTimeout();
    virtual void SignalExplosion();
    void SignalDrowning();
    void SignalGhostState (bool was_dead);

    virtual void ShootSound();
    virtual void Explosion();
    virtual void RandomizeShoot(double &angle,double &strength);
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
    virtual void SignalGroundCollision();
    virtual void SignalOutOfMap();
    virtual void SignalObjectCollision(PhysicalObj * obj);
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
    void Refresh();
  private:
    void DirectExplosion();
    void NetworkSetTimeoutProjectile();
  public:
    WeaponLauncher(Weapon_type type,
                   const std::string &id,
                   EmptyWeaponConfig * params,
                   weapon_visibility_t visibility = ALWAYS_VISIBLE);
    virtual ~WeaponLauncher();

    virtual void Draw();

    // Handle of projectile events
    virtual void SignalProjectileExplosion();
    virtual void SignalProjectileCollision();
    virtual void SignalProjectileDrowning();
    virtual void SignalProjectileGhostState();
    virtual void SignalProjectileTimeout();

    void IncActiveProjectile();
    void DecActiveProjectile();

    virtual void IncMissedShots();

    // Handle mouse events
    virtual void HandleMouseWheelUp();
    virtual void HandleMouseWheelDown();

    // Handle special keys
    virtual void HandleKeyReleased_Num1();
    virtual void HandleKeyReleased_Num2();
    virtual void HandleKeyReleased_Num3();
    virtual void HandleKeyReleased_Num4();
    virtual void HandleKeyReleased_Num5();
    virtual void HandleKeyReleased_Num6();
    virtual void HandleKeyReleased_Num7();
    virtual void HandleKeyReleased_Num8();
    virtual void HandleKeyReleased_Num9();
    virtual void HandleKeyReleased_Less();
    virtual void HandleKeyReleased_More();
  
    WeaponProjectile* GetProjectile() { return projectile; };
    ExplosiveWeaponConfig& cfg();
};

#endif /* WEAPON_LAUNCHER_H */
