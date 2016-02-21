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
 * Polygon Generator. Generate various polygon shape on demand (including random one).
 *****************************************************************************/

#include <stdlib.h>
#include "graphic/polygon_generator.h"
#include "network/randomsync.h"
#include "tool/affine_transform.h"

const int PolygonGenerator::MIN_SPACE_BETWEEN_POINT = 50;

Polygon * PolygonGenerator::GenerateCircle(Double diameter, int nb_point)
{
  return PolygonGenerator::GenerateDentedCircle(diameter, nb_point, ZERO);
}

Polygon * PolygonGenerator::GenerateRectangle(Double width, Double height)
{
  Polygon * tmp = new Polygon();
  tmp->AddPoint(Point2d( width / TWO,  height / TWO));
  tmp->AddPoint(Point2d( width / TWO, -height / TWO));
  tmp->AddPoint(Point2d(-width / TWO, -height / TWO));
  tmp->AddPoint(Point2d(-width / TWO,  height / TWO));
  return tmp;
}

Polygon * PolygonGenerator::GenerateRectangle(const Point2d & orig, const Point2d & size)
{
  Polygon * tmp = new Polygon();
  tmp->AddPoint(Point2d(orig.x + size.x, orig.y + size.y));
  tmp->AddPoint(Point2d(orig.x + size.x, orig.y));
  tmp->AddPoint(Point2d(orig.x,          orig.y));
  tmp->AddPoint(Point2d(orig.x,          orig.y + size.y));
  return tmp;
}

Polygon * PolygonGenerator::GenerateRectangle(const Rectanglei & r)
{
  return PolygonGenerator::GenerateRectangle(POINT2I_2_POINT2D(r.GetPosition()),
                                             POINT2I_2_POINT2D(r.GetSize()));
}

Polygon * PolygonGenerator::GenerateRectangle(const Point2i & orig, const Point2i & size)
{
  return PolygonGenerator::GenerateRectangle(POINT2I_2_POINT2D(orig),
                                             POINT2I_2_POINT2D(size));
}

Polygon * PolygonGenerator::GenerateDentedCircle(Double diameter, int nb_point, Double rand_offset)
{
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top;
  for(int i = 0; i < nb_point; i++) {
    top = Point2d(ZERO, (diameter + RandomSync().GetDouble(-rand_offset, rand_offset)) / TWO);
    trans.SetRotation((TWO * PI * -i) / nb_point);
    tmp->AddPoint(trans * top);
  }
  return tmp;
}

Polygon * PolygonGenerator::GenerateRoundedRectangle(Double width, Double height, Double edge)
{
  Polygon * tmp = new Polygon();
  Double edge_vector = edge / TWO;
  tmp->AddBezierCurve(Point2d(-width / 2 + edge, -height / 2),
                      Point2d(-edge_vector, 0),
                      Point2d(0, -edge_vector),
                      Point2d(-width / 2, -height / 2 + edge));
  tmp->AddBezierCurve(Point2d(-width / 2, height / 2 - edge),
                      Point2d(0, edge_vector),
                      Point2d(-edge_vector, 0),
                      Point2d(-width / 2 + edge, height / 2));
  tmp->AddBezierCurve(Point2d(width / 2 - edge, height / 2),
                      Point2d(edge_vector, 0),
                      Point2d(0, edge_vector),
                      Point2d(width / 2, height / 2 - edge));
  tmp->AddBezierCurve(Point2d(width / 2, -height / 2 + edge),
                      Point2d(0, -edge_vector),
                      Point2d(edge_vector, 0),
                      Point2d(width / 2 - edge, -height / 2));
  return tmp;
}

Polygon * PolygonGenerator::GenerateRandomShape()
{
  Double height = RandomSync().GetDouble(400.0, 600.0);
  Double width  = RandomSync().GetDouble(400.0, 2000.0);
  return GenerateRandomTrapeze(width, height, RandomSync().GetDouble(10.0, 15.0), RandomSync().GetDouble(10.0, 15.0),
                               RandomSync().GetSign() * RandomSync().GetDouble(0.5, 1.0));
}

Polygon * PolygonGenerator::GenerateRandomTrapeze(const Double width, const Double height,
                                                  const Double x_rand_offset, const Double y_rand_offset,
                                                  const Double coef)
{
  Double upper_width, lower_width, upper_offset, lower_offset;
  int number_of_bottom_point, number_of_side_point;
  // XXX Unused !?
  // int number_of_upper_point;
  Polygon * tmp = new Polygon();
  number_of_side_point = 1 + (int)RandomSync().GetDouble((height / FOUR) / MIN_SPACE_BETWEEN_POINT,
                                     height / MIN_SPACE_BETWEEN_POINT);
  if(coef > ZERO) {
    upper_width = width;
    lower_width = width * coef;
    upper_offset = RandomSync().GetDouble(ZERO, width - lower_width);
    lower_offset = ZERO;
  } else {
    upper_width = - width * coef;
    lower_width = width;
    upper_offset = ZERO;
    lower_offset = RandomSync().GetDouble(ZERO, width - upper_width);
  }
  // XXX Unused !?
  //number_of_upper_point = RandomSync().GetInt(1 + (int)((upper_width * 0.25) / MIN_SPACE_BETWEEN_POINT),
  //                                       (int)(upper_width / MIN_SPACE_BETWEEN_POINT));
  number_of_bottom_point = RandomSync().GetInt(1 + (int)((lower_width / FOUR) / MIN_SPACE_BETWEEN_POINT),
                                          (int)((coef * lower_width) / MIN_SPACE_BETWEEN_POINT));
  tmp->AddRandomCurve(Point2d(upper_offset, ZERO), Point2d(lower_offset, height),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  tmp->AddRandomCurve(Point2d(lower_offset, height), Point2d(lower_offset + lower_width, height),
                      x_rand_offset, y_rand_offset, number_of_bottom_point, false, false);
  tmp->AddRandomCurve(Point2d(lower_offset + lower_width, height), Point2d(upper_offset + upper_width, ZERO),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  tmp->AddRandomCurve(Point2d(upper_offset + upper_width, ZERO), Point2d(upper_offset, ZERO),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  return tmp;
}

Polygon * PolygonGenerator::GeneratePie(Double diameter, int nb_point, Double angle, Double angle_offset)
{
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top;
  for(int i = 0; i < nb_point; i++) {
    top = Point2d(ZERO, diameter / TWO);
    trans.SetRotation(angle_offset + ((i * angle) / nb_point));
    tmp->AddPoint(trans * top);
  }
  if(angle < 2 * PI)
    tmp->AddPoint(Point2d(ZERO, ZERO));
  return tmp;
}

Polygon * PolygonGenerator::GeneratePartialTorus(Double diameter, Double min_diameter, int nb_point, Double angle, Double angle_offset)
{
  if(diameter < min_diameter) {
    Double tmp = diameter;
    diameter = min_diameter;
    min_diameter = tmp;
  }
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top = Point2d(ZERO, diameter / TWO);
  for(int i = 0; i < nb_point; i++) {
    trans.SetRotation(angle_offset + ((i * angle) / (nb_point - 1)));
    tmp->AddPoint(trans * top);
  }
  top = Point2d(ZERO, min_diameter / TWO);
  for(int i = nb_point - 1; i >= 0; i--) {
    trans.SetRotation(angle_offset + ((i * angle) / (nb_point - 1)));
    tmp->AddPoint(trans * top);
  }
  return tmp;
}

DecoratedBox * PolygonGenerator::GenerateDecoratedBox(Double width, Double height)
{
  DecoratedBox * tmp = new DecoratedBox(width, height);

  return tmp;
}

