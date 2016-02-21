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

#include "include/base.h"
#include "tool/point.h"
#include <set>

class Character;

class AIMovementModule
{
  uint m_current_time;

  // ====================== Points to avoid
  std::set<Point2i> points_to_avoid;
  void UpdateListOfPointsToAvoid();
 public:
  void AddPointToAvoid(const Point2i& dangerous_point);
  // ======================================

  // ==================== Destination point
 private:
  int min_reachable_x, max_reachable_x;
  Point2i destination_point;
 public:
  void SetDestinationPoint(const Point2i& destination_point);
  bool SeemsToBeReachable(const Character& shooter, // must be ActiveCharacter()
                          const Character& enemy) const;
  bool IsProgressing() const;
  bool IsArrived() const;
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
    BLOCKED
  } movement_type_t;
  // Please, never modify m_current_movement directly
  // but always use SetMovement, this is better for debugging
  movement_type_t m_current_movement;
  void SetMovement(movement_type_t move);
  movement_type_t GetCurrentMovement() const;

  Point2i last_position;
  uint time_at_last_position;
  Point2i last_blocked_position;

  void InverseDirection(bool completely_blocked);

  void MakeStep() const;

  void Walk();
  void StopWalking();

  void PrepareJump();
  void GoBackToJump();
  void Jump();
  void EndOfJump();

  bool ObstacleHeight(int& height) const;
  bool RiskGoingOutOfMap() const;
  // ======================================

 public:
  AIMovementModule();
  void BeginTurn();

  void Move(uint current_time);
  void StopMoving();

};

#endif
