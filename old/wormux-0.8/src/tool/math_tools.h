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
 * Math tools
 *****************************************************************************/

#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

#include <math.h>

// Limit under which, real numbers are considered as NULL
#define EPS_ZERO 0.05;

// Template to force a value into a range.
template <class T>
T BorneTpl (const T &valeur, const T &min, const T &max){
  if (valeur < min)
    return min;
  else if (max < valeur)
    return max;
  else
    return valeur;
}

// Force a value into range [min;max]
long inline InRange_Long (const long &valeur, const long &min, const long &max) {
  return BorneTpl (valeur, min, max);
}
double inline InRange_Double (const double &valeur, const double &min, const double &max) {
  return BorneTpl (valeur, min, max);
}

// Invert an angle along the vertical axis
double inline InverseAngle (const double &angle) {
  if (angle < 0)
    return -M_PI -angle;
  return M_PI - angle;
}

double inline InverseAngleDeg (const double &angle) {
  if (angle < 0)
    return -180 - angle;
  return 180 - angle;
}

double inline InverseAngleRad (const double &angle) { return InverseAngle(angle); }

// Absolute value
double inline AbsoluteValue (const double x) { return fabs(x); }
bool inline EqualsZero (const double x) { return AbsoluteValue(x) <= EPS_ZERO; }

#ifdef _MSC_VER
// MIT licensed from http://opensource.adobe.com/cmath_8hpp-source.html
#  include <float.h>
double inline round(double a) { return (a<0.0) ? ceil(a-0.5) : floor(a+0.5); };
long int inline lround(double a) { return static_cast<long>(a + (a < 0.0 ? -0.5 : 0.5)); }
#endif

template <class T> T max(T a, T b)
{
  return a > b ? a : b;
};

#endif

