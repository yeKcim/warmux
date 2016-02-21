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
 *****************************************************************************/

#include "tool/euler_vector.h"

/**
 *  Set the euler vector to (0, 0, 0).
 */
void EulerVector::Clear(){
  x0 = 0.0;
  x1 = 0.0;
  x2 = 0.0;
}

/**
 *  Solve a.x'' + b.x' + c.x = d equation using Euler method.
 */
void EulerVector::ComputeOneEulerStep(double a, double b, double c, double d, double dt){
  x2 = (d - b * x1 - c * x0) / a;
  x1 = x1 + x2 * dt;
  x0 = x0 + x1 * dt;
}

