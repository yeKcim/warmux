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
 * Vertical or Horizontal Box
 *****************************************************************************/

#ifndef GUI_BOX_H
#define GUI_BOX_H

#include "widget_list.h"
#include <list>
#include "tool/point.h"

class Box : public WidgetList
{
 protected:
  bool visible;
  uint margin;
  Point2i border;

 public:
  Box(const Rectanglei &rect, bool _visible=true);
  virtual ~Box();

  void Update(const Point2i &mousePosition,
	      const Point2i &lastMousePosition,
	      Surface& surf);
  void Draw(const Point2i &mousePosition,
	    Surface& surf) const;
  void Redraw(const Rectanglei& rect,
	      Surface& surf);
  Widget* Click(const Point2i &mousePosition, uint button);
  Widget* ClickUp(const Point2i &mousePosition, uint button);

  void SetMargin(uint _margin);
  void SetBorder(const Point2i &newBorder);

  virtual void AddWidget(Widget *a_widget) = 0;
};

class VBox : public Box
{
 public:
  VBox(const Rectanglei &rect, bool _visible=true);
  void DelFirstWidget();
  void SetSizePosition(const Rectanglei &rect);
  void AddWidget(Widget *a_widget);
};

class HBox : public Box
{
 public:
  HBox(const Rectanglei &rect, bool _visible=true);
  void SetSizePosition(const Rectanglei &rect);
  void AddWidget(Widget *a_widget);
};

#endif

