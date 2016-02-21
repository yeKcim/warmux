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
 * A worms team.
 *****************************************************************************/

#ifndef TEAM_H
#define TEAM_H

#include <list>
#include <map>
#include <string>
#include "character.h"
#include "team_energy.h"
#include "../graphic/surface.h"
#include "../include/enum.h"
#include "../include/base.h"
#include "../weapon/crosshair.h"
#include "../weapon/weapon.h"
#include "../tool/point.h"
#include "../tool/resource_manager.h"

class Character;
class Weapon;

class Team
{
public:
  typedef std::list<Character>::iterator iterator;
  typedef std::list<Character>::const_iterator const_iterator;

  std::map<std::string, int> m_nb_ammos;
  std::map<std::string, int> m_nb_units;

  // Autres
  CrossHair crosshair;
  Surface ecusson;
  Point2i sauve_camera;
  bool camera_est_sauve;
  TeamEnergy energie;

 
private:
  std::string m_id;
  std::string m_name;
  std::string m_sound_profile;
  std::list<Character> vers;
  int ver_actif, vers_fin;
  iterator vers_fin_it;
  Weapon *active_weapon;
   bool ChargeDonnee( xmlpp::Element *xml, Profile *res_profile);
public:
  // Initialization
  Team ();
  bool Init (const std::string &teams_dir, const std::string &id);
  void Reset();

  // ******* TODO: KILL THIS FUNCTIONS !!! ********
  Character& operator[] (uint index);
  const Character& operator[] (uint index) const;
  // ******* TODO: KILL THIS FUNCTIONS !!! ********
  bool IsSameAs(const Team& other);

  // Switch to next worm.
//  void NextCharacter();
  int NextCharacterIndex();

  // Select an *ALIVE* character
  void SelectCharacterIndex (uint index);

  // Prepare turn.
  void PrepareTurn();

  // Access to the worms.
  int ActiveCharacterIndex() const;
  Character& ActiveCharacter();

  void Draw();
  void Refresh();

  // Change the weapon.
  void SetWeapon (Weapon_type nv_arme);
  int NbAliveCharacter() const;

  // Access to the active weapon.
  Weapon& AccessWeapon() const;
  const Weapon& GetWeapon() const;
  Weapon_type GetWeaponType() const;

  // Init the energy bar of the team.
  void InitEnergy (uint max);

  // Update the energy bar values of the team.
  void ActualiseBarreEnergie();

  // Read the total energy of the team.
  uint LitEnergie();

  // Access to data.
  const std::string& GetName() const { return m_name; }
  const std::string& GetId() const { return m_id; }  
  const std::string& GetSoundProfile() const { return m_sound_profile; }
  iterator begin();
  iterator end();

  // Number of ammo for the current selected weapon.
  // (return INFINITE_AMMO is ammo are unlimited !)
  int ReadNbAmmos() const;
  int ReadNbAmmos(const std::string &weapon_name) const;
  int& AccessNbAmmos();

  // Number of current unit per ammo for the selected weapon.
  int ReadNbUnits() const;
  int ReadNbUnits(const std::string &weapon_name) const;
  int& AccessNbUnits();
  void ResetNbUnits();

  // Only for network:
  // true if the team belong to a local player
  // false if the team belong to a player on the network
  bool is_local;
protected:
  void internal_NextCharacter();
};

#endif // TEAM_H

