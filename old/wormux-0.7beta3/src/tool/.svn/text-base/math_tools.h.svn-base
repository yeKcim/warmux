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
 * Math tools
 *****************************************************************************/

#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../tool/Point.h"
#include <string>
//-----------------------------------------------------------------------------

typedef struct { double x,y; } DoubleVector;

typedef struct { double x0,x1,x2; } EulerVector;

// Limit under which, real numbers are considered as NULL
extern const double EPS_ZERO;

// Set vector to 0.
void SetEulerVectorZero (EulerVector &vector);

/* Compute one step to solve a.x'' + b.x' + c.x = d equation using Euler method */
void ComputeOneEulerStep(EulerVector &V, double a, double b, double c, double d, double dt) ;

// Check if a vector is NULL.
bool VectorNull (DoubleVector vector) ;

// Set vector x and Y to 0.
void SetVectorZero (DoubleVector &vector);

// Init a vector.
void InitVector (DoubleVector &vector, double x, double y) ;

void AddVector (DoubleVector &A, DoubleVector B) ;

// Norm of a vector
double Norm (DoubleVector vector);

// Borne une valeur dans l'intervale [min;max]
long BorneLong (const long &valeur, const long &min, const long &max);
double BorneDouble (const double &valeur, const double &min, 
		    const double &max);

// Calcule l'angle en radian du point M dans le repère de centre O
// Pour O=(0,0) :
// - M=(10,10) -> PI/4 (0.78)
// - M=(0,10) -> PI/2 (1.57)
// - M=(-10,10) -> 3*PI/4 (2.35)
// - M=(10,-10) -> -PI/4 (-0.78)
// - M=O -> 0
double CalculeAngle (const Point2i &O, const Point2i &M);
double CalculeAnglef (DoubleVector V) ;

// Inverse un angle par rapport à l'axe vertical
double InverseAngle (const double &angle);
double InverseAngleDeg (const double &angle);

// Conversion degré en radian
double Deg2Rad (int degre);

// Valeur absolue
double AbsReel (const double x);
bool EgalZero (const double x);

// Contertit un double en int en arrondissant à la valeur la plus proche
int Arrondit(double x);
//-----------------------------------------------------------------------------
#endif
