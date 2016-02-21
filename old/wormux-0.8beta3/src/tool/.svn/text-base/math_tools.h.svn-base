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
 * Math tools
 *****************************************************************************/

#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

// Limit under which, real numbers are considered as NULL
extern const double EPS_ZERO;

// Force a value into range [min;max]
long InRange_Long (const long &valeur, const long &min, const long &max);
double InRange_Double (const double &valeur, const double &min,
                    const double &max);

// Invert an angle along the vertical axis
double InverseAngle (const double &angle);
double InverseAngleDeg (const double &angle);
double InverseAngleRad (const double &angle);

// degree<->radian angle conversion
double Deg2Rad (int degre);
int Rad2Deg(double rad);

// Absolute value
double AbsoluteValue (const double x);
bool EqualsZero (const double x);

#ifdef _MSC_VER
// MIT licensed from http://opensource.adobe.com/cmath_8hpp-source.html
#  include <math.h>
#  include <float.h>
double inline round(double a) { return (a<0.0) ? ceil(a-0.5) : floor(a+0.5); };
long int inline lround(double a) { return static_cast<long>(a + (a < 0.0 ? -0.5 : 0.5)); }
#endif

template <class T> T max(T a, T b)
{
  return a > b ? a : b;
};

#endif
