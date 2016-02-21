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
 * A team.
 *****************************************************************************/

#ifndef TEAM_H
#define TEAM_H

#include <list>
#include <vector>
#include <string>
#include "ai/ai_player.h"
#include "team/team_energy.h"
#include "graphic/surface.h"
#include "weapon/crosshair.h"
#include "weapon/weapon.h"

const std::string NO_AI_NAME = "none";
const std::string DEFAULT_AI_NAME = "default";

class Character;
class CustomTeam;
class WeaponsList;

class Team
{
  /* If you need this, implement it (correctly) */
  Team(const Team&);
  Team operator=(const Team&);
  /**********************************************/

  public:
    typedef std::list<Character>::iterator iterator;
    typedef std::list<Character>::const_iterator const_iterator;

    std::vector<int> m_nb_ammos;
    std::vector<int> m_nb_units;
    std::vector<int> m_energy;

    // Autres
    CrossHair        crosshair;
    TeamEnergy       energy;


  private:
    Surface mini_flag;
    Surface flag;
    Surface death_flag;
    Surface big_flag;
    std::string m_teams_dir; // parent directory hosting the data
    std::string m_id;
    std::string m_name;
    std::string m_player_name;
    std::string m_sound_profile;
    std::list<Character> characters;
    iterator active_character;
    Weapon *active_weapon;
    uint nb_characters;
    uint current_turn;
    AIPlayer * ai;
    std::string ai_name;
    bool remote;
    bool abandoned;
    CustomTeam *attached_custom_team;

    Team (const std::string& _teams_dir,
          const std::string& _id,
          const std::string& _name,
          const Surface &_flag,
          const std::string& _sound_profile);

    bool LoadCharacters();
    WeaponsList * weapons_list;
  public:
    Team (const std::string &teams_dir, const std::string &id);

    void LoadGamingData(WeaponsList * weapons);
    void UnloadGamingData();

    bool IsSameAs(const Team& other) const;

  // Manage number of characters
    void SetNbCharacters(uint howmany);
    uint GetNbCharacters() const { return nb_characters; }

  // Switch to next worm.
    void NextCharacter(bool new_turn = false);
    void PreviousCharacter();
    void SelectCharacter(const Character * c);

  // Prepare turn.
    void PrepareTurn();

  // Access to the character.
    Character& ActiveCharacter() const;

    void DrawEnergy(const Point2i& pos);
    void Refresh();
    void RefreshAI();

  // Change the weapon.
    void SetWeapon (Weapon::Weapon_type nv_arme);
    int NbAliveCharacter() const;

  // Access to the active weapon.
    Weapon& AccessWeapon() const;
    const Weapon& GetWeapon() const;
    Weapon::Weapon_type GetWeaponType() const;

  // Init the energy bar of the team.
    void InitEnergy (uint max);

  // Update the energy bar values of the team.
    void UpdateEnergyBar();
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
    iterator begin();
    iterator end();
    Character* FindByIndex(uint index);

    void SetPlayerName(const std::string& player_name) { m_player_name = player_name; };

  // Number of ammo for the current selected weapon.
  // (return INFINITE_AMMO is ammo are unlimited !)
    int ReadNbAmmos() const;
    int ReadNbAmmos(const Weapon::Weapon_type &weapon_type) const;
    int& AccessNbAmmos();


  // Number of current unit per ammo for the selected weapon.
    int ReadNbUnits() const;
    int ReadNbUnits(const Weapon::Weapon_type &weapon_type) const;
    int& AccessNbUnits();
    void ResetNbUnits();

    bool IsAI() const { return ai_name != NO_AI_NAME; }
    bool IsHuman() const { return !IsAI(); }
    bool IsLocal() const { return !remote; }
    bool IsRemote() const { return remote; }
    bool IsLocalAI() const { return IsLocal() && IsAI(); }
    bool IsLocalHuman() const { return IsLocal() && IsHuman(); }

    bool IsActiveTeam() const;

    void SetRemote(bool value) { remote = value; }
    void SetAIName(const std::string value) { ai_name = value; }
    const std::string GetAIName() { return ai_name; }
    void LoadAI();

  // reset characters number, type_of_player and player name
    void SetDefaultPlayingConfig();

  // Custom team
    void AttachCustomTeam(CustomTeam*);
    void Abandon() { abandoned = true; }
    bool IsAbandoned() { return abandoned; }
};

#endif /* TEAM_H */

