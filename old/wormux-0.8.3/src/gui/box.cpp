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

#include "gui/box.h"
#include "graphic/surface.h"
#include "graphic/colors.h"

Box::Box(const Point2i &size, bool _draw_border) : WidgetList(size)
{
  margin = 5;
  border.SetValues(5, 5);

  if (_draw_border) {
    Widget::SetBorder(defaultOptionColorRect, 2);
    Widget::SetBackgroundColor(defaultOptionColorBox);
  }
}

Box::~Box()
{
}

void Box::Update(const Point2i &mousePosition,
		 const Point2i &/*lastMousePosition*/)
{
  if (need_redrawing) {
    Widget::RedrawBackground(*this);
    Draw(mousePosition);
  }

  WidgetList::Update(mousePosition);
  need_redrawing = false;
}

// --------------------------------------------------

VBox::VBox(uint width, bool _draw_border, bool _force_widget_size) :
  Box(Point2i(width, 100), _draw_border),
  force_widget_size(_force_widget_size)
{
}

void VBox::Pack()
{
  uint _y = position.y;
  uint max_size_x = 0;

  std::list<Widget *>::iterator it;
  for (it = widget_list.begin();
       it != widget_list.end();
       ++it) {

    if (it == widget_list.begin())
      _y += border.y - margin;

    (*it)->SetPosition(position.x + border.x,
		       _y + margin);

    if (force_widget_size) {
      (*it)->SetSize(size.x - 2*border.x,
		     (*it)->GetSizeY());
    } else {
      max_size_x = std::max(max_size_x, uint((*it)->GetSizeX()));
    }

    (*it)->Pack();

    _y = (*it)->GetPositionY() + (*it)->GetSizeY();
  }

  size.y = _y - position.y + border.y;

  if (!force_widget_size) {
    size.x = max_size_x + 2*border.x;
  }
}

// --------------------------------------------------

HBox::HBox(uint height, bool _draw_border, bool _force_widget_size) :
  Box(Point2i(100, height), _draw_border),
  force_widget_size(_force_widget_size)
{
}

void HBox::Pack()
{
  uint _x = position.x;
  uint max_size_y = 0;

  std::list<Widget *>::iterator it;
  for (it = widget_list.begin();
       it != widget_list.end();
       ++it) {

    if (it == widget_list.begin())
      _x += border.x - margin;

    (*it)->SetPosition(_x + margin,
		       position.y + border.y);

    if (force_widget_size) {
      (*it)->SetSize((*it)->GetSizeX(),
		     size.y - 2*border.y);
    } else {
      max_size_y = std::max(max_size_y, uint((*it)->GetSizeY()));
    }

    (*it)->Pack();

    _x = (*it)->GetPositionX()+ (*it)->GetSizeX();
  }
  size.x = _x - position.x + border.x;

  if (!force_widget_size) {
    size.y = max_size_y + 2*border.y;
  }
}

// --------------------------------------------------

GridBox::GridBox(uint _max_line_width, const Point2i& size_of_widget, bool _draw_border) :
  Box(Point2i(-1, -1), _draw_border)
{
  max_line_width = _max_line_width;
  widget_size = size_of_widget;
  last_line = 0;
  last_column = 0;
}

void GridBox::PlaceWidget(Widget * a_widget, uint _line, uint _column)
{
  uint _x, _y;

  _x = position.x + border.x + _column * (widget_size.GetX() + margin);
  _y = position.y + border.y + _line * (widget_size.GetY() + margin);

  a_widget->SetPosition(_x, _y);
  a_widget->SetSize(widget_size);
}

uint GridBox::NbWidgetsPerLine(const uint nb_total_widgets)
{
  uint max_nb_widgets_per_line = nb_total_widgets;

  while (max_line_width - 2 * border.x <
	 max_nb_widgets_per_line * (widget_size.GetX()+margin) - margin) {
    max_nb_widgets_per_line--;
  }

  if (max_nb_widgets_per_line < 1) {
    max_nb_widgets_per_line = 1;
  }

  uint nb_lines = nb_total_widgets / max_nb_widgets_per_line;
  if (nb_total_widgets % max_nb_widgets_per_line != 0) {
    nb_lines++;
  }

  uint nb_widgets_per_line = nb_total_widgets / nb_lines;
  if (nb_total_widgets % nb_lines != 0) {
    nb_widgets_per_line++;
  }

  return nb_widgets_per_line;
}

void GridBox::Pack()
{
  WidgetList::Pack();

  uint nb_widgets_per_line = NbWidgetsPerLine(widget_list.size());

  std::list<Widget *>::iterator it;
  uint line = 0, column = 0;
  for( it = widget_list.begin();
       it != widget_list.end();
       ++it ){

    last_line = line;
    PlaceWidget((*it), line, column);

    if (column + 1 == nb_widgets_per_line) {
      column = 0;
      line++;
    } else {
      column++;
    }

  }

  size.x = 2*border.x + nb_widgets_per_line * (widget_size.GetX() + margin) - margin;
  size.y = 2*border.y + (last_line+1) * (widget_size.GetY() + margin) - margin;

  WidgetList::Pack();
}
