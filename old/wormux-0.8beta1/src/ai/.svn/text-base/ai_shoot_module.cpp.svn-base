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
 * Artificial intelligence Shoot module
 *****************************************************************************/

#include "ai_shoot_module.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "network/randomsync.h"
#include "team/macro.h"
#include "tool/error.h"
#include "tool/math_tools.h"
#include "tool/string_tools.h"

#include <iostream>


// =================================================
// Static method
// =================================================
// Try to find an enemy which is shootable by
// weapons like gun, shotgun, m16
// =================================================
const Character* AIShootModule::FindShootableEnemy(Character& shooter,
						   double& shoot_angle)
{
  FOR_ALL_LIVING_ENEMIES(shooter, team, character)
  {
    if ( IsDirectlyShootable(shooter, *character, shoot_angle) ) {
      return &(*character);
    }
  }
  return NULL;
}

// =================================================
// Static method
// =================================================
// Return true if there is a straight line with no
// collision between the shooter and a
// potential enemy
// Update the shoot_angle if it is ok
// =================================================
// This method is not perfect
// It tests from the Center of the shooter
// and not from the gun hole
// =================================================
bool AIShootModule::IsDirectlyShootable(const Character& shooter,
					const Character& enemy,
					double& shoot_angle)
{
  Point2i pos = shooter.GetCenter();
  Point2i arrival = enemy.GetCenter();
  Point2i departure = pos;
  Point2i delta_pos;

  double original_angle = pos.ComputeAngle(arrival);

  // compute to see if there any part of ground between the 2 characters
  // While test is not finished
  while (pos != arrival) {

    // is there a collision on the ground ??
    if ( !world.IsInVacuum(pos.x, pos.y) ) {
      return false;
    }

    // the point is outside the map
    if ( world.IsOutsideWorldX(pos.x) || world.IsOutsideWorldY(pos.y) ) {
      break;
    }

    // is there a collision with another character ?
    FOR_ALL_CHARACTERS(team, other_character) {
      if ( &(*other_character) != &shooter
	   && &(*other_character) != &enemy ) {

	if ( other_character->GetTestRect().Contains(pos) )
	  return false;

      }
    }

    // next step
    int diff_x = pos.x - arrival.x;
    int diff_y = pos.y - arrival.y;

    delta_pos.x = 0;
    delta_pos.y = 0;

    if (abs(diff_x) > abs(diff_y)) {
      if (pos.x < arrival.x)
	delta_pos.x = 1;   //Increment x
      else
	delta_pos.x = -1;
    } else {
      if (pos.y < arrival.y)
	delta_pos.y = 1;
      else
	delta_pos.y = -1;
    }

    pos += delta_pos;
  }

  // set the angle
  if (departure.x > arrival.x) {
    shoot_angle = InverseAngleRad(original_angle);
  } else {
    shoot_angle = original_angle;
  }

  return true;
}

bool AIShootModule::SelectFiringWeapon(double shoot_angle) const
{
  // we choose between gun, sniper_rifle, shotgun and submachine gun
  uint selected = uint(randomSync.GetDouble(0.0, 3.5));
  switch (selected) {
  case 0:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SHOTGUN);
    if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
  case 1:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SNIPE_RIFLE);
    if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
    //     case 2:
    //       ActiveTeam().SetWeapon(Weapon::WEAPON_SUBMACHINE_GUN);
    //       if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
  case 3:
  default:
    ActiveTeam().SetWeapon(Weapon::WEAPON_GUN);
  }

  // Check the angle
  double angle = BorneDouble(m_angle, - (ActiveTeam().GetWeapon().GetMaxAngle()),
			     - (ActiveTeam().GetWeapon().GetMinAngle()) );

  if (AbsReel(angle-m_angle) > 0.08726/* 5 degree */) {
    // angle is too wide for the weapon
    return false;
  }

  // Check number of ammo
  if ( ! ActiveTeam().GetWeapon().EnoughAmmo() ) {
    return false;
  }

  return true;
}

// =================================================
// Try to find an enemy which is shootable by
// weapons like dynamite, mine, ...
// =================================================
const Character* AIShootModule::FindProximityEnemy(const Character& shooter) const
{
  FOR_ALL_LIVING_ENEMIES(shooter, team, character) {
    if ( m_AIMovementModule.SeemsToBeReachable(shooter, *character) ) {
      return &(*character);
    }
  }
  return NULL;
  //     if (m_nearest_enemy == NULL
  // 	|| ( character->GetCenter().Distance( ActiveCharacter().GetCenter()) <
  // 	     m_nearest_enemy->GetCenter().Distance( ActiveCharacter().GetCenter()))
  // 	)
  //       m_nearest_enemy = &(*character);
  //   }
  //   assert(m_nearest_enemy != NULL);
}

bool AIShootModule::SelectProximityWeapon(const Character& enemy) const
{
  // selecting between dynamite, mine taking care of enemy life points
  uint life_points = enemy.GetEnergy();

//  TODO : baseball:
//   if (IsNearWater(ActivePlayer(), enemy)) {
//     ActiveTeam().SetWeapon(Weapon::BASEBALL);
//     if (ActiveTeam().GetWeapon().EnoughAmmo())
//       return true;
//   }

  // Dynamite
  if (life_points > 50) {
    ActiveTeam().SetWeapon(Weapon::WEAPON_DYNAMITE);
    if (ActiveTeam().GetWeapon().EnoughAmmo())
      return true;
  }

  // Mine
  ActiveTeam().SetWeapon(Weapon::WEAPON_MINE);
  if (ActiveTeam().GetWeapon().EnoughAmmo())
    return true;


  // No proximity weapons found !
  return false;
}

// =================================================
// Shoot!
// =================================================
void AIShootModule::Shoot()
{
  if (m_current_time > m_last_shoot_time + 2 ||
      m_last_shoot_time == 0) {
    ActiveTeam().GetWeapon().NewActionWeaponShoot();
    m_last_shoot_time = m_current_time;
  }

  if (!(ActiveTeam().GetWeapon().EnoughAmmoUnit())) {
    m_has_finished = true;
    ActiveCharacter().body->StartWalk();
  }
}

const Character* AIShootModule::FindEnemy()
{
  if (m_has_finished) {
    return NULL;
  }

  if (m_enemy != NULL && !(m_enemy->IsDead())) {
    return m_enemy;
  }

  m_current_strategy = NO_STRATEGY;

  // Proximity enemy ?
  m_enemy = FindProximityEnemy(ActiveCharacter());
  if ( m_enemy != NULL ) {

    m_current_strategy = NEAR_FROM_ENEMY;

    GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" has decided to injured "
				     + m_enemy->GetName());

    if ( SelectProximityWeapon(*m_enemy) ) {
      m_angle = 0;
      ActiveCharacter().SetFiringAngle(m_angle);
      return m_enemy;
    }
  }

  // "Gun-able" enemy ?
  m_enemy = FindShootableEnemy(ActiveCharacter(), m_angle);
  if ( m_enemy != NULL ) {

    m_current_strategy = SHOOT_FROM_POINT;

    GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" will shoot "
				     + m_enemy->GetName());

    // we choose between gun, sniper_rifle, shotgun and submachine gun
    if ( SelectFiringWeapon(m_angle) ) {
      ActiveCharacter().SetFiringAngle(m_angle);
      return m_enemy;
    }
  }

  m_current_strategy = NO_STRATEGY;
  m_angle = 0;
  m_enemy = NULL;

  return m_enemy;
}

void AIShootModule::ChooseDirection()
{
  if ( m_enemy ) {

    if ( abs(ActiveCharacter().GetCenterX() - m_enemy->GetCenterX()) <= 5 )
      return;

    if ( ActiveCharacter().GetCenterX() < m_enemy->GetCenterX())
      ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
    else if ( ActiveCharacter().GetCenterX() > m_enemy->GetCenterX())
      ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
    // else ActiveCharacter().GetCenterX() == m_enemy->GetCenterX()
  }
}

bool AIShootModule::Refresh(uint current_time)
{
  if (m_has_finished) {
    return true;
  }

  m_current_time = current_time;

  FindEnemy();
  ChooseDirection();

  switch (m_current_strategy) {

  case NO_STRATEGY:
    //ActiveTeam().SetWeapon(Weapon::WEAPON_SKIP_TURN);
    //Shoot();
    //break;

  case NEAR_FROM_ENEMY:
    // We are near enough of an enemy (perhaps not the first one we have choosen)
    FOR_ALL_LIVING_ENEMIES(ActiveCharacter(), team, character) {
      if ( abs((*character).GetX() - ActiveCharacter().GetX()) <= 10 &&
	   abs ((*character).GetY() - ActiveCharacter().GetY()) < 60 ) {
      //if ( (*character).GetCenter().Distance( ActiveCharacter().GetCenter()) < 50) {
	if (&(*character) != m_enemy) {
	  GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" changes target : "
					   + (*character).GetName());
	}
 	m_enemy = &(*character);
 	Shoot();
      }
    }
    break;

  case SHOOT_FROM_POINT:
    Shoot();
    return false;
    break;
  }

  return true;
}

// =================================================
// Initialize Shoot module when changing
// character to control
// =================================================
void AIShootModule::BeginTurn()
{
  m_enemy = NULL;
  m_last_shoot_time = 0;
  m_angle = 0;
  m_current_strategy = NO_STRATEGY;
  m_has_finished = false;

  // Choose random direction for the moment
  ActiveCharacter().SetDirection( randomSync.GetBool()?Body::DIRECTION_LEFT:Body::DIRECTION_RIGHT );
}

AIShootModule::AIShootModule(const AIMovementModule& to_remove) :
  m_current_strategy(NO_STRATEGY),
  m_current_time(0),
  m_enemy(NULL),
  m_has_finished(false),
  m_AIMovementModule(to_remove),
  m_angle(0),
  m_last_shoot_time(0)
{
  std::cout << "o Artificial Intelligence Shoot module initialization" << std::endl;
}

void AIShootModule::SetNoStrategy()
{
  m_current_strategy = NO_STRATEGY;
}
