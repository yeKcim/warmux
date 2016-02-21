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
 *****************************************************************************/

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <vector>
#include <WARMUX_base.h>
#include <WARMUX_point.h>
#include <WARMUX_types.h>

#include "character/member_type.h"
#include "tool/math_tools.h"

typedef struct _xmlNode xmlNode;

// Position of a member relative to its superior one
class member_mvt
{  
  MemberType type;
  Double angle_rad; // angle in radian
public:
  Point2i pos;
  Point2d scale;
  /* SetAngle take radian values */
  void SetAngle(Double angle) { angle_rad = RestrictAngle(angle); }
  /* GetAngle returns radian values */
  const Double &GetAngle() const { return angle_rad; }
  Double alpha;
  int follow_cursor_square_limit;
  bool follow_crosshair;
  bool follow_half_crosshair;
  bool follow_speed;
  bool follow_direction;
  member_mvt(const std::string& name = DUMMY_MEMBER)
    : type(name)
    , angle_rad(ZERO), pos(0, 0), scale(ONE, ONE), alpha(ONE)
    , follow_cursor_square_limit(0)
    , follow_crosshair(false), follow_half_crosshair(false)
    , follow_speed(false), follow_direction(false)
  { };
  operator const MemberType& () const { return type; }
  bool operator==(const MemberType& other) const { return type == other; }
  bool operator!=(const MemberType& other) const { return type != other; }
};

class Movement
{
public:
  typedef std::vector<class member_mvt> member_def; // Describe the position of each member for a given frame

private:
  uint ref_count;

  std::vector<member_def> frames;
  std::string type;
  uint nb_loops; // 0 means forever
  uint duration_per_frame; // in milli-second
  bool always_moving;
  uint test_left, test_right, test_top, test_bottom;

public:
  Movement(const xmlNode* xml);
  ~Movement() { };

  void SetType(const std::string& _type) { type = _type; }
  const std::string& GetType() const { return type; }

  uint GetFrameDuration() const { return duration_per_frame; }
  uint GetNbLoops() const { return nb_loops; }

  bool IsAlwaysMoving() const { return always_moving; }

  // TODO lami: use a pointer ... std::vector<Movement::member_def *>
  const std::vector<Movement::member_def> & GetFrames() const { return frames; }

  uint GetTestLeft() const { return test_left; }
  uint GetTestRight() const { return test_right; }
  uint GetTestTop() const { return test_top; }
  uint GetTestBottom() const { return test_bottom; }

  static void ShareMovement(Movement* mvt) { mvt->ref_count++; }
  static void UnshareMovement(Movement* mvt)
  {
    mvt->ref_count--;
    if (!mvt->ref_count)
      delete mvt;
  }
};

#endif //MEMBER_H
