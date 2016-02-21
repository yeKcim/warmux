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
 * Math tools
 *****************************************************************************/

#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

#include <WARMUX_types.h>

// Template to force a value into a range.
template <class T>
T BorneTpl(const T &valeur, const T &min, const T &max){
  if (valeur < min)
    return min;
  else if (max < valeur)
    return max;
  else
    return valeur;
}

// Force a value into range [min;max]
inline long InRange_Long(const long &valeur, const long &min, const long &max) {
  return BorneTpl(valeur, min, max);
}
inline Double InRange_Double(const Double &valeur, const Double &min, const Double &max) {
  return BorneTpl(valeur, min, max);
}

// Invert an angle along the vertical axis
inline Double InverseAngle(const Double &angle) {
  if (angle < 0)
    return -PI -angle;
  return PI - angle;
}
inline float InverseAngle(const float angle) {
  if (angle < 0)
    return (float)-M_PI -angle;
  return (float)M_PI - angle;
}

inline Double InverseAngleDeg(const Double &angle) {
  if (angle < 0)
    return -180 - angle;
  return 180 - angle;
}

inline Double InverseAngleRad(const Double &angle) { return InverseAngle(angle); }
inline float InverseAngleRad(const float angle) { return InverseAngle(angle); }

inline Double RestrictAngle(Double angle)
{
  while (angle >= TWO_PI)
    angle -= TWO_PI;
  while (angle < ZERO)
    angle += TWO_PI;
  return angle;
}

// Absolute value
inline Double AbsoluteValue(const Double x) { return x >= 0 ? x : -x; }
inline bool EqualsZero(const Double x) { return AbsoluteValue(x) <= EPSILON; }

template <class T> T max(T a, T b)
{
  return a > b ? a : b;
};

#endif
