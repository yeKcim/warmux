/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "../tool/math_tools.h"
#include "../tool/Point.h"
//-----------------------------------------------------------------------------
#include <math.h>
//-----------------------------------------------------------------------------

// Limit under which, real numbers are considered as NULL
const double EPS_ZERO = 0.05;

// Set a vector to 0
void SetEulerVectorZero (EulerVector &vector)
{
  vector.x0 = 0.0 ;
  vector.x1 = 0.0 ;
  vector.x2 = 0.0 ;
}

//-----------------------------------------------------------------------------

/* Solve a.x'' + b.x' + c.x = d equation using Euler method */
void ComputeOneEulerStep(EulerVector &V, double a, double b, double c, double d, double dt)
{
  V.x2 = (d - b * V.x1 - c * V.x0)/a ;
  V.x1 = V.x1 + V.x2 * dt ;
  V.x0 = V.x0 + V.x1 * dt ;
}

//-----------------------------------------------------------------------------

// Set a vector to 0
void SetVectorZero (DoubleVector &vector)
{
  vector.x = 0 ;
  vector.y = 0 ;
}


//-----------------------------------------------------------------------------

// Init a vector.
void InitVector (DoubleVector &vector, double x, double y)
{
  vector.x = x ;
  vector.y = y ;
}


//-----------------------------------------------------------------------------

// Check if a vector is NULL
bool VectorNull (DoubleVector vector)
{
  return (EgalZero(vector.x) && EgalZero(vector.y));
}


//-----------------------------------------------------------------------------

// Add 2 vectors : A = A+B
void AddVector (DoubleVector &A, DoubleVector B)
{
  A.x += B.x ;
  A.y += B.y ;
}


//-----------------------------------------------------------------------------

// Norm of a vector
double Norm (DoubleVector vector)
{
  return hypot(vector.x, vector.y);
}

//-----------------------------------------------------------------------------

// Conversion degré en radian
double Deg2Rad (int degre)
{ return ((double)degre)*M_PI/180; }

//-----------------------------------------------------------------------------

// Modèle pour borner une valeur entre min et max
template <class T>
T BorneTpl (const T &valeur, const T &min, const T &max)
{
  if (valeur < min)
    return min;
  else if (max < valeur)
    return max;
  else
    return valeur;
}

//-----------------------------------------------------------------------------

long BorneLong (const long &valeur, const long &min, const long &max)
{ return BorneTpl (valeur, min, max); }

double BorneDouble (const double &valeur, const double &min, const double &max)
{ return BorneTpl (valeur, min, max); }

//-----------------------------------------------------------------------------

// Calcule l'angle en radian du point M dans le repère de centre O
// Pour O=(0,0) :
// - M=(10,10) -> PI/4 (0.78)
// - M=(0,10) -> PI/2 (1.57)
// - M=(-10,10) -> 3*PI/4 (2.35)
// - M=(10,-10) -> -PI/4 (-0.78)
// - M=O -> 0
double CalculeAngle (const Point2i &O, const Point2i &M)  
{
  int x = M.x - O.x;
  int y = M.y - O.y;
  double angle;

  if (!EgalZero(x)) {
    if (!EgalZero(y)) {
      angle = atan((double)y/(double)x);
      if (0 < x) {
	angle = angle;
      } else {
	if (0 < y){
	  angle += M_PI;
	} else {
	  angle -= M_PI;
	}
      }
    } else {
      if (0 < x)
	angle = 0;
      else
	angle = M_PI;
    }
  } else {
    if (0 < y) {
      angle = M_PI/2;
    } else if (y < 0) {
      angle = -M_PI/2;
    } else { // x=y=0
      angle = 0.0;      
    }
  }
  return angle;
}

double CalculeAnglef (DoubleVector V)
{
  double angle;

  if (!EgalZero(V.x))
    {
      if (!EgalZero(V.y))
	{
	  angle = atan((double)V.y/V.x);
	  if (0 < V.x)
	    angle = angle;
	  else
	    {
	      if (0 < V.y)
		angle += M_PI;
	      else
		angle -= M_PI;
	    }
	}
      else
	{
	  if (0 < V.x)
	    angle = 0;
	  else
	    angle = M_PI;
	}
    }
  else
    {
      if (0 < V.y)
	{
	  angle = M_PI/2;
	} 
      else
	if (V.y < 0)
	  angle = -M_PI/2;
	else
	  angle = 0.0;      
    }
  return angle;
}


//-----------------------------------------------------------------------------

// Inverse un angle par rapport à l'axe vertical
double InverseAngle (const double &angle)
{
  if (angle < 0)
    return -M_PI - angle;
  else
    return M_PI - angle;
}

//-----------------------------------------------------------------------------

// Inverse un angle par rapport à l'axe vertical
double InverseAngleDeg (const double &angle)
{
  if (angle < 0)
    return -180 - angle;
  else
    return 180 - angle;
}

//-----------------------------------------------------------------------------

double AbsReel (const double x) { return fabs(x); }
bool EgalZero (const double x) { return AbsReel(x) <= EPS_ZERO; }

//-----------------------------------------------------------------------------
//Arrondit un double en int à la valeur la plus proche,
//plutôt qu'à la valeur inférieur avec une conversion
//normale
// ex:      (int)4.8 == 4
//     Arrondit(4.8) == 5
int Arrondit(double x)
{
  int reponse;
  reponse = (int)x;
  x -=(double)reponse;
  if(x>0.5)
    reponse++;
  else
  if(x<=-0.5)
    reponse--;
  return reponse;
}
