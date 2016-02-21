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

#include "gui/box.h"
#include "graphic/surface.h"
#include "graphic/colors.h"

Box::Box(const Rectanglei &rect, bool _visible) : WidgetList( rect )
{
  margin = 5;
  border.SetValues(5, 5);

  if (_visible) {
    Widget::SetBorder(defaultOptionColorRect, 2);
    Widget::SetBackgroundColor(defaultOptionColorBox);
  }
}

Box::~Box()
{
}

void Box::Update(const Point2i &mousePosition,
		   const Point2i &/*lastMousePosition*/,
		   Surface& surf)
{
  if (need_redrawing) {
    Widget::RedrawBackground(*this, surf);
    Draw(mousePosition, surf);
  }

  WidgetList::Update(mousePosition, surf);
  need_redrawing = false;
}

// --------------------------------------------------

VBox::VBox(int width, bool _visible) : 
  Box(Rectanglei(-1, -1, width, -1), _visible)
{
}

void VBox::DelFirstWidget()
{
  int w_height = widget_list.front()->GetSizeY();
  WidgetList::DelFirstWidget();
  //Make all remaining widget go up:
  for( std::list<Widget*>::iterator it = widget_list.begin();
       it != widget_list.end();
       ++it )
  {
    (*it)->SetPositionY((*it)->GetPositionY() - w_height - margin);
  }
  size.y -= w_height + margin;
}

void VBox::AddWidget(Widget * a_widget)
{
  ASSERT(a_widget != NULL);

  uint _y;

  if(!widget_list.empty())
    _y = widget_list.back()->GetPositionY() + widget_list.back()->GetSizeY();
  else
    _y = position.y + border.y - margin;

  a_widget->SetSizePosition(Rectanglei(position.x + border.x,
                                       _y + margin,
                                       size.x - 2 * border.x,
                                       a_widget->GetSizeY() ));

  size.y = a_widget->GetPositionY() + a_widget->GetSizeY() - position.y + border.y;
  WidgetList::AddWidget(a_widget);
}

void VBox::SetSizePosition(const Rectanglei &rect)
{
  position = rect.GetPosition();
  int _y = rect.GetPositionY();
  std::list<Widget *>::iterator it;
  for( it = widget_list.begin();
       it != widget_list.end();
       ++it ){

    if( it == widget_list.begin() )
      _y += border.y - margin;

    (*it)->SetSizePosition( Rectanglei(position.x + border.x,
                                       _y + margin,
                                       (*it)->GetSizeX(),
                                       (*it)->GetSizeY() ));
    _y = (*it)->GetPositionY() + (*it)->GetSizeY();
  }
}

// --------------------------------------------------

HBox::HBox(int height, bool _visible) : 
  Box(Rectanglei(-1, -1, -1, height), _visible)
{
}

void HBox::AddWidget(Widget * a_widget)
{
  ASSERT(a_widget != NULL);

  uint _x;

  if (!widget_list.empty())
    _x = widget_list.back()->GetPositionX() + widget_list.back()->GetSizeX();
  else
    _x = position.x + border.x - margin;

  a_widget->SetSizePosition( Rectanglei(_x + margin,
                                        position.y + border.y,
                                        a_widget->GetSizeX(),
                                        size.y - 2 * border.y) );

  size.x = a_widget->GetPositionX() + a_widget->GetSizeX() - position.x + border.x;

  WidgetList::AddWidget(a_widget);
}

void HBox::SetSizePosition(const Rectanglei &rect)
{
  position = rect.GetPosition();
  int _x = rect.GetPositionX();

  std::list<Widget *>::iterator it;
  for( it = widget_list.begin();
       it != widget_list.end();
       ++it ){

    if( it == widget_list.begin() )
      _x += border.x - margin;

    (*it)->SetSizePosition( Rectanglei(_x + margin,
                                       position.y + border.y,
                                       (*it)->GetSizeX(),
                                       (*it)->GetSizeY()) );
    _x = (*it)->GetPositionX()+ (*it)->GetSizeX();
  }
}

