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

#ifndef EULER_VECTOR_H
#define EULER_VECTOR_H

class EulerVector{
  public:
    double x0, x1, x2;

    EulerVector() :
                  x0(0.0), x1(0.0), x2(0.0)
                { };
    EulerVector(double v0, double v1, double v2) :
                  x0(v0), x1(v1), x2(v2)
                { };
    void Clear();
    void ComputeOneEulerStep(double a, double b, double c, double d, double dt);
};

#endif
