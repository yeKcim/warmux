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
 *****************************************************************************/

#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <map>
#include <vector>
#include "include/base.h"
#include "tool/point.h"

// Forward declaration
namespace xmlpp
{
  class Element;
}

class member_mvt
{  // Position of a member relative to its superior one
  double angle_rad; // angle in radian
public:
  Point2f pos;
  Point2f scale;
  /* SetAngle take radian values */
  inline void SetAngle(double angle)
  {
    while(angle_rad > 2*M_PI)
      angle_rad -= 2 * M_PI;
    while(angle_rad <= -2*M_PI)
      angle_rad += 2 * M_PI;
    angle_rad = angle;
  }
  /* GetAngle returns radian values */
  inline const double &GetAngle() const { return angle_rad; }
  float alpha;
  int follow_cursor_limit;
  bool follow_cursor;
  bool follow_crosshair;
  bool follow_half_crosshair;
  bool follow_speed;
  bool follow_direction;
  member_mvt(): angle_rad(0), pos(0.0, 0.0), scale(1.0, 1.0), alpha(1),
                follow_cursor_limit(0), follow_cursor(false),
                follow_crosshair(false), follow_half_crosshair(false),
                follow_speed(false), follow_direction(false)
  { };
};

class Movement
{
  typedef std::map<std::string, class member_mvt> member_def; // Describe the position of each member for a given frame

public:
  std::vector<member_def> frames;
  bool always_moving;
  int speed;
  uint test_left, test_right, test_top, test_bottom;
  enum
  {
    LOOP,
    PLAY_ONCE
  } play_mode;

  std::string type;

  ~Movement();
  Movement(xmlpp::Element *xml);
};

#endif //MEMBER_H
