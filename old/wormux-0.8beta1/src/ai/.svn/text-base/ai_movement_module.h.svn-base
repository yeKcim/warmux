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
 * Artificial intelligence Movement module
 *****************************************************************************/
#ifndef AI_MOVEMENT_MODULE
#define AI_MOVEMENT_MODULE

#include "tool/point.h"
#include <set>

class AIMovementModule
{
  uint m_current_time;

  // ====================== Points to avoid
 private:
  std::set<Point2i> points_to_avoid;
  void UpdateListOfPointsToAvoid();
 public:
  void AddPointToAvoid(Point2i dangerous_point);
  // ======================================

  // ==================== Destination point
 private:
  uint min_reachable_x, max_reachable_x;
  Point2i destination_point;
 public:
  void SetDestinationPoint(Point2i destination_point);
  bool SeemsToBeReachable(const Character& shooter, // must be ActiveCharacter()
			  const Character& enemy) const;
  bool IsProgressing();
  bool IsArrived();
  // ======================================
  
  // ====================== Manage movement
 private:  
  typedef enum {
    NO_MOVEMENT,
    WALKING,
    BACK_TO_JUMP,
    JUMPING,
    FLYING,
    ROPING,
  } movement_type_t;

  movement_type_t current_movement;

  Point2i last_position;
  uint time_at_last_position; 
  Point2i last_blocked_position;
  
  void InverseDirection(bool completely_blocked);

  void MakeStep();

  void Walk();
  void StopWalking();

  void PrepareJump();
  void GoBackToJump();
  void Jump();
  void EndOfJump();

  bool ObstacleHeight(int& height);
  bool RiskGoingOutOfMap();
  // ======================================

 public:
  AIMovementModule();
  void BeginTurn();

  void Move(uint current_time);
  void StopMoving();

};

#endif
