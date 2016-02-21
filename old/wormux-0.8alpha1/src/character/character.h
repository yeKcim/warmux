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
 * Character of a team.
 *****************************************************************************/

#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <string>
#include <SDL.h>
//#include "body.h"
//#include "../team/team.h"
#include "../gui/progress_bar.h"
#include "../graphic/sprite.h"
#include "../graphic/text.h"
#include "../include/base.h"
#include "../object/physical_obj.h"
#include "../particles/particle.h"

class Body;
class Team;
class ParticleEngine;

class Character : public PhysicalObj
{
private:
  std::string character_name;
  Team &m_team;
  bool step_sound_played;
  bool prepare_shoot;
  bool back_jumping;

  // energy
  uint energy;
  uint disease_damage_per_turn;
  uint disease_duration;
  int  damage_other_team;
  int  damage_own_team;
  int  max_damage;
  int  current_total_damage;
  BarreProg energy_bar;
  int crosshair_angle;

  // survived games
  int survivals;

  // name
  Text* name_text;

  // chrono
  uint pause_bouge_dg;  // pause pour mouvement droite/gauche
  uint do_nothing_time;
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
  void DrawEnergyBar (int dy);
  void DrawName (int dy) const;

  void SignalDrowning();
  void SignalGhostState (bool was_dead);
  void SignalCollision();

public:

  Character (Team& my_team, const std::string &name);
  Character (const Character& acharacter);
  ~Character();

  void SignalExplosion();

  // Energy related
  void SetEnergyDelta (int delta, bool do_report=true);
  void SetEnergy(int new_energy);
  uint GetEnergy() const;
  bool GotInjured() const;
  void Die();
  bool IsActiveCharacter() const;
  // Disease handling
  bool IsDiseased() const;
  void SetDiseaseDamage(const uint damage_per_turn, const uint disease_duration);
  uint GetDiseaseDamage() const;
  void DecDiseaseDuration();

  void Draw();
  void Refresh();

  void PrepareTurn ();
  void StartPlaying();
  void StopPlaying();

// Handle a key event on the character.
  void HandleKeyEvent(int key, int event_type) ;
  void PrepareShoot();
  void DoShoot();
  void HandleShoot(int event_type) ;

  // Show hide the Character
  void Hide();
  void Show();

  // ---- Movement  -----
  // Can we move (check a timeout)
  bool MouvementDG_Autorise() const;
  bool CanJump() const;

  // Jumps
  void Jump(double strength, int angle);
  void Jump();
  void HighJump();
  void BackJump();

  // Initialise left or right movement
  void InitMouvementDG (uint pause);
  bool CanStillMoveDG (uint pause);

  // Direction of the character ( -1 == looks to the left / +1 == looks to the right)
  void SetDirection (int direction);
  int GetDirection() const;

  // Team owner
  const Team& GetTeam() const;
  uint GetTeamIndex();
  uint GetCharacterIndex();

  // Access to character info
  const std::string& GetName() const { return character_name; }
  bool IsSameAs(const Character& other) { return (GetName() == other.GetName()); }

  // Hand position
  const Point2i & GetHandPosition() const;
  void GetHandPositionf (double &x, double &y);

  // Damage report
  void HandleMostDamage();
  void MadeDamage(const int Dmg, const Character &other);
  int  GetMostDamage() const { return max_damage; }
  int  GetOwnDamage() const { return damage_own_team; }
  int  GetOtherDamage() const { return damage_other_team; }

  // Body handling
  void SetBody(Body* _body);
  void SetWeaponClothe();
  void SetClothe(std::string name);
  void SetMovement(std::string name);
  void SetClotheOnce(std::string name);
  void SetMovementOnce(std::string name);
};

#endif
