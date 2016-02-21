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
 * Vertical or Horizontal Box
 *****************************************************************************/

#ifndef GUI_BOX_H
#define GUI_BOX_H

#include "gui/widget_list.h"

class Box : public WidgetList
{
protected:
  uint margin;
  Point2i border;

public:
  Box(const Point2i &size, bool _draw_border=true);
  virtual ~Box();

  void Update(const Point2i &mousePosition,
              const Point2i &lastMousePosition);

  Widget* Click(const Point2i &mousePosition, uint button) { return WidgetList::Click(mousePosition, button); };
  Widget* ClickUp(const Point2i &mousePosition, uint button) { return WidgetList::ClickUp(mousePosition, button); };

  void SetMargin(uint _margin) { margin = _margin; };

  void SetBorder(const Point2i &newBorder) { border = newBorder; };
  void SetBorder(uint x, uint y) { border.SetValues(x, y); };
  void SetNoBorder() { border.SetValues(0, 0); };

  virtual void Pack() = 0;
};

class VBox : public Box
{
protected:
  bool force_widget_size;

public:
  VBox(uint width, bool _draw_border=true, bool force_widget_size = true);
  virtual void Pack();
};

class HBox : public Box
{
protected:
  bool force_widget_size;

public:
  HBox(uint height, bool _draw_border=true, bool force_widget_size = true);
  virtual void Pack();
};

class GridBox : public Box
{
private:
  uint max_line_width;
  Point2i widget_size;
  uint last_line;
  uint last_column;

  uint NbWidgetsPerLine(uint nb_total_widgets);
  void PlaceWidget(Widget * a_widget, uint line, uint column);

public:
  GridBox(uint max_line_width, const Point2i& widget_size, bool _draw_border=true);
  virtual void Pack();
};
#endif

