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
 * Vertical Box
 *****************************************************************************/

#include "gui/vertical_box.h"

VBox::VBox(uint width, bool draw_border, bool shadowed, bool _force_widget_size)
  : Box(Point2i(width, 100), draw_border, shadowed)
  , force_widget_size(_force_widget_size)
{
}

VBox::VBox(Profile * profile,
           const xmlNode * verticalBoxNode) :
  Box(profile, verticalBoxNode),
  force_widget_size(false)
{
}

void VBox::Pack()
{
  uint _y = position.y;
  uint max_size_x = 0;

  std::list<Widget *>::iterator it;
  for (it = widget_list.begin(); it != widget_list.end(); ++it) {
    Widget *w = *it;

    if (it == widget_list.begin())
      _y += border_size - margin;

    w->SetPosition(position.x + border_size, _y + margin);

    if (force_widget_size) {
      w->SetSize(size.x - 2*border_size, w->GetSizeY());
    } else {
      max_size_x = std::max(max_size_x, uint(w->GetSizeX()));
    }

    w->Pack();

    _y = w->GetPositionY() + w->GetSizeY();
  }

  size.y = _y - position.y + border_size;

  if (!force_widget_size) {
    size.x = max_size_x + 2*border_size;
  }
}

bool VBox::LoadXMLConfiguration(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  ParseXMLGeometry();
  ParseXMLBoxParameters();

  //TODO

  return true;
}
