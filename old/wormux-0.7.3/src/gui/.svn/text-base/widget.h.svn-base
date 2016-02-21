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
 * Widget
 *****************************************************************************/

#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "../include/base.h"
#include "../tool/rectangle.h"
#include "../tool/point.h"

class Widget : public Rectanglei
{
 protected:
  void StdSetSizePosition(const Rectanglei &rect);

 public:
  Widget();
  Widget(const Rectanglei &rect);
  virtual ~Widget();

  virtual void Draw(const Point2i &mousePosition) = 0;
  virtual bool Clic(const Point2i &mousePosition, uint button);

  virtual void SetSizePosition(const Rectanglei &rect) = 0;
  void SetXY(int _x, int _y){ 
	  SetSizePosition( Rectanglei(Point2i(_x, _y), size) ); 
  };
};

#endif

