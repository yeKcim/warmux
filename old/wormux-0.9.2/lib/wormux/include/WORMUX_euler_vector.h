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
 *****************************************************************************/

#ifndef WORMUX_EULER_VECTOR_H
#define WORMUX_EULER_VECTOR_H

#include <WORMUX_types.h>

class EulerVector{
  public:
    Double x0, x1, x2;

    EulerVector() :
                  x0(0.0), x1(0.0), x2(0.0)
                { };
    EulerVector(Double v0, Double v1, Double v2) :
                  x0(v0), x1(v1), x2(v2)
                { };
    void Clear();
    void ComputeOneEulerStep(Double a, Double b, Double c, Double d, Double dt);
};

#endif
