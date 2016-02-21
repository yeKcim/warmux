/******************************************************************************
 *  Wormux is a convivial mass murder game.
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

#include "widget.h"
#include "../tool/point.h"

Widget::Widget()
{
  have_focus = false;
  ct = NULL;

  need_redrawing = true;
}

Widget::Widget(const Rectanglei &rect)
{
  position = rect.GetPosition();
  size = rect.GetSize();
  have_focus = false;
  ct = NULL;
  
  need_redrawing = true;
}

Widget::~Widget()
{
}

void Widget::SendKey(SDL_keysym key)
{
}

Widget* Widget::Clic(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  return this;
}

void Widget::StdSetSizePosition(const Rectanglei &rect)
{
  position = rect.GetPosition();
  size = rect.GetSize();
}

void Widget::SetContainer( Container * _ct)
{
  ct = _ct;
}
 
void Widget::Update(const Point2i &mousePosition, 
		    const Point2i &lastMousePosition,
		    Surface& surf)
{
  if ( 
      need_redrawing 
      || (Contains(mousePosition) && mousePosition != lastMousePosition) 
      || (Contains(lastMousePosition) && !Contains(mousePosition))
      ) 
    {
      if (ct != NULL) ct->Redraw(*this, surf);
      
      Draw(mousePosition, surf);
    }
  need_redrawing = false;
}

void Widget::ForceRedraw()
{
  need_redrawing = true;
}
