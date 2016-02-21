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
 * Outils mathématiques.
 *****************************************************************************/

#include "tool/math_tools.h"
#include <math.h>

// Limit under which, real numbers are considered as NULL
const double EPS_ZERO = 0.05;

// Conversion degré en radian
double Deg2Rad (int degre){
  return ((double)degre)*M_PI/180;
}

int Rad2Deg(double rad) {
  return int(rad*180/M_PI);
}

// Modèle pour borner une valeur entre min et max
template <class T>
T BorneTpl (const T &valeur, const T &min, const T &max){
  if (valeur < min)
    return min;
  else if (max < valeur)
    return max;
  else
    return valeur;
}

long BorneLong (const long &valeur, const long &min, const long &max){
        return BorneTpl (valeur, min, max);
}

double BorneDouble (const double &valeur, const double &min, const double &max){
        return BorneTpl (valeur, min, max);
}

// Inverse un angle par rapport à l'axe vertical
double InverseAngle (const double &angle){
  if (angle < 0)
    return -M_PI - angle;
  else
    return M_PI - angle;
}

// Inverse un angle par rapport à l'axe vertical
double InverseAngleRad (const double &angle){
  if (angle < 0)
    return -M_PI - angle;
  else
    return M_PI - angle;
}

// Inverse un angle par rapport à l'axe vertical
double InverseAngleDeg (const double &angle){
  if (angle < 0)
    return -180 - angle;
  else
    return 180 - angle;
}

double AbsoluteValue (const double x){
        return fabs(x);
}

bool EqualsZero (const double x){
        return AbsoluteValue(x) <= EPS_ZERO;
}
