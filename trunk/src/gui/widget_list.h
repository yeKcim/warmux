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
public:
  typedef std::list<Widget*>::iterator wit;
  typedef std::list<Widget*>::const_iterator cwit;
  typedef std::list<Widget*>::reverse_iterator rwit;
  typedef std::list<Widget*>::const_reverse_iterator crwit;

private:
  Widget* selected_widget;

protected:
  std::list<Widget*> widget_list;
  virtual void DelFirstWidget(); // usefull only for message_box

public:
  WidgetList();
  WidgetList(const Point2i &size);
  WidgetList(Profile * profile,
             const xmlNode * widgetListNode);
  virtual ~WidgetList();

  // Highlight and background
  virtual void SetVisible(bool _visible);
  virtual void SetHighlighted(bool focus);
  virtual void SetBackgroundColor(const Color &background_color);
  virtual void SetHighlightBgColor(const Color &highlight_bg_color);
  virtual void SetSelfBackgroundColor(const Color &background_color);
  virtual void SetSelfHighlightBgColor(const Color &highlight_bg_color);

  virtual bool Update(const Point2i &mousePosition,
                      const Point2i &lastMousePosition);
  virtual void Draw(const Point2i &mousePosition);
  // set need_redrawing to true for all sub widgets;
  virtual void NeedRedrawing();

  // methods specialized from Widget to manage the list of widgets
  virtual bool SendKey(SDL_keysym key);
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // to add a widget
  virtual void AddWidget(Widget* widget);
  virtual void RemoveWidget(Widget* w);
  virtual size_t WidgetCount() const { return widget_list.size(); }
  virtual void Empty() { widget_list.clear(); }
  virtual void Clear();

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

  virtual void SetFocusOn(Widget* widget, bool force_mouse_position = false);
  virtual void Pack();
};

#endif // WIDGET_LIST_H
