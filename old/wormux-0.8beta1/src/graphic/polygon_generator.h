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
 * Polygon class. Store point of a polygon and handle affine transformation
 *****************************************************************************/

#ifndef POLYGON_GENERATOR_H
#define POLYGON_GENERATOR_H

#include "polygon.h"

class PolygonGenerator {
 public:
   static const int MIN_SPACE_BETWEEN_POINT;
   static Polygon * GenerateCircle(double diameter, int nb_point);
   static Polygon * GenerateDentedCircle(double diameter, int nb_point, double rand_offset);
   static Polygon * GenerateRectangle(double width, double height);
   static Polygon * GenerateRectangle(const Rectanglei & rectangle);
   static Polygon * GenerateRectangle(const Point2d & orig, const Point2d & size);
   static Polygon * GenerateRectangle(const Point2i & orig, const Point2i & size);
   static Polygon * GenerateRoundedRectangle(double width, double height, double edge);
   static Polygon * GenerateRandomShape();
   static Polygon * GenerateRandomTrapeze(const double width, const double height,
                                          const double x_rand_offset, const double y_rand_offset,
                                          const double coef);
};

#endif /* POLYGON_GENERATOR_H */
