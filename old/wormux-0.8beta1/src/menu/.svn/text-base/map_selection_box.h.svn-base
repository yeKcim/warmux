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
 * Map selection box
 *****************************************************************************/

#ifndef MAP_SELECTION_BOX_H
#define MAP_SELECTION_BOX_H

#include "gui/box.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/picture_widget.h"

class MapSelectionBox : public HBox
{
 private:
  /* If you need this, implement it (correctly) */
  MapSelectionBox(const MapSelectionBox&);
  MapSelectionBox operator=(const MapSelectionBox&);
  /**********************************************/

  uint selected_map_index;
  bool display_only;

  PictureWidget *map_preview_selected;
  PictureWidget *map_preview_before, *map_preview_before2;
  PictureWidget *map_preview_after, *map_preview_after2;

  Label *map_name_label;
  Label *map_author_label;
  Button *bt_map_plus, *bt_map_minus;

  void ChangeMapDelta(int delta_index);
  void ChangeMap(int index);

 public:
  MapSelectionBox(const Rectanglei &rect, bool _display_only = false);

  void ValidMapSelection();
  void ChangeMapCallback();
  Widget* Click(const Point2i &mousePosition, uint button);
  Widget* ClickUp(const Point2i &mousePosition, uint button);
};


#endif
