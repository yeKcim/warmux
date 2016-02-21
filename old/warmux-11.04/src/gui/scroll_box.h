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
 * Vertical Scroll Box
 *****************************************************************************/

#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include "gui/widget_list.h"
#include "gui/vertical_box.h"

// Forward declaration
struct SDL_keysym;
class VBox;
class Button;

class ScrollBox : public WidgetList
{
public:
  typedef enum {
    SCROLL_MODE_NONE,
    SCROLL_MODE_THUMB,
    SCROLL_MODE_DRAG
  } ScrollMode;

protected:
  // Internal box
  VBox *vbox;
  bool alternate_colors;

  // Buttons
  Button * m_up;
  Button * m_down;

  // Scroll information
  int          start_drag_y;
  int          start_drag_offset;
  int          offset;
  int          scrollbar_width;
  ScrollMode   scroll_mode;

  virtual void __Update(const Point2i & mousePosition,
                        const Point2i & lastMousePosition);
  Rectanglei GetScrollThumb() const;
  Rectanglei GetScrollTrack() const
  {
    return Rectanglei(GetScrollTrackPos(),
                      Point2i(scrollbar_width, GetTrackHeight()));
  }
  Point2i    GetScrollTrackPos() const;
  int GetMaxOffset() const { return vbox->GetSizeY() - size.y; }
  int GetTrackHeight() const;
  bool HasScrollBar() const { return GetMaxOffset() > 0; }

public:
  ScrollBox(const Point2i & size, bool force = true, bool alternate = false);
  virtual ~ScrollBox() { };

  // No need for a Draw method: the additional stuff drawn is made by Update
  virtual bool Update(const Point2i &mousePosition,
                      const Point2i &lastMousePosition);
  virtual Widget* Click(const Point2i & mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i & mousePosition, uint button);
  virtual bool SendKey(const SDL_keysym & key);
  virtual void Pack();

  // to add a widget
  virtual bool Contains(const Point2i & point) const
  {
    return scroll_mode != SCROLL_MODE_NONE || Widget::Contains(point);
  }
  virtual void AddWidget(Widget* widget);
  virtual void RemoveWidget(Widget* w) { vbox->RemoveWidget(w); }
  virtual void RemoveFirstWidget()
  {
    Widget *w = vbox->GetFirstWidget();
    if (w) {
      RemoveWidget(w);
    }
  }
  virtual size_t WidgetCount() const { return vbox->WidgetCount(); }
  virtual void Empty() { offset = 0; vbox->Empty(); }
  virtual void Clear() { offset = 0; vbox->Clear(); }
};

#endif  //SCROLL_BOX_H
