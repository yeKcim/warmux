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
* 2D Affine Transformation
* Note, You need to do every transformation in this order :
* rotation * shrink * translation;
*****************************************************************************/

#include "tool/affine_transform.h"

AffineTransform2D::AffineTransform2D()
{
  Init();
}

void AffineTransform2D::Init()
{
  x1 = 1; x2 = 0; xt = 0;
  y1 = 0; y2 = 1; yt = 0;
  w1 = 0; w2 = 0; wt = 1;
}

void AffineTransform2D::Set(const AffineTransform2D &mat)
{
  x1 = mat.x1;
  x2 = mat.x2;
  xt = mat.xt;
  y1 = mat.y1;
  y2 = mat.y2;
  yt = mat.yt;
  w1 = mat.w1;
  w2 = mat.w2;
  wt = mat.wt;
}

void AffineTransform2D::SetRotation(Double rad_angle)
{
  Init();
  x1 = y2 = cos(rad_angle);
  x2 = - sin(rad_angle);
  y1 = - x2;
}

void AffineTransform2D::SetTranslation(Double trans_x, Double trans_y)
{
  Init();
  xt = trans_x;
  yt = trans_y;
}

void AffineTransform2D::SetTranslation(const Point2i & position)
{
  SetTranslation((Double)position.x, (Double)position.y);
}

void AffineTransform2D::SetTranslation(const Point2d & position)
{
  SetTranslation(position.x, position.y);
}

void AffineTransform2D::SetShrink(Double shrink_x, Double shrink_y)
{
  Init();
  x1 = shrink_x;
  y2 = shrink_y;
}

void AffineTransform2D::SetShear(Double shear_x, Double shear_y)
{
  Init();
  x2 = shear_x;
  y1 = shear_y;
}

void AffineTransform2D::SetTranslationAnimation(int start_time, int duration, int time, bool invert,
                                                const Point2d & start, const Point2d & end)
{
  Double coef = (time - start_time) / (Double)duration;
  coef = (coef > ZERO ? coef : ZERO);
  coef = (coef < ONE ? coef : ONE);
  if(invert)
    coef = ONE - coef;
  SetTranslation(start + (end - start) * coef);
}

void AffineTransform2D::SetRotationAnimation(int start_time, int duration, int time, bool invert,
                                             Double angle_start, Double angle_end)
{
  Double coef = (time - start_time) / (Double)duration;
  coef = (coef > ZERO ? coef : ZERO);
  coef = (coef < ONE ? coef : ONE);
  if(invert)
    coef = ONE - coef;
  SetRotation(angle_start + coef * (angle_end - angle_start));
}

void AffineTransform2D::SetShrinkAnimation(int start_time, int duration, int time, bool invert,
                                           Double shrink_x_start, Double shrink_y_start,
                                           Double shrink_x_end, Double shrink_y_end)
{
  Double coef = (time - start_time) / (Double)duration;
  coef = (coef > ZERO ? coef : ZERO);
  coef = (coef < ONE ? coef : ONE);
  if(invert)
    coef = ONE - coef;
  SetShrink(shrink_x_start + (shrink_x_end - shrink_x_start) * coef, shrink_y_start + (shrink_y_end - shrink_y_start) * coef);
}

void AffineTransform2D::SetShearAnimation(int start_time, int duration, int time, bool invert,
                                          Double shear_tremor, Double shear_x_start, Double shear_y_start,
                                          Double shear_x_end, Double shear_y_end)
{
  if(time < start_time + duration && time > start_time) {
    Double coef = ONE - (time - start_time) / (Double)duration;
    if(invert)
    coef = ONE - coef;
    coef = -(cos((ONE - coef) * PI * TWO * shear_tremor) * coef);
    SetShear(shear_x_end + (coef * (shear_x_start - shear_x_end)), shear_y_end + (coef * (shear_y_start - shear_y_end)));
  } else {
    Init();
  }
}

AffineTransform2D AffineTransform2D::Rotate(Double rad_angle)
{
  AffineTransform2D tmp;
  tmp.SetRotation(rad_angle);
  return tmp;
}

AffineTransform2D AffineTransform2D::Translate(Double trans_x, Double trans_y)
{
  AffineTransform2D tmp;
  tmp.SetTranslation(trans_x, trans_y);
  return tmp;
}

AffineTransform2D AffineTransform2D::Shrink(Double shrink_x, Double shrink_y)
{
  AffineTransform2D tmp;
  tmp.SetShrink(shrink_x, shrink_y);
  return tmp;
}

AffineTransform2D AffineTransform2D::Shear(Double shear_x, Double shear_y)
{
  AffineTransform2D tmp;
  tmp.SetShear(shear_x, shear_y);
  return tmp;
}

/* Matrix multiplication */
AffineTransform2D AffineTransform2D::operator*(const AffineTransform2D &mat) const
{
  AffineTransform2D tmp;
  tmp.x1 = x1 * mat.x1 + x2 * mat.y1 + xt * mat.w1;
  tmp.x2 = x1 * mat.x2 + x2 * mat.y2 + xt * mat.w2;
  tmp.xt = x1 * mat.xt + x2 * mat.yt + xt * mat.wt;

  tmp.y1 = y1 * mat.x1 + y2 * mat.y1 + yt * mat.w1;
  tmp.y2 = y1 * mat.x2 + y2 * mat.y2 + yt * mat.w2;
  tmp.yt = y1 * mat.xt + y2 * mat.yt + yt * mat.wt;

  tmp.w1 = w1 * mat.x1 + w2 * mat.y1 + wt * mat.w1;
  tmp.w2 = w1 * mat.x2 + w2 * mat.y2 + wt * mat.w2;
  tmp.wt = w1 * mat.xt + w2 * mat.yt + wt * mat.wt;
  return tmp;
}

Point2i AffineTransform2D::operator*(const Point2i& p) const
{
  Point2i tmp;
  tmp.x = (int)(x1 * (Double)p.x + x2 * (Double)p.y + xt);
  tmp.y = (int)(y1 * (Double)p.x + y2 * (Double)p.y + yt);
  return tmp;
}

Point2d AffineTransform2D::operator*(const Point2d& p) const
{
  Point2d tmp;
  tmp.x = (Double)(x1 * (Double)p.x + x2 * (Double)p.y + xt);
  tmp.y = (Double)(y1 * (Double)p.x + y2 * (Double)p.y + yt);
  return tmp;
}

