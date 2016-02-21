/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Classes virtuelles permettant de d�inir une arme et un projectile. Les
 * armes ont un nom, une image, un �at actif/inactif et une ic�e (affich�
 * dans l'interface). Les projectiles sont des objets physiques qui ont un
 * comportement sp�ial lorsqu'ils entrent en collision ou qu'ils sortent du
 * terrain.
 *****************************************************************************/

#ifndef WEAPON_H
#define WEAPON_H
#include <string>
#include "weapon_cfg.h"
#include "../graphic/surface.h"
#include "../graphic/sprite.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../include/enum.h"
#include "../particles/particle.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
//#include "../character/character.h"
class Character;

// Infinite ammos constant
extern const int INFINITE_AMMO;

extern const uint BUTTON_ICO_WIDTH;
extern const uint BUTTON_ICO_HEIGHT;

extern const uint WEAPON_ICO_WIDTH;
extern const uint WEAPON_ICO_HEIGHT;

class WeaponStrengthBar : public BarreProg
{
 public:
  bool visible ;
} ;

//-----------------------------------------------------------------------------

class Weapon
{
protected:
  Weapon_type m_type;
  std::string m_id;
  std::string m_name;
  bool m_is_active;
  Sprite *m_image;
  Sprite *m_weapon_fire;
  uint m_fire_remanence_time;

  typedef enum {
    weapon_origin_HAND,
    weapon_origin_OVER
  } weapon_origin_t;
  weapon_origin_t origin;

  Point2i hole_delta; // relative position of the hole of the weapon
  Point2i position;   // Position of the weapon

  // Time when the weapon is selected for the animation
  uint m_time_anim_begin;

  // Actual strength of the weapon
  double m_strength;

  // time of beginning to load (for choosing the strength)
  uint m_first_time_loading;

  // time of the last fire
  uint m_last_fire_time;

  // change weapon after ? (for the ninja cord = true)
  bool m_can_change_weapon;

  // Extra parameters
  EmptyWeaponConfig *extra_params;

  typedef enum weapon_visibility {
    ALWAYS_VISIBLE,
    NEVER_VISIBLE,
    VISIBLE_ONLY_WHEN_ACTIVE,
    VISIBLE_ONLY_WHEN_INACTIVE
  } weapon_visibility_t;

 // Visibility
  weapon_visibility_t m_visibility;
  weapon_visibility_t m_unit_visibility;

  // how many times can we use this weapon (since the beginning of the game) ?
  int m_initial_nb_ammo;
  int m_initial_nb_unit_per_ammo;
  bool use_unit_on_first_shoot;
  bool can_be_used_on_closed_map;

  // For sound
  int channel_load;

public:
  // Icone de l'arme dans l'interface
  Surface icone;

  // if max_strength != 0, display the strength bar
  double max_strength;

  // True if the weapon uses keys when activated.
  bool override_keys ;

  //Force weapons to use keys when true
  bool force_override_keys ;

  // Angle in degrees between -90 to 90
  int min_angle, max_angle;
  bool use_flipping;

protected:
  virtual void p_Select();
  virtual void p_Deselect();
  virtual void Refresh() = 0;
  virtual bool p_Shoot() = 0;

public:
  Weapon(Weapon_type type,
	 const std::string &id,
	 EmptyWeaponConfig * params,
	 weapon_visibility_t visibility = ALWAYS_VISIBLE);
  virtual ~Weapon();

  // Select or deselect the weapon
  void Select();
  void Deselect();

  // Gestion de l'arme
  void Manage();
  bool CanChangeWeapon() const ;

  // Draw the weapon
  virtual void Draw();
  virtual void DrawWeaponFire();
  void DrawWeaponBox();

  void DrawUnit(int unit);

  // Manage the numbers of ammunitions
  bool EnoughAmmo() const;
  void UseAmmo();
  bool EnoughAmmoUnit() const;
  void UseAmmoUnit();
  int ReadInitialNbAmmo() const;
  int ReadInitialNbUnit() const;

  bool CanBeUsedOnClosedMap() const;
  bool UseCrossHair() const { return min_angle != max_angle; };

  // Calculate weapon position
  virtual void PosXY (int &x, int &y) const;

  // Create a new action "shoot" in action handler
  void NewActionShoot() const;

  // Prepare the shoot : set the angle and strenght of the weapon
  // Begin the shooting animation of the character
  void PrepareShoot(double strength, int angle);

  // Shot with the weapon
  // Return true if we have been able to trigger the weapon
  bool Shoot();

  // L'arme est encore active (animation par ex.) ?
  bool IsActive() const;

  // the weapon is ready to use ? (is there bullets left ?)
  virtual bool IsReady() const ;

  // Begin to load, to choose the strength
  virtual void InitLoading() ;

  // Are we loading to choose the strength
  virtual bool IsLoading() const;

  // Stop loading
  virtual void StopLoading() ;

  // update strength (so the strength bar can be updated)
  virtual void UpdateStrength();

  const Point2i GetGunHolePosition();

  // Choose a target.
  virtual void ChooseTarget (Point2i mouse_pos);

  //Misc actions.
  virtual void ActionUp ();//called by mousse.cpp when mousewhellup
  virtual void ActionDown ();//called by mousse.cpp when mousewhelldown

  // Handle a keyboard event.
  virtual void HandleKeyEvent(int key, int event_type) ;

  // Get informed that the turn is over.
  virtual void SignalTurnEnd();

  // Stop using this weapon (only used with lowgrav and jetpack)
  virtual void ActionStopUse();

  // Load parameters from the xml config file
  // Return true if xml has been succesfully load
  bool LoadXml(xmlpp::Element * weapon);

  // return the strength of the weapon
  const double ReadStrength() const;

  // Acc� aux donn�s
  const std::string& GetName() const;
  const std::string& GetID() const;
  Weapon_type GetType() const;

  // Allows or not the character selection with mouse click (tab is allowed)
  // This is used in weapons like the automated bazooka, where it's required
  // a target. Default is true.
  bool mouse_character_selection;
};

//-----------------------------------------------------------------------------
#endif
