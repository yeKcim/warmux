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
 * Character of a team.
 *****************************************************************************/

#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <string>
#include <SDL.h>
#include "gui/EnergyBar.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "include/base.h"
#include "object/physical_obj.h"
#include "particles/particle.h"
#include "body.h"
#include "damage_stats.h"

class Team;
class ParticleEngine;

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
  DamageStatistics damage_stats;
  EnergyBar energy_bar;

  // survived games
  int survivals;

  // name
  Text* name_text;

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
  ParticleEngine bubble_engine;
public:

  // Previous strength
  double previous_strength;

  Body* body;

private:
  void DrawEnergyBar(int dy);
  void DrawName(int dy) const;

  void SignalDrowning();
  void SignalGhostState(bool was_dead);
  void SignalCollision();
  void SetBody(Body* char_body);

  void AddFiringAngle(double angle);

public:

  Character (Team& my_team, const std::string &name, Body *char_body);
  Character (const Character& acharacter);
  ~Character();

  void SignalExplosion();

  // Energy related
  void SetEnergyDelta (int delta, bool do_report=true);
  void SetEnergy(int new_energy);
  inline const int & GetEnergy() const { return life_points;}

  bool GotInjured() const;
  void Die();
  void DisableDeathExplosion();
  bool IsActiveCharacter() const;
  // Disease handling
  bool IsDiseased() const;
  void SetDiseaseDamage(const uint damage_per_turn, const uint disease_duration);
  uint GetDiseaseDamage() const;
  uint GetDiseaseDuration() const;
  void DecDiseaseDuration();

  // to be used by action handler
  alive_t GetLifeState() const;
  void SetLifeState(alive_t state);

  void Draw();
  void Refresh();

  void PrepareTurn();
  void StartPlaying();
  void StopPlaying();

  void PrepareShoot();
  bool IsPreparingShoot();
  void DoShoot();
  double GetFiringAngle() const;
  double GetAbsFiringAngle() const;
  void SetFiringAngle(double angle);

  // Show hide the Character
  void Hide();
  void Show();

  // ---- Movement  -----
  // Can we move (check a timeout)
  bool CanMoveRL() const;
  bool CanJump() const;

  // Jumps
  void Jump(double strength, double angle);
  void Jump();
  void HighJump();
  void BackJump();

  // Initialise left or right movement
  void BeginMovementRL (uint pause);
  bool CanStillMoveRL (uint pause);

  // Direction of the character ( -1 == looks to the left / +1 == looks to the right)
  void SetDirection(Body::Direction_t direction);
  Body::Direction_t GetDirection() const;

  // Team owner
  const Team& GetTeam() const;
  uint GetTeamIndex();
  uint GetCharacterIndex();

  // Access to character info
  const std::string& GetName() const { return character_name; }
  bool IsSameAs(const Character& other) const { return (GetName() == other.GetName()); }

  // Hand position
  const Point2i & GetHandPosition() const;

  // Damage report
  const DamageStatistics& GetDamageStats() const;
  void ResetDamageStats();

  // Body handling
  Body * GetBody() const;
  void SetWeaponClothe();
  void SetClothe(std::string name);
  void SetMovement(std::string name);
  void SetClotheOnce(std::string name);
  void SetMovementOnce(std::string name);

  // Keyboard handling
  void HandleKeyPressed_MoveRight();
  void HandleKeyRefreshed_MoveRight();
  void HandleKeyReleased_MoveRight();

  void HandleKeyPressed_MoveLeft();
  void HandleKeyRefreshed_MoveLeft();
  void HandleKeyReleased_MoveLeft();

  void HandleKeyPressed_Up();
  void HandleKeyRefreshed_Up();
  void HandleKeyReleased_Up();

  void HandleKeyPressed_Down();
  void HandleKeyRefreshed_Down();
  void HandleKeyReleased_Down();

  void HandleKeyPressed_Jump();
  void HandleKeyRefreshed_Jump();
  void HandleKeyReleased_Jump();

  void HandleKeyPressed_HighJump();
  void HandleKeyRefreshed_HighJump();
  void HandleKeyReleased_HighJump();

  void HandleKeyPressed_BackJump();
  void HandleKeyRefreshed_BackJump();
  void HandleKeyReleased_BackJump();

};

#endif
