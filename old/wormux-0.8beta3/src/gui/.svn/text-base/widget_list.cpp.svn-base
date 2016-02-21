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
 * Widget list : store all widgets displayed on one screen
 * It is a fake widget.
 *****************************************************************************/
#include <SDL_keyboard.h>
#include "gui/widget_list.h"
#include "gui/widget.h"
#include <iostream>

WidgetList::WidgetList()
{
  last_clicked = NULL;
  keyboard_selection = NULL;
  mouse_selection = NULL;
}

WidgetList::WidgetList(const Rectanglei &rect) : Widget(rect)
{
  last_clicked = NULL;
  keyboard_selection = NULL;
  mouse_selection = NULL;
}

WidgetList::~WidgetList()
{
  for(std::list<Widget*>::iterator w=widget_list.begin();
      w != widget_list.end();
      w++)
    delete *w;

  widget_list.clear();
}

void WidgetList::DelFirstWidget()
{
  delete widget_list.front();
  widget_list.pop_front();
}

void WidgetList::AddWidget(Widget* w)
{
  ASSERT(w!=NULL);
  widget_list.push_back(w);
  w->SetContainer(this);
}

void WidgetList::Update(const Point2i &mousePosition, Surface& surf)
{
  if(keyboard_selection != NULL && lastMousePosition != mousePosition) {
    keyboard_selection->Unselect();
    keyboard_selection = NULL;
  }
  for(std::list<Widget*>::iterator w=widget_list.begin();
      w != widget_list.end();
      w++)
  {
    // Then redraw the widget
    (*w)->Update(mousePosition, lastMousePosition, surf);
    if(lastMousePosition != mousePosition && (*w)->Contains(mousePosition)) {
      mouse_selection = (*w);
    }
  }
  if(mouse_selection != NULL && keyboard_selection != NULL && lastMousePosition != mousePosition) {
    keyboard_selection->Unselect();
    keyboard_selection = NULL;
  }
  lastMousePosition = mousePosition;
}

void WidgetList::SetFocusOnNextWidget()
{
  if(mouse_selection != NULL && mouse_selection->Contains(lastMousePosition))
    return;
  // No widget => exit
  if(widget_list.size() == 0) {
    keyboard_selection = NULL;
    return;
  }
  // Previous selection ?
  if(keyboard_selection != NULL)
    keyboard_selection->Unselect();
  else {
    keyboard_selection = (*widget_list.begin());
    keyboard_selection->Select();
    return;
  }
  std::list<Widget*>::iterator w = widget_list.begin();
  for(;  w != widget_list.end(); w++) {
    if(keyboard_selection == (*w))
      break;
  }
  w++;
  // The next widget is not the end ?
  if(w != widget_list.end()) {
    keyboard_selection = (*w);
  } else {
    keyboard_selection = (*widget_list.begin());
  }
  keyboard_selection->Select();
}

void WidgetList::SetFocusOnPreviousWidget()
{
  if(mouse_selection != NULL && mouse_selection->Contains(lastMousePosition))
    return;
  Widget * previous_one = NULL;
  // No widget => exit
  if(widget_list.size() == 0) {
    keyboard_selection = NULL;
    return;
  }
  // Previous selection ?
  if(keyboard_selection != NULL)
    keyboard_selection->Unselect();
  else {
    keyboard_selection = (*widget_list.begin());
    keyboard_selection->Select();
    return;
  }
  std::list<Widget*>::iterator w = widget_list.begin();
  for(;  w != widget_list.end(); w++) {
    if(keyboard_selection == (*w))
      break;
    previous_one = (*w);
  }
  // The next widget is not the end ?
  if(previous_one == NULL) {
    w = widget_list.end(); w--;
    keyboard_selection = (*w);
  } else {
    keyboard_selection = previous_one;
  }
  keyboard_selection->Select();
}

void WidgetList::Redraw(const Rectanglei& rect, Surface& surf)
{
  // Redraw bottom layer
  if (ct != NULL) {
    ct->Redraw(rect, surf);
  }
}

bool WidgetList::SendKey(SDL_keysym key)
{
  if (last_clicked != NULL)
    return last_clicked->SendKey(key);

  return false;
}

Widget* WidgetList::ClickUp(const Point2i &mousePosition, uint button)
{
  for(std::list<Widget*>::iterator w=widget_list.begin();
      w != widget_list.end();
      w++)
  {
    if((*w)->Contains(mousePosition))
    {
      Widget* child = (*w)->ClickUp(mousePosition,button);
      if(child != NULL)
      {
        SetFocusOn(child);
        return child;
      }
    }
  }
  return NULL;
}

Widget* WidgetList::Click(const Point2i &mousePosition, uint button)
{
  for(std::list<Widget*>::iterator w=widget_list.begin();
      w != widget_list.end();
      w++)
  {
    if((*w)->Contains(mousePosition))
    {
      (*w)->Click(mousePosition,button);
    }
  }
  return NULL;
}

void WidgetList::ForceRedraw()
{
  need_redrawing = true;

  for(std::list<Widget*>::iterator w=widget_list.begin();
      w != widget_list.end();
      w++)
  {
    (*w)->ForceRedraw();
  }
}

void WidgetList::SetFocusOn(Widget* w)
{
  if(last_clicked != NULL) {
    last_clicked->have_focus = false;
    last_clicked->ForceRedraw();
  }

  if (w != NULL) {
    last_clicked = w ;
    last_clicked->have_focus = true;
    last_clicked->ForceRedraw();
  }
}
