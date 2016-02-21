/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Classes virtuelles permettant de définir une arme et un projectile. Les
 * armes ont un nom, une image, un état actif/inactif et une icône (affichée
 * dans l'interface). Les projectiles sont des objets physiques qui ont un
 * comportement spécial lorsqu'ils entrent en collision ou qu'ils sortent du
 * terrain.
 *****************************************************************************/

#ifndef WEAPON_H
#define WEAPON_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../team/character.h"
#include "../graphic/sprite.h"
#include "../include/base.h"
#include "../include/enum.h"
#include "../object/particle.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
#include "../gui/progress_bar.h"
#include "weapon_cfg.h"
#include <string>
//-----------------------------------------------------------------------------
class Character;

// Constante munitions illimitées
extern const int INFINITE_AMMO;

extern const uint BUTTON_ICO_WIDTH;
extern const uint BUTTON_ICO_HEIGHT;

extern const uint WEAPON_ICO_WIDTH;
extern const uint WEAPON_ICO_HEIGHT;


enum weapon_visibility {
  ALWAYS_VISIBLE,
  NEVER_VISIBLE,
  VISIBLE_ONLY_WHEN_ACTIVE,
  VISIBLE_ONLY_WHEN_INACTIVE
};

class WeaponStrengthBar : public BarreProg
{
 public:
  bool visible ;
} ;


//-----------------------------------------------------------------------------

// Projectil d'une arme
class WeaponProjectile : public PhysicalObj
{
public:
  bool is_active;
  Sprite *image;
protected:
  Character* dernier_ver_touche;
  PhysicalObj* dernier_obj_touche;

  // Peut toucher les vers et les objets ? (test de collision)
  bool touche_ver_objet;

public:
  WeaponProjectile(const std::string &nom);
  virtual void Draw();
  virtual void Refresh();
  virtual void Reset() ;
  virtual void Init()=0 ;
  void PrepareTir();
  virtual bool CollisionTest (int dx, int dy);
  Character* LitDernierVerTouche() const { return dernier_ver_touche; }
  PhysicalObj* LitDernierObjTouche() const { return dernier_obj_touche; }

  // Il y a eu impact avec un ver, un objet ou le sol ?
  bool TestImpact ();

protected:
  virtual void SignalGhostState (bool was_dead);
  virtual void SignalFallEnding();
  virtual void SignalCollision() = 0;
};

//-----------------------------------------------------------------------------

class Weapon 
{
protected:
  typedef enum {
    weapon_origin_HAND,
    weapon_origin_OVER
  } weapon_origin_t;
  Weapon_type m_type;
  std::string m_id;
  std::string m_name;
  bool m_is_active;
  Sprite *m_image;

  struct s_position{
    int dx, dy;
    weapon_origin_t origin;
  } position;

  // Actual strength of the weapon
  double m_strength;

  // time of beginning to load (for choosing the strength)
  uint m_first_time_loading;

  // change weapon after ? (for the ninja cord = true)
  bool m_can_change_weapon;

  // Extra parameters
  EmptyWeaponConfig *extra_params;

  // Visibility
  uint m_visibility;
  uint m_unit_visibility;
  // how many times can we use this weapon (since the beginning of the game) ?
  int m_initial_nb_ammo;
  int m_initial_nb_unit_per_ammo;
  bool use_unit_on_first_shoot;

  // For sound
  int channel_load;

public:
  // Icone de l'arme dans l'interface
  SDL_Surface *icone;

  // if max_strength != 0, display the strength bar
  double max_strength;

  // True if the weapon uses keys when activated.
  bool override_keys ;

  // Angle in degrees between -90 to 90
  int min_angle, max_angle;
  bool use_flipping;

protected:
  virtual void p_Init();
  virtual void p_Select();
  virtual void p_Deselect();
  virtual void Refresh() = 0;
  virtual bool p_Shoot() = 0;

public:
  Weapon(Weapon_type type, const std::string &id);
  virtual ~Weapon() {}

  void Init();

  // Select or deselect the weapon
  void Select(); 
  void Deselect(); 

  // Gestion de l'arme
  void Manage();
  bool CanChangeWeapon() const ;

  // Draw the weapon
  virtual void Draw();
  void DrawWeaponBox();

  void DrawUnit(int unit);

  // Manage the numbers of ammunitions
  bool EnoughAmmo() const;
  void UseAmmo();
  bool EnoughAmmoUnit() const;
  void UseAmmoUnit();
  int ReadInitialNbAmmo() const;
  int ReadInitialNbUnit() const;

  // Calculate weapon position
  virtual void PosXY (int &x, int &y) const;
  
  // Create a new action "shoot" in action handler
  void NewActionShoot() const;

  // Tire avec l'arme
  // Renvoie true si l'arme a pu être enclanchée
  bool Shoot(double strength, int angle);

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

  // Get the rotation point of the weapon
  // This is used for drawing the crosshair
  virtual void RotationPointXY (int &x, int &y) const;

  // Choose a target.
  // Return false if it not fire directly after
  virtual void ChooseTarget ();

  // Handle a keyboard event.
  virtual void HandleKeyEvent(int key, int event_type) ;

  // Get informed that the turn is over.
  virtual void SignalTurnEnd();

  // Load parameters from the xml config file
  // Return true if xml has been succesfully load
  bool LoadXml(xmlpp::Element * weapon);  

  // return the strength of the weapon
  const double ReadStrength() const;

  // Accès aux données
  const std::string& GetName() const;
  const std::string& GetID() const;
  Weapon_type GetType() const;
};

//-----------------------------------------------------------------------------
#endif
