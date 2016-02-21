/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 ******************************************************************************
 *
 * This file is derived from "ScummVM - Graphic Adventure Engine"
 * Original licence below:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef COMMON_POLYGON_H
#define COMMON_POLYGON_H

#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD
#include <WARMUX_rectangle.h>
#include <WARMUX_point.h>
#include <vector>

namespace Common {

using namespace std;

struct Polygon
{

  Polygon()
  {
  }
  Polygon(vector<Point2i> p) :
    _points(p)
  {
    for (uint i = 0; i < p.size(); i++) {
      //	_bound.extend(Rect(p[i].x, p[i].y, p[i].x, p[i].y));
    }
  }
  Polygon(Point2i *p, int n)
  {
    for (int i = 0; i < n; i++) {
      addPoint(p[i]);
    }
  }

  void addPoint(const Point2i& p)
  {
    _points.push_back(p);
    //_bound.extend(Rect(p.x, p.y, p.x, p.y));
  }

  void addPoint(int x, int y)
  {
    addPoint(Point2i(x, y));
  }

  uint getPointCount()
  {
    return _points.size();
  }

  /**
   * Check if given position is inside this polygon.
   *
   * @param x the horizontal position to check
   * @param y the vertical position to check
   * @return true if the given position is inside this polygon, false otherwise
   */
  bool contains(int x, int y) const;

  /**
   * Check if given point is inside this polygon.
   *
   * @param p the point to check
   * @return true if the given point is inside this polygon, false otherwise
   */
  bool contains(const Point2i &p) const
  {
    return contains(p.x, p.y);
  }

  void moveTo(int x, int y)
  {
    int dx = x - ((_bound.GetRight() + _bound.GetLeft()) / 2);
    int dy = y - ((_bound.GetBottom() + _bound.GetTop()) / 2);
    translate(dx, dy);
  }

  void moveTo(const Point2i &p)
  {
    moveTo(p.x, p.y);
  }

  void translate(int dx, int dy)
  {
    vector<Point2i>::iterator it;
    for (it = _points.begin(); it != _points.end(); it++) {
      it->x += dx;
      it->y += dy;
    }
  }

  Rectanglei getBoundingRect() const
  {
    return _bound;
  }

private:
  vector<Point2i> _points;
  Rectanglei _bound;
};

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
#endif // #ifndef COMMON_POLYGON_H
