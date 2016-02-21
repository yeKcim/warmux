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
 * Polygon Generator. Generate various polygon shape on demand (including random one).
 *****************************************************************************/

#include <stdlib.h>
#include "graphic/polygon_generator.h"
#include "tool/random.h"
#include "tool/affine_transform.h"

const int PolygonGenerator::MIN_SPACE_BETWEEN_POINT = 50;

Polygon * PolygonGenerator::GenerateCircle(double diameter, int nb_point)
{
  return PolygonGenerator::GenerateDentedCircle(diameter, nb_point, 0.0);
}

Polygon * PolygonGenerator::GenerateRectangle(double width, double height)
{
  Polygon * tmp = new Polygon();
  tmp->AddPoint(Point2d( width / 2.0,  height / 2.0));
  tmp->AddPoint(Point2d( width / 2.0, -height / 2.0));
  tmp->AddPoint(Point2d(-width / 2.0, -height / 2.0));
  tmp->AddPoint(Point2d(-width / 2.0,  height / 2.0));
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

Polygon * PolygonGenerator::GenerateDentedCircle(double diameter, int nb_point, double rand_offset)
{
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top;
  for(int i = 0; i < nb_point; i++) {
    top = Point2d(0.0, (diameter + RandomLocal().GetDouble(-rand_offset, rand_offset)) / 2.0);
    trans.SetRotation((2.0 * M_PI * -i) / nb_point);
    tmp->AddPoint(trans * top);
  }
  return tmp;
}

Polygon * PolygonGenerator::GenerateRoundedRectangle(double width, double height, double edge)
{
  Polygon * tmp = new Polygon();
  double edge_vector = edge / 2.0;
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
  double height = RandomLocal().GetDouble(400.0, 600.0);
  double width  = RandomLocal().GetDouble(400.0, 2000.0);
  return GenerateRandomTrapeze(width, height, RandomLocal().GetDouble(10.0, 15.0), RandomLocal().GetDouble(10.0, 15.0),
                               RandomLocal().GetSign() * RandomLocal().GetDouble(0.5, 1.0));
}

Polygon * PolygonGenerator::GenerateRandomTrapeze(const double width, const double height,
                                                  const double x_rand_offset, const double y_rand_offset,
                                                  const double coef)
{
  double upper_width, lower_width, upper_offset, lower_offset;
  int number_of_bottom_point, number_of_side_point;
  // XXX Unused !?
  // int number_of_upper_point;
  Polygon * tmp = new Polygon();
  number_of_side_point = 1 + (int)RandomLocal().GetDouble((height * 0.25) / MIN_SPACE_BETWEEN_POINT,
                                     height / MIN_SPACE_BETWEEN_POINT);
  if(coef > 0.0) {
    upper_width = width;
    lower_width = width * coef;
    upper_offset = RandomLocal().GetDouble(0.0, width - lower_width);
    lower_offset = 0.0;
  } else {
    upper_width = - width * coef;
    lower_width = width;
    upper_offset = 0.0;
    lower_offset = RandomLocal().GetDouble(0.0, width - upper_width);
  }
  // XXX Unused !?
  //number_of_upper_point = RandomLocal().GetInt(1 + (int)((upper_width * 0.25) / MIN_SPACE_BETWEEN_POINT),
  //                                       (int)(upper_width / MIN_SPACE_BETWEEN_POINT));
  number_of_bottom_point = RandomLocal().GetInt(1 + (int)((lower_width * 0.25) / MIN_SPACE_BETWEEN_POINT),
                                          (int)((coef * lower_width) / MIN_SPACE_BETWEEN_POINT));
  tmp->AddRandomCurve(Point2d(upper_offset, 0.0), Point2d(lower_offset, height),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  tmp->AddRandomCurve(Point2d(lower_offset, height), Point2d(lower_offset + lower_width, height),
                      x_rand_offset, y_rand_offset, number_of_bottom_point, false, false);
  tmp->AddRandomCurve(Point2d(lower_offset + lower_width, height), Point2d(upper_offset + upper_width, 0.0),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  tmp->AddRandomCurve(Point2d(upper_offset + upper_width, 0.0), Point2d(upper_offset, 0.0),
                      x_rand_offset, y_rand_offset, number_of_side_point, false, false);
  return tmp;
}

Polygon * PolygonGenerator::GeneratePie(double diameter, int nb_point, double angle, double angle_offset)
{
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top;
  for(int i = 0; i < nb_point; i++) {
    top = Point2d(0.0, diameter / 2.0);
    trans.SetRotation(angle_offset + ((i * angle) / nb_point));
    tmp->AddPoint(trans * top);
  }
  if(angle < 2 * M_PI)
    tmp->AddPoint(Point2d(0.0, 0.0));
  return tmp;
}

Polygon * PolygonGenerator::GeneratePartialTorus(double diameter, double min_diameter, int nb_point, double angle, double angle_offset)
{
  if(diameter < min_diameter) {
    double tmp = diameter;
    diameter = min_diameter;
    min_diameter = tmp;
  }
  Polygon * tmp = new Polygon();
  AffineTransform2D trans = AffineTransform2D();
  Point2d top = Point2d(0.0, diameter / 2.0);
  for(int i = 0; i < nb_point; i++) {
    trans.SetRotation(angle_offset + ((i * angle) / (nb_point - 1)));
    tmp->AddPoint(trans * top);
  }
  top = Point2d(0.0, min_diameter / 2.0);
  for(int i = nb_point - 1; i >= 0; i--) {
    trans.SetRotation(angle_offset + ((i * angle) / (nb_point - 1)));
    tmp->AddPoint(trans * top);
  }
  return tmp;
}
