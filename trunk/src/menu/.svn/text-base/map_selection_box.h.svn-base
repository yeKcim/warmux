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
 * Map selection box
 *****************************************************************************/

#ifndef MAP_SELECTION_BOX_H
#define MAP_SELECTION_BOX_H

#include <vector>

#include <WARMUX_base.h>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>

#include "graphic/surface.h"
#include "gui/vertical_box.h"

// Forward declarations
class Label;
class PictureWidget;
class InfoMap;
class ItemBox;

class MapSelectionBox : public VBox
{
  ItemBox *box;
  uint selected_map_index;
  bool display_only;
  bool selectable;

  Label *map_name_label;
  Label *map_author_label;
  Surface random_map_preview;

  void RefreshBox();
  void ChangeMap(uint index);
  void UpdateMapInfo();

  std::vector<InfoMap*> common;

 public:
  MapSelectionBox(const Point2i &size, bool show_border, bool _display_only = false);

  void ValidMapSelection();
  void ChangeMapCallback();
  void ChangeMapListCallback(const std::vector<uint>& list);
  void AllowSelection();

  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
  virtual void Pack();
  virtual bool Update(const Point2i & mousePosition,
                      const Point2i & lastMousePosition);
};


#endif
