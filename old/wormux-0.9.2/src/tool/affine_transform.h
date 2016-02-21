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
 * 2D Affine Transformation.
 *****************************************************************************/

#ifndef AFFINE_TRANSFORM_2D
#define AFFINE_TRANSFORM_2D

#include <WORMUX_types.h>
#include <stdio.h>
#include <WORMUX_point.h>

class AffineTransform2D {
 protected:
  // The matrix
  Double x1, x2, xt;
  Double y1, y2, yt;
  Double w1, w2, wt;
 public:
  AffineTransform2D();
  void Init();
  void Set(const AffineTransform2D &mat);
  void SetRotation(Double rad_angle);
  void SetTranslation(Double trans_x, Double trans_y);
  void SetTranslation(const Point2i & position);
  void SetTranslation(const Point2d & position);
  void SetShrink(Double shrink_x, Double shrink_y);
  void SetShear(Double shear_x, Double shear_y);
  void SetTranslationAnimation(int start_time, int duration, int time, bool invert, const Point2d & start, const Point2d & end);
  void SetRotationAnimation(int start_time, int duration, int time, bool invert, Double angle_start, Double angle_end = 0.0);
  void SetShrinkAnimation(int start_time, int duration, int time, bool invert, Double shrink_x_start, Double shrink_y_start,
                          Double shrink_x_end = 1.0, Double shrink_y_end = 1.0);
  void SetShearAnimation(int start_time, int duration, int time, bool invert, Double shear_tremor,
                         Double shear_x_start, Double shear_y_start,
                         Double shear_x_end = 0.0, Double shear_y_end = 0.0);
  /* Matrix multiplication */
  AffineTransform2D operator*(const AffineTransform2D &mat) const;
  Point2i operator*(const Point2i& p) const;
  Point2d operator*(const Point2d& p) const;
  // Creation of instance
  static AffineTransform2D Rotate(Double rad_angle);
  static AffineTransform2D Translate(Double trans_x, Double trans_y);
  static AffineTransform2D Shrink(Double shrink_x, Double shrink_y);
  static AffineTransform2D Shear(Double shear_x, Double shear_y);
};

#endif /* AFFINE_TRANSFORM_2D */
