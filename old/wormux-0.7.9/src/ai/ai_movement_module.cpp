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
 * Artificial intelligence Movement module
 *****************************************************************************/

#include <iostream>
#include "../include/action_handler.h"
#include "../character/body.h"
#include "../character/move.h"
#include "../interface/game_msg.h"
#include "../map/map.h"
#include "../team/macro.h"
#include "../team/teams_list.h"
#include "../tool/error.h"
#include "../tool/math_tools.h"
#include "../tool/string_tools.h"

#include "ai_movement_module.h"

// TODO: 
// - Be sure to not go out of the map
// - Fix hole detection
// - Do not go back on the shoot position !


// =================================================
// Go on !
// =================================================
void AIMovementModule::MakeStep()
{
  if(ActiveCharacter().IsImmobile()) {
    if (ActiveCharacter().GetDirection() == Body::DIRECTION_RIGHT) {
      MoveCharacterRight(ActiveCharacter());
    }  else {
      MoveCharacterLeft(ActiveCharacter());
    }
  }
}

// =================================================
// Compute obstacle height (barrier or hole)
// Return true if sure that it does not need to jump
// or to use parachute
// =================================================
bool AIMovementModule::ObstacleHeight(int& height)
{
  if (ComputeHeightMovement(ActiveCharacter(), height, false))
    return true;

  int y_floor=ActiveCharacter().GetY();
  if (height < 0) {

    for (height = -15; height >= -150 ; height--) {
      if ( ActiveCharacter().IsInVacuum( Point2i(ActiveCharacter().GetDirection(), height) ) ) {
        break;
      }
    }

  } else {

    // Compute exact whole size
    for (height = 15; height <= 150 ; height++){
      if ( !ActiveCharacter().IsInVacuum(Point2i(ActiveCharacter().GetDirection(), height))
	  ||  ActiveCharacter().FootsOnFloor(y_floor+height)){
        break;
      }
    }

  }
  return false;
}

bool AIMovementModule::RiskGoingOutOfMap()
{
  if ( ActiveCharacter().GetDirection() == Body::DIRECTION_LEFT &&
       ActiveCharacter().GetX() <= 5 ) {
    return true;
  } else if ( ActiveCharacter().GetDirection() == Body::DIRECTION_RIGHT &&
	      world.GetWidth() - 5 <= ActiveCharacter().GetX() + ActiveCharacter().GetSize().GetX() ) {
    return true;
  }

  return false;
}

// =================================================
// A jump is made of many movement :
// 1) Decect collision and prepare to go back
// 2) Go back, then when far enought of the obstacle
// 3) Jump!
// 4) Detect when on the ground! 
// =================================================

void AIMovementModule::PrepareJump()
{
  ActiveCharacter().body->StartWalk();
  
  current_movement = BACK_TO_JUMP;
  time_at_last_position = m_current_time;

  InverseDirection();
}

void AIMovementModule::GoBackToJump()
{
  assert(current_movement = BACK_TO_JUMP);

  MakeStep();

  int height;
  bool blocked = !(ObstacleHeight(height));

  if ( abs(last_position.GetX() - ActiveCharacter().GetPosition().GetX()) >= 20
       || time_at_last_position +1 < m_current_time
       || blocked) {
    //it's time to jump!
    InverseDirection();
    Jump();
  }
}

void AIMovementModule::Jump()
{
  //  GameMessages::GetInstance()->Add("try to jump!");
  current_movement = JUMPING;
  ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_HIGH_JUMP));
}

void AIMovementModule::EndOfJump()
{
  assert(current_movement = JUMPING);

  //GameMessages::GetInstance()->Add("finished to jump");
  
  
  if ( last_position.GetX() == ActiveCharacter().GetPosition().GetX() ) {
    // we have not moved since last movement
    StopMoving();
    
  } else {
    // No more blocked !!
    current_movement = WALKING;
  }  
}


// =================================================
// The walking methods
// Walk() is responsible to detect obstacles
// =================================================

void AIMovementModule::Walk()
{
  // Animate skin
  if ( current_movement != WALKING ) {
    ActiveCharacter().InitMouvementDG(100);
    ActiveCharacter().body->StartWalk();
    current_movement = WALKING;
  }

  MakeStep();

  int height;
  bool blocked = !(ObstacleHeight(height));

  // we are blocked, what next ?
  if ( blocked ) {

    if ( last_blocked_position.GetX() != ActiveCharacter().GetPosition().GetX() ) {
      
      last_blocked_position = ActiveCharacter().GetPosition();
      
      if (height < 0 ) {
	// There's a barrier
	
	if (height >= -80) { // we can try to jump!
	  PrepareJump();
	  return; // do not update position
	} else { // it's too high!
	  //	  GameMessages::GetInstance()->Add("It's too high!!");
	  InverseDirection();
	}
      } else {
	// There's a hole
	
	if (height >= 100) { // it's too deep, go back!!
	  //	  GameMessages::GetInstance()->Add("It's too deep!" + ulong2str(height));
	  InverseDirection();
	}
      }
    } else {
      // already have been blocked here...
      InverseDirection();
    }
  }

  // Inverse direction if there is a risk to go out of the map
  if (RiskGoingOutOfMap()) {
    InverseDirection();
  }

  // Update position if we are not jumping
  last_position = ActiveCharacter().GetPosition();
  time_at_last_position = m_current_time;
}

void AIMovementModule::StopWalking()
{
  current_movement = NO_MOVEMENT;
  ActiveCharacter().body->StopWalk();
}


// =================================================
// Invert walking direction
// =================================================
void AIMovementModule::InverseDirection()
{
  if (ActiveCharacter().GetDirection() == Body::DIRECTION_RIGHT) {
    ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
  } else {
    ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
  }
}

// =================================================
// Public method updating the movement
// =================================================
void AIMovementModule::Move(uint current_time)
{
  m_current_time = current_time;

  // are we on the ground ?
  if ( ActiveCharacter().FootsInVacuum() ) { // No!
    return;
  }

  switch (current_movement) {

  case NO_MOVEMENT:
    // Begin to walk
    Walk();
    break;

  case WALKING:
    // Continue to walk
    Walk();
    break;

  case BACK_TO_JUMP:
    // Go back to have enough place to jump
    GoBackToJump();
    break;

  case JUMPING:
    EndOfJump();
 
    break;
  default:
    break;
  }
}

void AIMovementModule::StopMoving()
{
  //  GameMessages::GetInstance()->Add("stop moving");
  StopWalking();
  //m_step++;
}

// =================================================
// Initialize Movement module when changing 
// character to control
// =================================================
void AIMovementModule::BeginTurn()
{
  current_movement = NO_MOVEMENT;
  time_at_last_position = 0;
  last_position = Point2i(0,0);
  last_blocked_position = Point2i(0,0);
}

AIMovementModule::AIMovementModule()
{
  std::cout << "o Artificial Intelligence Movement module initialization" << std::endl;
}

// ====================== Points to avoid
// private:
//  std::set<Point2i> points_to_avoid;

void AIMovementModule::UpdateListOfPointsToAvoid()
{
  // TODO : Refresh position of mines
}

void AIMovementModule::AddPointToAvoid(Point2i dangerous_point)
{
  points_to_avoid.insert(dangerous_point);
}

// ======================================

// ==================== Destination point
// private:
//  uint min_reachable_x, max_reachable_x;
//  Point2i destination_point;

void AIMovementModule::SetDestinationPoint(Point2i _destination_point)
{
  destination_point = _destination_point;
}

// =================================================
// STATIC METHOD
// =================================================
// Return true if character seems to be accessible
// =================================================
// This method is not perfect!!
// =================================================
bool AIMovementModule::SeemsToBeReachable(const Character& shooter,
					  const Character& enemy) // replace method IsNear()
{
  uint delta_x = abs(shooter.GetX() - enemy.GetX());
  uint delta_y = abs(shooter.GetY() - enemy.GetY());

  if (delta_x > 300)
    return false;

  if (delta_y > 100)
    return false;

  // TODO : check the min_reachable_x an max_reachable_x

  return true;
}

bool AIMovementModule::IsProgressing()
{
  // TODO
  return true;
}


bool AIMovementModule::IsArrived()
{
  return (ActiveCharacter().GetPosition() == destination_point);
}

// ======================================
