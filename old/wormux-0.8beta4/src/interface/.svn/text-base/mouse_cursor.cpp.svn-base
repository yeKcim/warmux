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
 * Mouse cursor
 *****************************************************************************/

#include "interface/mouse_cursor.h"
#include "tool/resource_manager.h"

MouseCursor::MouseCursor(Mouse::pointer_t _pointer_id, 
			 std::string filename, Point2i _clic_pos)
{
  pointer_id = _pointer_id;
  surf.ImgLoad(filename);
  clic_pos = _clic_pos;
}

const Mouse::pointer_t MouseCursor::GetPointerId() const
{
  return pointer_id;
}
  
const Surface& MouseCursor::GetSurface() const
{
  return surf;
}

const Point2i& MouseCursor::GetClicPos() const
{
  return clic_pos;
}

