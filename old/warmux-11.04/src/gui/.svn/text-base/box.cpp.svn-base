/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Generic Box
 *****************************************************************************/

#include "gui/box.h"
#include "graphic/surface.h"
#include "graphic/colors.h"
#include "graphic/video.h"

Box::Box(void)
  : margin(0)
{
}

Box::Box(const Point2i & size, bool _draw_border, bool shadowed)
  : WidgetList(size)
  , margin(5)
{
  border_size = 5;

  if (_draw_border) {
    Widget::SetBorder(defaultOptionColorRect, 2);
  }

  if (shadowed) {
    Widget::SetBackgroundColor(defaultOptionColorBox);
  }
}

Box::Box(Profile * profile,
         const xmlNode * boxNode) :
  WidgetList(profile, boxNode),
  margin(5)
{
}

Box::~Box()
{
}

void Box::ParseXMLBoxParameters()
{
  ParseXMLBorder();
  ParseXMLBackground();
}
