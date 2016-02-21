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
 * Character of a team.
 *****************************************************************************/

#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <string>
#include "gui/energy_bar.h"
#include "include/base.h"
#include "object/physical_obj.h"
#include "character/body.h"

class Text;
class Team;
class ParticleEngine;
class DamageStatistics;
class Body;

#ifdef DEBUG
//#define DEBUG_SKIN
#endif

class Character : public PhysicalObj
{
private:
  /* If you need this, implement it (correctly) */
  Character operator=(const Character&);
  /**********************************************/

  std::string character_name;
  Team &m_team;
  bool step_sound_played;
  bool prepare_shoot;
  bool back_jumping;
  bool death_explosion;
  double firing_angle;

  uint disease_damage_per_turn;
  uint disease_duration;
  DamageStatistics *damage_stats;
  EnergyBar energy_bar;

  // survived games
  int survivals;

  // name
  Text* name_text;
#ifdef DEBUG_SKIN
  Text* skin_text;
#endif

  // chrono
  uint pause_bouge_dg;  // pause pour mouvement droite/gauche
  uint do_nothing_time;
  uint walking_time;
  uint animation_time;
  int lost_energy;
  bool hidden; //The character is hidden (needed by teleportation)

  // Channel used for sound
  int channel_step;

  // Generates green bubbles when the character is ill
  ParticleEngine *particle_engine;

  // this is needed because of network to know
  // if we have changed of active character
  bool is_playing;
public:

  // Previous strength
  double previous_strength;

  Body* body;

private:
  void DrawEnergyBar(int dy) const;
  void DrawName(int dy) const;

  void SignalDrowning();
  void SignalGhostState(bool was_dead);
  void SignalCollision(const Point2d& speed_vector);
  void SetBody(Body* char_body);

  void AddFiringAngle(double angle) { SetFiringAngle(firing_angle + angle); };

public:

  Character (Team& my_team, const std::string &name, Body *char_body);
  Character (const Character& acharacter);
  ~Character();

  void SignalExplosion();

  // Energy related
  void SetEnergyDelta(int delta, bool do_report = true);
  void SetEnergy(int new_energy);
  inline const int & GetEnergy() const { return m_energy; };

  bool GotInjured() const { return lost_energy < 0; };
  void Die();
  void DisableDeathExplosion() { death_explosion = false; };
  bool IsActiveCharacter() const;
  // Disease handling
  bool IsDiseased() const { return disease_duration > 0 && !IsDead(); };
  void SetDiseaseDamage(const uint damage_per_turn, const uint duration)
  {
    disease_damage_per_turn = damage_per_turn;
    disease_duration = duration;
  }
// Keep almost 1 in energy
  uint GetDiseaseDamage() const
  {
    if (disease_damage_per_turn < static_cast<uint>(GetEnergy()))
      return disease_damage_per_turn;
    return GetEnergy() - 1;
  }
  uint GetDiseaseDuration() const { return disease_duration; };
  void DecDiseaseDuration()
  {
    if (disease_duration > 0) disease_duration--;
    else disease_damage_per_turn = 0;
  }

  // Used to sync value across network
  virtual void GetValueFromAction(Action *);
  virtual void StoreValue(Action *);

  void Draw();
  void Refresh();

  void PrepareTurn();
  void StartPlaying();
  void StopPlaying();

  void PrepareShoot();
  bool IsPreparingShoot() const { return prepare_shoot; };
  void DoShoot();
  double GetFiringAngle() const;
  double GetAbsFiringAngle() const { return firing_angle; };
  void SetFiringAngle(double angle);

  // Show hide the Character
  void Hide() { hidden = true; };
  void Show() { hidden = false; };

  // ---- Movement  -----
  // Can we move (check a timeout)
  bool CanMoveRL() const;
  bool CanJump() const { return CanMoveRL(); };

  // Jumps
  void Jump(double strength, double angle);
  void Jump();
  void HighJump();
  void BackJump();

  // Initialise left or right movement
  void BeginMovementRL (uint pause, bool slowly = false);
  bool CanStillMoveRL (uint pause);

  // Direction of the character ( -1 == looks to the left / +1 == looks to the right)
  void SetDirection(BodyDirection_t direction);
  BodyDirection_t GetDirection() const;

  // Team owner
  const Team& GetTeam() const { return m_team; };
  Team& AccessTeam() const { return m_team; };
  uint GetTeamIndex() const;
  uint GetCharacterIndex() const;

  // Access to character info
  const std::string& GetName() const { return character_name; }
  bool IsSameAs(const Character& other) const { return (GetName() == other.GetName()); }

  // Hand position
  const Point2i & GetHandPosition() const;

  // Damage report
  const DamageStatistics* GetDamageStats() const { return damage_stats; };
  void ResetDamageStats();

  // Body handling
  Body * GetBody() { return body; };
  void SetWeaponClothe();

  // "force" option forces to apply the clothe/movement even if character is dead
  void SetClothe(const std::string& name, bool force=false);
  void SetMovement(const std::string& name, bool force=false);
  void SetClotheOnce(const std::string& name, bool force=false);
  void SetMovementOnce(const std::string& name, bool force=false);

  // Keyboard handling
  void HandleKeyPressed_MoveRight(bool shift);
  void HandleKeyRefreshed_MoveRight(bool shift) const;
  void HandleKeyReleased_MoveRight(bool shift);

  void HandleKeyPressed_MoveLeft(bool shift);
  void HandleKeyRefreshed_MoveLeft(bool shift) const;
  void HandleKeyReleased_MoveLeft(bool shift);

  void HandleKeyPressed_Up(bool shift) { HandleKeyRefreshed_Up(shift); };
  void HandleKeyRefreshed_Up(bool shift);
  void HandleKeyReleased_Up(bool) const {};

  void HandleKeyPressed_Down(bool shift) { HandleKeyRefreshed_Down(shift); };
  void HandleKeyRefreshed_Down(bool shift);
  void HandleKeyReleased_Down(bool) const {};

  void HandleKeyPressed_Jump(bool shift);
  void HandleKeyRefreshed_Jump(bool) const {};
  void HandleKeyReleased_Jump(bool) const {};

  void HandleKeyPressed_HighJump(bool shift);
  void HandleKeyRefreshed_HighJump(bool) const { };
  void HandleKeyReleased_HighJump(bool) const { };

  void HandleKeyPressed_BackJump(bool shift);
  void HandleKeyRefreshed_BackJump(bool) const {};
  void HandleKeyReleased_BackJump(bool) const {};

};

#endif
