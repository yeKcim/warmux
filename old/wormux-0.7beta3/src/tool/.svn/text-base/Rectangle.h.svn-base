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
 * Rectangle.h: Standard C++ Rectangle template
 ******************************************************************************
 * 2005/09/21:  Jean-Christophe Duberga (jcduberga@gmx.de) 
 *              Initial version
 *****************************************************************************/

#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include <cmath>
#include "Point.h"

template<class T> class Wormux_Rectangle
{
   
  public:
    T x, y, w, h;
    inline Wormux_Rectangle(){}
    inline Wormux_Rectangle(T ox, T oy, T width, T height)
    {
      this->x = ox;
      this->y = oy;
      this->w = width;
      this->h = height;
     }
   
    inline void Clip( const Wormux_Rectangle &cr)
    {
      T left    = ( x < cr.x ) ? cr.x : ( x > cr.x+cr.w ) ? cr.x+cr.w : x; 
      T right   = ( x+w < cr.x ) ? cr.x : ( x+w > cr.x+cr.w ) ? cr.x+cr.w : x+w;
      T top     = ( y < cr.y ) ? cr.y : ( y > cr.y+cr.h ) ? cr.y+cr.h : y;
      T bottom  = ( y+h < cr.y ) ? cr.y : ( y+h > cr.y+cr.h ) ? cr.y+cr.h : y+h;
	
      x = left;
      w = right-left;
      y = top;
      h = bottom-top;
    }
    
    inline bool Contains( const Point2<T> p ) const{
      return ( p.x >= x && 
               p.x <= x+w && 
               p.y >= y && 
               p.y <= y+h );
    }

    inline bool Intersect( const Wormux_Rectangle<T> r2 ) const{
      return Contains( Point2<T>( r2.x, r2.y))
	  || Contains( Point2<T>( r2.x, r2.y+r2.h))
	  || Contains( Point2<T>( r2.x+r2.w, r2.y+r2.h))
	  || Contains( Point2<T>( r2.x+r2.w, r2.y));
    }
};

typedef Wormux_Rectangle<int>    Rectanglei;
typedef Wormux_Rectangle<float>  Rectanglef;
typedef Wormux_Rectangle<double> Rectangled;
#endif // _RECTANGLE_H
