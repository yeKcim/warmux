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
 * Artificial intelligence Shoot module
 *****************************************************************************/

#include "ai/ai_shoot_module.h"
#include "ai/ai_movement_module.h"
#include "character/character.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "map/wind.h"
#include "network/randomsync.h"
#include "team/team.h"
#include "team/macro.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "include/app.h"
#include "graphic/video.h"
#include <iostream>


// =================================================
// Static method
// =================================================
// Try to find an enemy which is shootable by
// weapons like gun, shotgun, m16
// =================================================
const Character* AIShootModule::FindShootableEnemy(const Character& shooter,
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
// Returns true if not dangerous as the shooter pulls enemy
// else returns false.
// For example if the shooter with a given angle is in front of wall
// he doesn't need to shoot the enemy (otherwises he shoots itself).
// =================================================
//
//
bool AIShootModule::IsBazookable(const Character& shooter,
                                 double& angle)
{
  Point2i tmp = ActiveTeam().GetWeapon().GetGunHolePosition();
  // Set the rotation of "angle" radians
  Point2i pos = Point2i::FromPolarCoordinates(sqrt(double(tmp.x * tmp.x + tmp.y * tmp.y)), double(tmp.ComputeAngle() + angle));

  Point2i delta = Point2i(1, 0);
  Point2i shoot_pos = shooter.GetCenter();

  // Affine equation : y = ax + b.
  double a = tan(angle);
  double b = pos.y - (a * pos.x);
  int distance = 0;

  while (distance < 30)
    {
      if ( world.IsOutsideWorldX(pos.x) || world.IsOutsideWorldY(pos.y) ) {
        break;
      }
      if (!world.IsInVacuum(pos.x, pos.y))
        return false;
      pos += delta;
      pos.y = int(a * pos.x + b);
      distance = shoot_pos.Distance(pos);
    }
  return true;
}

// =================================================
// Static method
// =================================================
// Return true if there is a straight line with no
// collision between the shooter and a
// potential enemy
// Update the shoot_angle if it is ok
// =================================================
bool AIShootModule::IsDirectlyShootable(const Character& shooter,
                                        const Character& enemy,
                                        double& shoot_angle)
{
  // We need to use center point, because gunholePosition is location
  // of last weapon of the ActiveTeam() and not the future gunholePos
  // which will be select.

  // TODO: Please find an alternative to solve this tempory solution
  Point2d pos = ActiveCharacter().GetCenter();
  Point2d arrival = enemy.GetCenter();

  double original_angle = pos.ComputeAngle(arrival);

  int delta_x = (pos.x > arrival.x) ? -1 : 1;
  double a = tan(original_angle);
  double b = pos.y - (a * pos.x);


  // compute to see if there any part of ground between the 2 characters
  // While test is not finished
  while (pos != arrival) {
    // XXX is rounding fine?
    Point2i posi(int(pos.x+0.5), int(pos.y+0.5));

    // the point is outside the map
    if ( world.IsOutsideWorld(posi) )
      {
        return false;
      }

    // is there a collision on the ground ??
    if (!world.IsInVacuum(posi)) {
      return false;
    }

    // is there a collision with another character ?
    FOR_ALL_CHARACTERS(team, other_character) {
      if ( &(*other_character) != &shooter
           && &(*other_character) != &enemy ) {

        // Skip only if this character has the same team of shooter
        // otherwises he's a enemy. (more reachable than the current)
        if (other_character->GetTestRect().Contains(pos)
            && (&other_character->GetTeam()) == (&shooter.GetTeam()))
          return false;
      }
    }
      pos.x += delta_x;
      pos.y = a * pos.x + b;
  }
    /*
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
    */
  shoot_angle = original_angle;

  return true;
}

bool AIShootModule::SelectFiringWeapon(double /*shoot_angle*/) const
{
  // we choose between gun, sniper_rifle, shotgun and submachine gun
  uint selected = uint(RandomSync().GetDouble(0.0, 3.5));
  switch (selected) {
  case 0:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SHOTGUN);
    if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
  case 1:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SNIPE_RIFLE);
    if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
  case 2:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SUBMACHINE_GUN);
    if (ActiveTeam().GetWeapon().EnoughAmmo()) break;
  case 3:
  default:
    ActiveTeam().SetWeapon(Weapon::WEAPON_GUN);
  }

  // Check the angle
  double angle = InRange_Double(m_angle, - (ActiveTeam().GetWeapon().GetMaxAngle()),
                             - (ActiveTeam().GetWeapon().GetMinAngle()) );

  if (AbsoluteValue(angle-m_angle) > 0.03490/* 2 degrees */) {
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
  //         || ( character->GetCenter().Distance( ActiveCharacter().GetCenter()) <
  //              m_nearest_enemy->GetCenter().Distance( ActiveCharacter().GetCenter()))
  //         )
  //       m_nearest_enemy = &(*character);
  //   }
  //   ASSERT(m_nearest_enemy != NULL);
}

bool AIShootModule::SelectProximityWeapon(const Character& enemy) const
{
  // selecting between dynamite, mine taking care of enemy life points
  int life_points = enemy.GetEnergy();

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

void AIShootModule::ShootWithBazooka()
{
  if (m_current_time > m_last_shoot_time + 2 ||
      m_last_shoot_time == 0) {
    ActiveTeam().SetWeapon(Weapon::WEAPON_BAZOOKA);
    double Xe = m_enemy->GetCenterX();
    double Ye = m_enemy->GetCenterY();
    double Xs = ActiveCharacter().GetCenterX();
    double Ys = ActiveCharacter().GetCenterY();
    std::cout << "Xe = " << Xe << std::endl;
    std::cout << "Ye = " << Ye << std::endl;
    std::cout << "Xs = " << Xs << std::endl;
    std::cout << "Ys = " << Ys << std::endl;
    double angle = atan(wind.GetStrength() * 75.0 /*wind factor */ /(30.0/* g */ *20 /* mass*/) );

    if (!IsBazookable(ActiveCharacter(), angle))
      {
        MSG_DEBUG("ai.shoot", "%s is not bazookable !\n", m_enemy->GetName().c_str());
        //MSG_DEBUG("ai.shoot", "%s is not bazookable !", m_enemy.GetName());
        ActiveTeam().SetWeapon(Weapon::WEAPON_SKIP_TURN);
        Shoot();
      }
    else
      {
        double Xpe = (Xe - Xs) * cos(angle) - (Ye - Ys) * sin(angle) + Xs;
        double Ype = (Xe - Xs) * sin(angle) + (Ye - Ys) * cos(angle) + Ys;
        Xe = Xpe;
        Ye = Ype;
        double V0x = (Xe - Xs ) / 80;
        double V0y = V0x * (Ye - (Ys))/ (Xe - Xs -V0x) - 1/2.0 * sqrt(30*30 /* g */+ wind.GetStrength() * 75.0 *wind.GetStrength() * 75.0  /20.0 /20.0 /* W/m*/ )  / V0x * (Xe - Xs - V0x)/40 /* pixel per metre */;


        std::cout << "shooting " << V0x <<" "  <<"   " << V0y << " "<< " " <<  atan(V0y/V0x) << " " <<m_enemy->GetName() << std::endl;
        ActiveTeam().AccessWeapon().PrepareShoot(sqrt(V0y*V0y + V0x*V0x), /*Xe*/m_enemy->GetCenterX() - Xs > 0 ? atan(V0y/V0x) - angle: -atan(V0y/V0x) + angle);
        m_last_shoot_time = m_current_time;
      }
  }
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

const Character* AIShootModule::FindBazookaShootableEnemy(const Character& shooter) const
{
  FOR_ALL_LIVING_ENEMIES(shooter, team, character)
    return &(*character);
  return NULL;
}
const Character* AIShootModule::FindEnemy()
{
  if (m_has_finished) {
    return NULL;
  }

  if (m_enemy != NULL && !(m_enemy->IsDead())) {
    return m_enemy;
  }

  SetStrategy(NO_STRATEGY);

  // Proximity enemy ?
  m_enemy = FindProximityEnemy(ActiveCharacter());
  if ( m_enemy != NULL ) {

    SetStrategy(NEAR_FROM_ENEMY);

    GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" has decided to injure "
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

    SetStrategy(SHOOT_FROM_POINT);

    GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" will shoot "
                                     + m_enemy->GetName());

    // we choose between gun, sniper_rifle, shotgun and submachine gun
    if ( SelectFiringWeapon(m_angle) ) {
      ActiveCharacter().SetFiringAngle(m_angle);
      return m_enemy;
    }
  }

  m_enemy = FindBazookaShootableEnemy(ActiveCharacter());
  if (m_enemy != NULL) {
    SetStrategy(SHOOT_BAZOOKA);
    return m_enemy;
  }

  m_current_strategy = NO_STRATEGY;
  m_angle = 0;
  m_enemy = NULL;

  return m_enemy;
}

void AIShootModule::ChooseDirection() const
{
  if ( m_enemy ) {
    // TODO : Replace by a cleverer function
    if ( abs(ActiveCharacter().GetCenterX() - m_enemy->GetCenterX()) <= 10 )
      return;
    if (m_enemy->GetTestRect().Intersect(ActiveCharacter().GetTestRect()))
      {
        MSG_DEBUG("ai.shoot","%s is on or next to %s", ActiveCharacter().GetName().c_str(), m_enemy->GetName().c_str());
        return;
      }

    MSG_DEBUG("ai", "Character: %d, enemy %d",
              ActiveCharacter().GetCenterX(), m_enemy->GetCenterX());

    if ( ActiveCharacter().GetCenterX() < m_enemy->GetCenterX())
      ActiveCharacter().SetDirection(DIRECTION_RIGHT);
    else if ( ActiveCharacter().GetCenterX() > m_enemy->GetCenterX())
      ActiveCharacter().SetDirection(DIRECTION_LEFT);
    // else ActiveCharacter().GetCenterX() == m_enemy->GetCenterX()
  }
}

bool AIShootModule::Refresh(uint current_time)
{
  if (m_has_finished) {
    return true;
  }

  m_current_time = current_time;
  m_enemy = FindEnemy();
  ChooseDirection();

  switch (m_current_strategy) {

  case NO_STRATEGY:
    //ActiveTeam().SetWeapon(Weapon::WEAPON_SKIP_TURN);
    //Shoot();
    //break;

  case NEAR_FROM_ENEMY:
    // We are near enough of an enemy (perhaps not the first one we have choosen)
    FOR_ALL_LIVING_ENEMIES(ActiveCharacter(), team, character) {
//      if ( abs((*character).GetX() - ActiveCharacter().GetX()) <= 10 &&
//                 abs ((*character).GetY() - ActiveCharacter().GetY()) < 60 ) {
        if ( (*character).GetCenter().Distance( ActiveCharacter().GetCenter()) < 40) {
              if (&(*character) != m_enemy) {
                GameMessages::GetInstance()->Add(ActiveCharacter().GetName()+" changes target : "
                                                 + (*character).GetName());
              }
               m_enemy = &(*character);
               Shoot();
               // If IA selected ProximityWeapon, he needs to go back, in the opposite direction (otherwises BOOM :-) )
               ActiveCharacter().SetDirection( (ActiveCharacter().GetDirection()==DIRECTION_RIGHT) ? DIRECTION_LEFT : DIRECTION_RIGHT);
      }
    }
    break;

  case SHOOT_FROM_POINT:
    Shoot();
    return false;

  case SHOOT_BAZOOKA:
    ShootWithBazooka();
    return false;

  case SKIP_TURN:
    ActiveTeam().SetWeapon(Weapon::WEAPON_SKIP_TURN);
    Shoot();
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
  m_has_finished = false;
  SetStrategy(NO_STRATEGY);

  // Choose random direction for the moment
  ActiveCharacter().SetDirection( RandomSync().GetBool()?DIRECTION_LEFT:DIRECTION_RIGHT );
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

void AIShootModule::SetStrategy(strategy_t new_strategy)
{
  if (m_current_strategy != new_strategy) {
    MSG_DEBUG("ai", "%s changes his strategy: %d -> %d",
              ActiveCharacter().GetName().c_str(), m_current_strategy, new_strategy);
    if (IsLOGGING("ai")) {
      std::cout << "SetStrategy: " << new_strategy << std::endl;
    }
    m_current_strategy = new_strategy;
  }
}
