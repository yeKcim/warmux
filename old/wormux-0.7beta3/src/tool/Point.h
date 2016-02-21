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
 * Point.h:    Standard C++ 2D Point template
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 *****************************************************************************/

#ifndef _POINT_H
#define _POINT_H

#include <cmath>

template<class T> class Point2
{
   
  public:
    T x, y;
   
    inline Point2(){}
    inline Point2(T x, T y) 
    { 
      this->x = x;
      this->y = y;
   }

   inline double Distance(const Point2<T> p2) const{
     double distPow2 = (p2.x-x)*(p2.x-x) + (p2.y-y)*(p2.y-y);
     return sqrt( distPow2 );
   }
};
   
typedef Point2<int>    Point2i;   
typedef Point2<float>  Point2f;   
typedef Point2<double> Point2d;   

#endif // _POINT_H
