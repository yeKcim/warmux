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
 * Point.h:    Standard C++ 2D Point template
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 *****************************************************************************/

#ifndef _POINT_H
#define _POINT_H

#include "vector2.h"

typedef Vector2<int>    Point2i;
typedef Vector2<float>  Point2f;
typedef Vector2<double> Point2d;

#define POINT2D_2_POINT2I(point) \
  Point2i((int)point.x, (int)point.y)

#define POINT2I_2_POINT2D(point) \
  Point2d((double)point.x, (double)point.y)

#endif // _POINT_H
