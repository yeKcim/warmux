/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * A team.
 *****************************************************************************/

#ifndef TEAM_H
#define TEAM_H

#include <list>
#include <vector>
#include <string>
#include "character/character.h"
#include "team/team_energy.h"
#include "graphic/surface.h"
#include "weapon/crosshair.h"
#include "weapon/weapon.h"

#define NO_AI_NAME       "none"
#define DEFAULT_AI_NAME  "default"
#define DUMB_AI_NAME     "dumb"
#define STRONG_AI_NAME   "strong"

class AIPlayer;
class Body;
class WeaponsList;
class XmlReader;
class Profile;

class Team
{
public:
  typedef std::list<Character>::iterator iterator;
  typedef std::list<Character>::const_iterator const_iterator;

private:
  // parameters that never change after first loading
  Surface mini_flag;
  Surface flag;
  Surface death_flag;
  Surface big_flag;
  const std::string m_id;
  const std::string m_name;
  std::string m_sound_profile;
  std::vector<std::string> default_characters_names;
  std::vector<std::string> bodies_ids;

  // parameters reseted by SetDefaultPlayingConfig
  std::string m_player_name;
  std::vector<std::string> custom_characters_names;
  uint nb_characters;
  bool remote;
  AIPlayer * ai;
  std::string ai_name;

  // parameters changed just before game or in game
  std::list<Character> characters;
  iterator active_character;
  Weapon *active_weapon;
  uint current_turn;
  bool abandoned;
  WeaponsList * weapons_list;

  // Colors
  Color team_color;
  uint  group;

  bool LoadCharacters();

  void AddOnePlayingCharacter(const std::string& character_name, Body *body);
  bool AddPlayingCharacters(const std::vector<std::string> characters);

protected:
  Team(XmlReader& doc, Profile* res,
       const std::string& name, const std::string &id);

public:
  // Do not call UnloadGamingData in here - these data are shared!
  ~Team();
  static Team* LoadTeam(const std::string &teams_dir, const std::string &id, std::string& error);

  std::vector<int> m_nb_ammos;
  std::vector<int> m_nb_units;
  std::vector<int> m_energy;

  // Others
  CrossHair        crosshair;
  TeamEnergy       energy;

  void LoadGamingData(WeaponsList * weapons);
  void UnloadGamingData();

  bool IsSameAs(const Team& other) const { return m_id == other.m_id; }

  // Manage number of characters
  void SetNbCharacters(uint howmany) { nb_characters = howmany; }
  uint GetNbCharacters() const { return nb_characters; }

  // Switch to next worm.
  void NextCharacter(bool new_turn = false);
  void PreviousCharacter();
  void SelectCharacter(Character * c);

  // Prepare turn.
  void PrepareTurn();

  // Access to the character.
  Character& ActiveCharacter() const { return (*active_character); }

  void DrawEnergy(const Point2i& pos) { energy.Draw(pos); }
  void Refresh() { energy.Refresh(); }
  void RefreshAI();

  // Change the weapon.
  void SetWeapon(Weapon::Weapon_type nv_arme);
  int NbAliveCharacter() const;

  // Access to the active weapon.
  Weapon& AccessWeapon() const { return *active_weapon; }
  const Weapon& GetWeapon() const { return *active_weapon; }
  Weapon::Weapon_type GetWeaponType() const { return GetWeapon().GetType(); }

  // Init the energy bar of the team.
  void InitEnergy(uint max) { energy.Config(ReadEnergy(), max); }

  // Update the energy bar values of the team.
  void UpdateEnergyBar() { energy.SetValue(ReadEnergy()); }
  TeamEnergy & GetEnergyBar() { return energy; };

  // Read the total energy of the team.
  uint ReadEnergy() const;

  // Access to data.
  const std::string& GetName() const { return m_name; }
  const std::string& GetId() const { return m_id; }
  const std::string& GetPlayerName() const { return m_player_name; }
  const std::string& GetSoundProfile() const { return m_sound_profile; }
  const Surface& GetFlag() const { return flag; }
  const Surface& GetMiniFlag() const { return mini_flag; }
  const Surface& GetDeathFlag() const { return death_flag; }
  const Surface& GetBigFlag() const { return big_flag; }
  iterator begin() { return characters.begin(); }
  iterator end() { return characters.end(); }
  Character* FindByIndex(uint index);

  void SetPlayerName(const std::string& player_name) { m_player_name = player_name; };

  // Number of ammo for the current selected weapon.
  // (return INFINITE_AMMO is ammo are unlimited !)
  int ReadNbAmmos() const { return ReadNbAmmos(active_weapon->GetType()); }
  int ReadNbAmmos(Weapon::Weapon_type weapon_type) const
  {
    ASSERT((uint)weapon_type < m_nb_ammos.size());
    return m_nb_ammos[weapon_type];
  }
  // if value not initialized, it initialize to 0 and then return 0
  int& AccessNbAmmos() { return m_nb_ammos[active_weapon->GetType()]; }

  // Number of current unit per ammo for the selected weapon.
  int ReadNbUnits() const { return ReadNbUnits(active_weapon->GetType()); }
  int ReadNbUnits(Weapon::Weapon_type weapon_type) const
  {
    ASSERT((uint)weapon_type < m_nb_units.size());
    return m_nb_units[weapon_type];
  }
  // if value not initialized, it initialize to 0 and then return 0
  int& AccessNbUnits() { return m_nb_units[active_weapon->GetType()]; }
  void ResetNbUnits() { m_nb_units[active_weapon->GetType()] = active_weapon->ReadInitialNbUnit(); }

  bool IsAI() const { return ai_name != NO_AI_NAME; }
  bool IsHuman() const { return !IsAI(); }
  bool IsLocal() const { return !remote; }
  bool IsRemote() const { return remote; }
  bool IsLocalAI() const { return IsLocal() && IsAI(); }
  bool IsLocalHuman() const { return IsLocal() && IsHuman(); }

  bool IsActiveTeam() const;

  void SetRemote(bool value) { remote = value; }
  void SetAIName(const std::string value) { ai_name = value; }
  const std::string& GetAIName() { return ai_name; }
  void LoadAI();

  // reset characters number, type_of_player and player name
  void SetDefaultPlayingConfig();

  void SetCustomCharactersNames(const std::vector<std::string>& custom_names) { custom_characters_names = custom_names; }
  void ClearCustomCharactersNames() { custom_characters_names.clear(); }

  void SetCustomCharactersNamesFromAction(Action *a);
  void PushCustomCharactersNamesIntoAction(Action *a) const;

  void Abandon() { abandoned = true; }
  bool IsAbandoned() { return abandoned; }

  // Color management
  const Color& GetColor() const { return team_color; };

  // Group management
  uint GetGroup() const { return group; }
  void SetGroup(uint g) { group = g; }
};

#endif /* TEAM_H */

