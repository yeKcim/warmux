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
 * Widget list : store all widgets displayed on one screen
 * It is a fake widget
 *****************************************************************************/
#ifndef WIDGET_LIST_H
#define WIDGET_LIST_H

#include <list>
#include "widget.h"
#include "container.h"

class WidgetList : public Widget
{
private:
  /* If you need this, implement it (correctly)*/
  WidgetList(const WidgetList&);
  WidgetList operator=(const WidgetList&);
  /*********************************************/

  Point2i lastMousePosition;
  Widget* selected_widget;

protected:
  std::list<Widget*> widget_list;
  virtual void DelFirstWidget(); // usefull only for message_box

public:
  WidgetList();
  WidgetList(const Point2i &size);
  virtual ~WidgetList();

  virtual void Update(const Point2i &mousePosition);
  virtual void Draw(const Point2i &mousePosition) const;
  // set need_redrawing to true for all sub widgets;
  virtual void NeedRedrawing();

  // methods specialized from Widget to manage the list of widgets
  virtual bool SendKey(SDL_keysym key);
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // to add a widget
  void AddWidget(Widget* widget);
  void RemoveWidget(Widget* w);

  // Navigate between widget with keyboard
  virtual void SetFocusOnNextWidget();
  virtual void SetFocusOnPreviousWidget();
  Widget * GetCurrentKeyboardSelectedWidget() const { return selected_widget; };

  // to implement WidgetBrowser
  virtual Widget* GetFirstWidget() const;
  virtual Widget* GetLastWidget() const;
  virtual Widget* GetNextWidget(const Widget *w, bool loop) const;
  virtual Widget* GetPreviousWidget(const Widget *w, bool loop) const;
  virtual bool IsWidgetBrowser() const { return true; };

  void SetFocusOn(Widget* widget, bool force_mouse_position = false);
  virtual void Pack();
};

#endif // WIDGET_LIST_H
