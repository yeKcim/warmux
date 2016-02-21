/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Represtents he trajectory of a missile.
 *****************************************************************************/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <WORMUX_point.h>

class Trajectory {
  private:
    Point2d initial_position;
    Point2d initial_speed;
    Point2d acceleration;
    Point2d half_acceleration;
  public:
    Trajectory(Point2d pos_0, Point2d v_0, Point2d a);
    const Point2i GetPositionAt(Double time);
    Double GetSpeedAt(Double time);
};
#endif
