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
 * Vertical or Horizontal Box
 *****************************************************************************/

#ifndef GUI_BOX_H
#define GUI_BOX_H

#include "gui/widget_list.h"
#include "tool/resource_manager.h"

class Box : public WidgetList
{
protected:
  uint    margin;

public:
  Box(void);
  Box(const Point2i & size,
      bool _draw_border = true,
      bool shadowed = true);
  Box(Profile * _profile,
      const xmlNode * _boxNode);
  virtual ~Box();

  void ParseXMLBoxParameters(void);

  void SetMargin(uint _margin) { margin = _margin; };
  uint GetMargin() { return margin; }

  virtual void Pack() = 0;
};

#endif

