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
 ******************************************************************************
 * Mouse cursor
 *****************************************************************************/

#ifndef MOUSE_CURSOR_H
#define MOUSE_CURSOR_H

#include "graphic/surface.h"
#include "interface/mouse.h"
#include "tool/point.h"

class MouseCursor
{
private:
  Mouse::pointer_t pointer_id;
  Surface surf;
  // relative position for the clic pixel
  Point2i clic_pos;

public:
  MouseCursor(Mouse::pointer_t pointer_id, 
	      std::string filename, Point2i clic_pos);

  Mouse::pointer_t GetPointerId() const;
  const Surface& GetSurface() const;
  const Point2i& GetClicPos() const;
};

#endif
